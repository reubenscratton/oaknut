//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#include "bitmap.h"
#if TARGET_OS_IOS
#include <UIKit/UIKit.h>
#include <CoreText/CoreText.h>
#else
#include <AppKit/AppKit.h>
#endif

class AppleFont : public Font {
public:
    CTFontRef _ctfont;
    
    CTFontRef createCTFont() {
        CTFontRef ctfont = NULL;
        CGFloat size = dp(_size);
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
            ObjPtr<Data> data = oakLoadAsset(_name.data());
            CGDataProviderRef dataProvider = CGDataProviderCreateWithData(NULL, data->data, data->cb, NULL);
            CGFontRef cgfont = CGFontCreateWithDataProvider(dataProvider);
            assert(cgfont);
            CGDataProviderRelease(dataProvider);
            ctfont = CTFontCreateWithGraphicsFont(cgfont, size, NULL, NULL);
            CGFontRelease(cgfont);
        }
        return ctfont;
    }
    
    AppleFont(const string& fontAssetPath, float size) : Font(fontAssetPath, size) {
        _ctfont = createCTFont();
        _ascent = CTFontGetAscent(_ctfont);
        _descent = -CTFontGetDescent(_ctfont); // as per Freetype convention, this is -ve.
        _leading = CTFontGetLeading(_ctfont);
        if (_height<=0) {
            _height = (_ascent-_descent) + _leading;
        }
    }
    
    Glyph* createGlyph(char32_t ch, Atlas* atlas) {
        //oakLog("Creating glyph for char code %d in font %X", ch, this);
        UniChar uch = ch;
        CGGlyph cgglyph = 0;
        if (!CTFontGetGlyphsForCharacters(_ctfont, &uch, &cgglyph, 1)) {
            // Sometimes CTFontGetGlyphsForCharacters() fails for no clear reason and the only
            // workaround I can find is to recreate the CTFont and try again. Bloody Apple.
            CFRelease(_ctfont);
            _ctfont = createCTFont();
            if (!CTFontGetGlyphsForCharacters(_ctfont, &uch, &cgglyph, 1)) {
                assert(false); // glyph is not in the font, TODO: fall back to another font
                return NULL;
            }
            oakLog("Warning: a stupid Core Text bug caused us to waste time recreating a font");
        }
        Glyph* glyph = new Glyph(this, ch, cgglyph);
        
        // Get glyph advances
        CGSize advances;
        CTFontGetAdvancesForGlyphs(_ctfont, kCTFontOrientationDefault, &cgglyph, &advances, 1);
        glyph->advance = SIZE_Make(advances.width, advances.height);
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
        OSBitmap* bitmap = (OSBitmap*)glyph->atlasNode->page->_bitmap._obj;
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
};

Font* oakFontGet(const string& fontAssetPath, float size) {
    return new AppleFont(fontAssetPath, size);
}



@interface AppleCanvas : NSObject {
    @public
    ObjPtr<OSBitmap> _bitmap;
    CGSize _size;
    CGColorRef _fillColor;
    CGColorRef _strokeColor;
    CGFloat _strokeWidth;
    const AffineTransform* _transform;
}

@end

static CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();

static CGColorRef rgba(unsigned int rgba) {
    CGFloat components[4];
    components[0] = ((rgba&0xff0000)>>16)/255.0;
    components[1] = ((rgba&0xff00)>>8)/255.0;
    components[2] = (rgba&0xff)/255.0;
    components[3] = ((rgba&0xff000000)>>24)/255.0;
    return CGColorCreate(rgb, components);
}

@implementation AppleCanvas

- (void)resize:(CGSize)size {
    _size = size;
    _bitmap = new OSBitmap(size.width, size.height, BITMAPFORMAT_RGBA32, NULL, 0);
}

- (void)clear:(COLOUR)colour {
    CGContextSetBlendMode(_bitmap->_context, kCGBlendModeCopy);
    CGContextRef context = _bitmap->_context;
    CGContextSetFillColorWithColor(context, rgba(colour));
    CGContextFillRect(context, CGRectMake(0,0,_size.width,_size.height));
    CGContextSetFillColorWithColor(context, _fillColor);
    CGContextSetLineCap(context, kCGLineCapRound);
    _bitmap->_needsUpload = true;
    CGContextSetBlendMode(_bitmap->_context, kCGBlendModeNormal);
}

- (void)setFillColour:(COLOUR)colour {
    _fillColor = rgba(colour);
    CGContextSetFillColorWithColor(_bitmap->_context, _fillColor);
}
- (void)setStrokeColour:(COLOUR)colour {
    _strokeColor = rgba(colour);
    CGContextSetStrokeColorWithColor(_bitmap->_context, _strokeColor);
}
- (void)setStrokeWidth:(CGFloat)strokeWidth {
    _strokeWidth = strokeWidth;
    CGContextSetLineWidth(_bitmap->_context, strokeWidth);
}
- (void)setAffineTransform:(AffineTransform*)t {
    _transform = t;
}

- (void)drawRect:(RECT)rect {
    CGRect cgrect = CGRectMake(rect.origin.x,rect.origin.y,rect.size.width,rect.size.height);
    if (_fillColor) {
        CGContextFillRect(_bitmap->_context, cgrect);
    }
    if (_strokeColor && _strokeWidth>0) {
        CGContextStrokeRect(_bitmap->_context, cgrect);
    }
    _bitmap->_needsUpload = true;
}

- (void)drawOval:(RECT)rect {
    CGRect cgrect = CGRectMake(rect.origin.x,rect.origin.y,rect.size.width,rect.size.height);
    if (_fillColor) {
        CGContextFillEllipseInRect(_bitmap->_context, cgrect);
    }
    if (_strokeColor && _strokeWidth>0) {
        CGContextStrokeEllipseInRect(_bitmap->_context, cgrect);
    }
    _bitmap->_needsUpload = true;
}

- (void)drawPath:(void*)ospath {
    CGPathRef path = (CGPathRef)ospath;
    if (_transform) {
        CGAffineTransform cgtransform;
        cgtransform.a = _transform->a;
        cgtransform.b = _transform->b;
        cgtransform.c = _transform->c;
        cgtransform.d = _transform->d;
        cgtransform.tx = _transform->tx;
        cgtransform.ty = _transform->ty;
        path = CGPathCreateCopyByTransformingPath(path, &cgtransform);
    }
    CGContextBeginPath(_bitmap->_context);
    CGContextAddPath(_bitmap->_context, path);
    CGContextClosePath(_bitmap->_context);
    if (_fillColor) {
        CGContextFillPath(_bitmap->_context);
    }
    if (_strokeColor && _strokeWidth>0) {
        CGContextStrokePath(_bitmap->_context);
    }
    _bitmap->_needsUpload = true;
}

@end


// API
void* oakCanvasCreate() {
    AppleCanvas* canvas = [AppleCanvas new];
    return (__bridge_retained void*)(canvas);
}
Bitmap* oakCanvasGetBitmap(void* oscanvas) {
    AppleCanvas* canvas = (__bridge AppleCanvas*)oscanvas;
    return canvas->_bitmap;
}
void oakCanvasResize(void* oscanvas, int width, int height) {
    AppleCanvas* canvas = (__bridge AppleCanvas*)oscanvas;
    [canvas resize:CGSizeMake(width, height)];
}
void oakCanvasClear(void* canvas, COLOUR colour) {
    AppleCanvas* c = (__bridge AppleCanvas*)canvas;
    [c clear:colour];
}
void oakCanvasSetFillColour(void* canvas, COLOUR colour) {
    AppleCanvas* c = (__bridge AppleCanvas*)canvas;
    [c setFillColour:colour];
}
void oakCanvasSetStrokeColour(void* canvas, COLOUR colour) {
    AppleCanvas* c = (__bridge AppleCanvas*)canvas;
    [c setStrokeColour:colour];
}
void oakCanvasSetStrokeWidth(void* canvas, float strokeWidth) {
    AppleCanvas* c = (__bridge AppleCanvas*)canvas;
    [c setStrokeWidth:strokeWidth];
}
void oakCanvasSetAffineTransform(void* canvas, AffineTransform* t) {
    AppleCanvas* c = (__bridge AppleCanvas*)canvas;
    [c setAffineTransform:t];
}
void oakCanvasDrawRect(void* canvas, RECT rect) {
    AppleCanvas* c = (__bridge AppleCanvas*)canvas;
    [c drawRect:rect];
}
void oakCanvasDrawOval(void* canvas, RECT rect) {
    AppleCanvas* c = (__bridge AppleCanvas*)canvas;
    [c drawOval:rect];
}
void oakCanvasDrawPath(void* canvas, void* ospath) {
    AppleCanvas* c = (__bridge AppleCanvas*)canvas;
    [c drawPath:ospath];
}

void* oakCanvasPathCreate() {
    CGMutablePathRef path = CGPathCreateMutable();
    return path;
}
void oakCanvasPathMoveTo(void* ospath, POINT pt) {
    CGMutablePathRef path = (CGMutablePathRef)ospath;
    CGPathMoveToPoint(path, nil, pt.x, pt.y);
}
void oakCanvasPathLineTo(void* ospath, POINT pt) {
    CGMutablePathRef path = (CGMutablePathRef)ospath;
    CGPathAddLineToPoint(path, nil, pt.x, pt.y);
}
void oakCanvasPathCurveTo(void* ospath, POINT ctrl1, POINT ctrl2, POINT pt) {
    CGMutablePathRef path = (CGMutablePathRef)ospath;
    CGPathAddCurveToPoint(path, nil, ctrl1.x, ctrl1.y, ctrl2.x, ctrl2.y, pt.x, pt.y);
}
void oakCanvasPathRelease(void* ospath) {
    CGMutablePathRef path = (CGMutablePathRef)ospath;
    CGPathRelease(path);
}

#endif
