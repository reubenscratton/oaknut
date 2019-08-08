//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#include <oaknut.h>

#ifdef PLATFORM_IOS
#define NSUI(x) UI##x
#else
#define NSUI(x) NS##x
#endif

float UIFontWeightFromStandardWeight(float weight) {
    if (weight < FONT_WEIGHT_THIN) return NSUI(FontWeightUltraLight);
    if (weight < FONT_WEIGHT_LIGHT) return NSUI(FontWeightThin);
    if (weight < FONT_WEIGHT_REGULAR) return NSUI(FontWeightLight);
    if (weight < FONT_WEIGHT_MEDIUM) return NSUI(FontWeightRegular);
    if (weight < FONT_WEIGHT_SEMIBOLD) return NSUI(FontWeightMedium);
    if (weight < FONT_WEIGHT_BOLD) return NSUI(FontWeightSemibold);
    if (weight < FONT_WEIGHT_HEAVY) return NSUI(FontWeightBold);
    if (weight < FONT_WEIGHT_BLACK) return NSUI(FontWeightHeavy);
    return NSUI(FontWeightBlack);
}



static CTFontRef createCTFont(FontApple* font) {
    CTFontRef ctfont = NULL;
    CGFloat size = font->_size;
    if (font->_name.length() == 0) {
#if TARGET_OS_IOS
        UIFont* font = [UIFont systemFontOfSize:size weight:UIFontWeightFromStandardWeight(_weight)];
        _ascent = font.ascender;
        _descent = font.descender;
        _height = font.lineHeight;
        ctfont = CTFontCreateWithName((__bridge CFStringRef)font.fontName, size, NULL);
#else
        NSFont* nsfont = [NSFont systemFontOfSize:size weight:UIFontWeightFromStandardWeight(font->_weight)];
        font->_ascent = nsfont.ascender;
        font->_descent = nsfont.descender;
        font->_height = ceilf(font->_ascent-font->_descent);
        ctfont = CTFontCreateWithName((__bridge CFStringRef)nsfont.fontName, size, NULL);
#endif
        
    } else {
        bytearray data;
        app->loadAsset(font->_name.data(), data);
        assert(data.size());
        CGDataProviderRef dataProvider = CGDataProviderCreateWithData(NULL, data.data(), data.size(), NULL);
        CGFontRef cgfont = CGFontCreateWithDataProvider(dataProvider);
        assert(cgfont);
        CGDataProviderRelease(dataProvider);
        ctfont = CTFontCreateWithGraphicsFont(cgfont, size, NULL, NULL);
        CGFontRelease(cgfont);
    }
    return ctfont;

}

FontApple::FontApple(const string& fontAssetPath, float size, float weight) : Font(fontAssetPath, size, weight) {
    _ctfont = createCTFont(this);
    _ascent = CTFontGetAscent(_ctfont);
    _descent = -CTFontGetDescent(_ctfont); // as per Freetype convention, this is -ve.
    _leading = CTFontGetLeading(_ctfont);
    if (_height<=0) {
        _height = (_ascent-_descent) + _leading;
    }
}

class GlyphApple : public Glyph {
public:
    CGGlyph _cgglyph;
    POINT _dd;
    
    GlyphApple(FontApple* font, char32_t ch) : Glyph(font, ch) {
        //app->log("Creating glyph for char code %d in font %X", ch, this);
        UniChar uch = ch;
        if (!CTFontGetGlyphsForCharacters(font->_ctfont, &uch, &_cgglyph, 1)) {
            // Sometimes CTFontGetGlyphsForCharacters() fails for no clear reason and the only
            // workaround I can find is to recreate the CTFont and try again. Bloody Apple.
            CFRelease(font->_ctfont);
            font->_ctfont = createCTFont(font);
            if (!CTFontGetGlyphsForCharacters(font->_ctfont, &uch, &_cgglyph, 1)) {
                app->warn("Glyph '%c' not in chosen font", ch);
                uch = '?';
                if (!CTFontGetGlyphsForCharacters(font->_ctfont, &uch, &_cgglyph, 1)) {
                    assert(false); // glyph is not in the font, TODO: fall back to another font
                }
            }
            app->warn("Stupid Core Text bug caused us to waste time recreating a font");
        }
        
        
        // Get glyph advances
        CGSize advances;
        CTFontGetAdvancesForGlyphs(font->_ctfont, kCTFontOrientationDefault, &_cgglyph, &advances, 1);
        _advance.width = ceilf((float)advances.width);
        _advance.height = (float)advances.height;
        
        // Get the glyph bounding rect. This is idealised in that it has fractional coordinates
        // so we round up and down to whole pixel boundaries.
        CGRect boundingRect, boundingRectOrig;
        CTFontGetBoundingRectsForGlyphs(font->_ctfont, kCTFontOrientationDefault, &_cgglyph, &boundingRectOrig, 1);
        boundingRect = CGRectIntegral(boundingRectOrig);
        
        // Keep the fractional part of the idealised rect for use during rasterization
        _dd.x = boundingRectOrig.origin.x - boundingRect.origin.x;
        _dd.y = boundingRectOrig.origin.y - boundingRect.origin.y;
        
        _size.width = boundingRect.size.width;
        _size.height = boundingRect.size.height;
        _origin.x = boundingRect.origin.x;
        _origin.y = boundingRect.origin.y;
        

    }
};


Glyph* FontApple::createGlyph(char32_t ch) {
    return new GlyphApple(this, ch);
}

void FontApple::rasterizeGlyph(Glyph *glyph, Atlas *atlas) {
    
    // Reserve a space in the glyph atlas
    glyph->_atlasNode = atlas->reserve(glyph->_size.width, glyph->_size.height, 1);
    
    // Get the atlas bitmap context
    auto bitmap = glyph->_atlasNode->page->_bitmap.as<BitmapApple>();
    CGContext* c = bitmap->_context;
    
    CGPoint pt = CGPointMake(glyph->_atlasNode->rect.origin.x, glyph->_atlasNode->rect.origin.y);
    pt.x += ((GlyphApple*)glyph)->_dd.x; // apply fractional offset
    pt.y -= ((GlyphApple*)glyph)->_dd.y; // apply fractional offset
    pt.x -= glyph->_origin.x;
    pt.y += glyph->_origin.y;
    
    CGContextSaveGState(c);
    int h = (int)CGBitmapContextGetHeight(c);
    CGContextScaleCTM(c, 1, -1);
    CGContextTranslateCTM(c, 0, -h);
    pt.y = h-pt.y;
    pt.y -= glyph->_size.height;
    CGContextSetShouldSubpixelPositionFonts(c, false);
    CGContextSetShouldSubpixelQuantizeFonts(c, false);
    CTFontDrawGlyphs(_ctfont, &(((GlyphApple*)glyph)->_cgglyph), &pt, 1, c);
    CGContextRestoreGState(c);
    
    bitmap->texInvalidate();
}

Font* Font::create(const oak::string &fontAssetPath, float size, float weight) {
    return new FontApple(fontAssetPath, size, weight);
}

#endif

