//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#include <oaknut.h>


Font::Font(const string& fontAssetPath, float size) : FontBase(fontAssetPath, size) {
    _ctfont = createCTFont();
    _ascent = CTFontGetAscent(_ctfont);
    _descent = -CTFontGetDescent(_ctfont); // as per Freetype convention, this is -ve.
    _leading = CTFontGetLeading(_ctfont);
    if (_height<=0) {
        _height = (_ascent-_descent) + _leading;
    }
}

CTFontRef Font::createCTFont() {
    CTFontRef ctfont = NULL;
    CGFloat size = _size;
    if (_name.length() == 0) {
#if TARGET_OS_IOS
        UIFont* font = [UIFont systemFontOfSize:size];
        _ascent = font.ascender;
        _descent = font.descender;
        _height = font.lineHeight;
        ctfont = CTFontCreateWithName((__bridge CFStringRef)font.fontName, size, NULL);
#else
        NSFont* font = [NSFont systemFontOfSize:size];
        _ascent = font.ascender;
        _descent = font.descender;
        _height = ceilf(_ascent-_descent);
        ctfont = CTFontCreateWithName((__bridge CFStringRef)font.fontName, size, NULL);
#endif
        
    } else {
        ObjPtr<ByteBuffer> data = app.loadAsset(_name.data());
        assert(data->data);
        CGDataProviderRef dataProvider = CGDataProviderCreateWithData(NULL, data->data, data->cb, NULL);
        CGFontRef cgfont = CGFontCreateWithDataProvider(dataProvider);
        assert(cgfont);
        CGDataProviderRelease(dataProvider);
        ctfont = CTFontCreateWithGraphicsFont(cgfont, size, NULL, NULL);
        CGFontRelease(cgfont);
    }
    return ctfont;
}
    

Glyph* Font::createGlyph(char32_t ch, Atlas* atlas) {
    //app.log("Creating glyph for char code %d in font %X", ch, this);
    UniChar uch = ch;
    CGGlyph cgglyph = 0;
    if (!CTFontGetGlyphsForCharacters(_ctfont, &uch, &cgglyph, 1)) {
        // Sometimes CTFontGetGlyphsForCharacters() fails for no clear reason and the only
        // workaround I can find is to recreate the CTFont and try again. Bloody Apple.
        CFRelease(_ctfont);
        _ctfont = createCTFont();
        if (!CTFontGetGlyphsForCharacters(_ctfont, &uch, &cgglyph, 1)) {
            app.warn("Glyph '%c' not in chosen font", ch);
            uch = '?';
            if (!CTFontGetGlyphsForCharacters(_ctfont, &uch, &cgglyph, 1)) {
                assert(false); // glyph is not in the font, TODO: fall back to another font
                
                return NULL;
            }
        }
        app.warn("Stupid Core Text bug caused us to waste time recreating a font");
    }
    Glyph* glyph = new Glyph(this, ch, cgglyph);
    
    // Get glyph advances
    CGSize advances;
    CTFontGetAdvancesForGlyphs(_ctfont, kCTFontOrientationDefault, &cgglyph, &advances, 1);
    glyph->advance = {(float)advances.width, (float)advances.height};
    glyph->advance.width = ceilf(glyph->advance.width);
    
    // Get the glyph bounding rect. This is idealised in that it has fractional coordinates
    // so we round up and down to whole pixel boundaries.
    CGRect boundingRect, boundingRectOrig;
    CTFontGetBoundingRectsForGlyphs(_ctfont, kCTFontOrientationDefault, &cgglyph, &boundingRectOrig, 1);
    boundingRect = CGRectIntegral(boundingRectOrig);
    
    // Keep the fractional part of the idealised rect for use during rasterization
    CGFloat ddx = boundingRectOrig.origin.x - boundingRect.origin.x;
    CGFloat ddy = boundingRectOrig.origin.y - boundingRect.origin.y;
    
    // Reserve a space in the glyph atlas
    glyph->bitmapWidth = boundingRect.size.width;
    glyph->bitmapHeight = boundingRect.size.height;
    glyph->atlasNode = atlas->reserve(glyph->bitmapWidth, glyph->bitmapHeight, 1);
    glyph->bitmapLeft = boundingRect.origin.x;
    glyph->bitmapTop = boundingRect.origin.y;
    
    // Get the atlas bitmap context
    Bitmap* bitmap = (Bitmap*)glyph->atlasNode->page->_bitmap._obj;
    CGContext* c = bitmap->_context;
    
    POINT origin = glyph->atlasNode->rect.origin;
    CGPoint pt = CGPointMake(origin.x, origin.y);
    pt.x += ddx; // apply fractional offset
    pt.y -= ddy; // apply fractional offset
    pt.x -= glyph->bitmapLeft;
    pt.y += glyph->bitmapTop;
    
    CGContextSaveGState(c);
    int h = (int)CGBitmapContextGetHeight(c);
    CGContextScaleCTM(c, 1, -1);
    CGContextTranslateCTM(c, 0, -h);
    pt.y = h-pt.y;
    pt.y -= glyph->bitmapHeight;
    CGContextSetShouldSubpixelPositionFonts(c, false);
    CGContextSetShouldSubpixelQuantizeFonts(c, false);
    CTFontDrawGlyphs(_ctfont, &cgglyph, &pt, 1, c);
    CGContextRestoreGState(c);
    
    bitmap->_needsUpload = true;
    
    return glyph;
}

#endif

