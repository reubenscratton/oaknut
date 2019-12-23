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

static NSString* s_systemFontFamilyName;

static CTFontRef createCTFont(FontApple* font) {
    CGFloat size = font->_size;
    if (font->_name.length()) {

        // Treat the name as a font family and ask NSFontManager to get the best match
        /*NSString* nsname = [NSString stringWithUTF8String:font->_name.c_str()];
#if PLATFORM_MACOS
        NSFontTraitMask traits = 0;
        if (font->_italic) {
            traits |= NSFontItalicTrait;
        }
        NSInteger weight = font->_weight / 100;
        NSFont* nsfont = [[NSFontManager sharedFontManager] fontWithFamily:nsname traits:traits weight:weight size:font->_size];
        if (nsfont) {
            return (__bridge_retained CTFontRef)nsfont;
        }
#endif*/
        
        // Maybe its the name of an font asset?
        bytearray data;
        app->loadAsset(font->_name, data);
        if (data.size()) {
            CGDataProviderRef dataProvider = CGDataProviderCreateWithData(NULL, data.data(), data.size(), NULL);
            CGFontRef cgfont = CGFontCreateWithDataProvider(dataProvider);
            assert(cgfont);
            CGDataProviderRelease(dataProvider);
            CTFontRef ctfont = CTFontCreateWithGraphicsFont(cgfont, size, NULL, NULL);
            CGFontRelease(cgfont);
            return ctfont;
        }
    }
    
    // Default to the system font if no name given. This is usually ".AppleSystemUIFont" these days.
    NSString* fontName = nil;
    if (!font->_name.lengthInBytes()) {
        if (!s_systemFontFamilyName) {
            auto sysfont = [NSUI(Font) systemFontOfSize:0 weight:0];
            s_systemFontFamilyName = sysfont.fontName;
        }
        fontName = s_systemFontFamilyName;
    } else {
        fontName = [NSString stringWithUTF8String:font->_name.c_str()];
    }
    
    // Get font via Core Text
    NSDictionary* attrs = @{
        (NSString*)kCTFontFamilyNameAttribute: fontName,
        (NSString*)kCTFontSizeAttribute: @(font->_size),
        (NSString*)kCTFontTraitsAttribute: @{
                (NSString*)kCTFontSymbolicTrait: @(font->_italic ? kCTFontTraitItalic : 0),
                (NSString*)kCTFontWeightTrait: @(UIFontWeightFromStandardWeight(font->_weight))
            }
        };
    CTFontDescriptorRef descriptor = CTFontDescriptorCreateWithAttributes((CFDictionaryRef)attrs);
    CTFontRef ctfont = CTFontCreateWithFontDescriptor(descriptor, 0, NULL);
    CFRelease(descriptor);
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
        CGRect rect;
        CTFontGetBoundingRectsForGlyphs(font->_ctfont, kCTFontOrientationDefault, &_cgglyph, &rect, 1);
#define MAGIC_X 0.297 //0.157

        // If origin.y has a -ve fractional part then floor the rect to include it
        float dy = rect.origin.y - floor(rect.origin.y);
        if (dy>0) {
            rect.origin.y = floor(rect.origin.y);
            rect.size.height += dy;
        }
        rect.size.height += 1.0f;
        
        
        float dx = rect.origin.x - floor(rect.origin.x);
        if (dx<=MAGIC_X) { // Determined by experimentation and probably wrong <0.1% of the time.
            rect.origin.x -= 0.5;
            dx += 1;
        }
        rect.origin.x = floor(rect.origin.x);
        rect.size.width += dx;
        rect.size.width += 1.0f;
        rect.size.width = round(rect.size.width);
        rect.size.height = round(rect.size.height);
        
        _size = {(int)rect.size.width, (int)rect.size.height};

        _origin = {static_cast<float>(rect.origin.x), static_cast<float>(rect.origin.y)};

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
    
    //CGAffineTransform t = CGContextGetUserSpaceToDeviceSpaceTransform(c);

    // Core text will draw the glyph at ITS origin. To precisely position the glyph
    // into its atlas node we must subtract the glyph's origin from the atlas node origin.
    CGPoint pt = CGPointMake(glyph->_atlasNode->rect.origin.x, glyph->_atlasNode->rect.origin.y);
    //pt.x += 1-((GlyphApple*)glyph)->_dd.x; // apply fractional offset
    //pt.y -= ((GlyphApple*)glyph)->_dd.y; // apply fractional offset
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

