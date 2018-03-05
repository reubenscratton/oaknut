//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_LINUX

#include "bitmap.h"

static FT_Library ft_library;
static cairo_t* cairo_for_measuring;

class LinuxFont : public Font {
public:

    hb_font_t* _hb_font;
    cairo_font_face_t* _cairo_font_face;

    LinuxFont(const string& fontAssetPath, float size) : Font(fontAssetPath, size) {
        size = dp(_size);
        //PangoFontMap* fm = pango_cairo_font_map_get_default();
        //PangoFontDescription* pfd = pango_font_description_new();
        //pango_font_description_set_family(pfd, "serif");
        //pango_font_description_set_size(pfd, (int)size);
        //_pangoContext = pango_context_new();
        //_pangoFont = pango_font_map_load_font (fm, _pangoContext, pfd);
        if (!ft_library) {
            FT_Init_FreeType(&ft_library);
            OSBitmap* bmp2 = new OSBitmap(1,1,BITMAPFORMAT_RGBA32); // lives forever
            cairo_for_measuring = bmp2->getCairo();
        }

        FT_Face face;
        FT_New_Face(ft_library, "/usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-R.ttf", 0, &face);
        FT_Set_Char_Size(face, 0, (int)size<<6,0,0);
        _hb_font = hb_ft_font_create(face, NULL);
        _cairo_font_face = cairo_ft_font_face_create_for_ft_face(face, 0);
    }
    
    Glyph* createGlyph(char32_t ch, Atlas* atlas) {
        oakLog("Creating glyph for char code %d in font %X", ch, this);
        //UniChar uch = ch;

        hb_codepoint_t glyphIndex = 0;
        hb_bool_t r = hb_font_get_glyph (_hb_font, (hb_codepoint_t)ch, 0, &glyphIndex);


        // Measure the glyph
        cairo_glyph_t cairo_glyph;
        cairo_glyph.index = glyphIndex;
        cairo_set_font_face(cairo_for_measuring, _cairo_font_face);
        cairo_set_font_size(cairo_for_measuring, dp(_size));
        cairo_text_extents_t cairo_text_extents;
        cairo_glyph_extents(cairo_for_measuring, &cairo_glyph, 1, &cairo_text_extents);

        // Reserve a space in the glyph atlas
        Glyph* glyph = new Glyph(this, ch, glyphIndex);
        glyph->advance = SIZE_Make(cairo_text_extents.x_advance, cairo_text_extents.y_advance);
        glyph->bitmapWidth = ceilf(cairo_text_extents.width);
        glyph->bitmapHeight = ceilf(cairo_text_extents.height);
        glyph->atlasNode = atlas->reserve(glyph->bitmapWidth, glyph->bitmapHeight, 1);
        glyph->bitmapLeft = floorf(cairo_text_extents.x_bearing);
        glyph->bitmapTop = glyph->bitmapHeight + floorf(cairo_text_extents.y_bearing);

        // Get the atlas bitmap context
        OSBitmap* bitmap = (OSBitmap*)glyph->atlasNode->page->_bitmap._obj;
        cairo_t* cr = bitmap->getCairo();
        cairo_glyph.x = glyph->atlasNode->rect.origin.x - glyph->bitmapLeft;
        cairo_glyph.y = glyph->atlasNode->rect.origin.y+glyph->atlasNode->rect.size.height-glyph->bitmapTop;
        cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
        cairo_set_font_face(cr, _cairo_font_face);
        cairo_set_font_size(cr, dp(_size));
        cairo_show_glyphs(cr, &cairo_glyph, 1);
        //cairo_surface_flush(bitmap->_cairo_surface);

        bitmap->_needsUpload = true;
        return glyph;
    }
};

Font* oakFontGet(const string& fontAssetPath, float size) {
    return new LinuxFont(fontAssetPath, size);
}


class LinuxPath : public Path {
public:

    void moveTo(POINT pt) {
        //CGPathMoveToPoint(path, nil, pt.x, pt.y);
    }
    void lineTo(POINT pt) {
        //CGPathAddLineToPoint(path, nil, pt.x, pt.y);
    }
    void curveTo(POINT ctrl1, POINT ctrl2, POINT pt) {
        //CGPathAddCurveToPoint(path, nil, ctrl1.x, ctrl1.y, ctrl2.x, ctrl2.y, pt.x, pt.y);
    }
};


class LinuxCanvas : public Canvas {
public:
    ObjPtr<OSBitmap> _bitmap;
    //CGSize _size;
    //CGColorRef _fillColor;
    //CGColorRef _strokeColor;
    //CGFloat _strokeWidth;
    //const AffineTransform* _transform;

    Bitmap* getBitmap() {
        return _bitmap;
    }


    void resize(int width, int height) {
    //    _size = size;
    //_bitmap = new OSBitmap(size.width, size.height, BITMAPFORMAT_RGBA32, NULL, 0);
    }

    void clear(COLOUR colour) {
        //CGContextSetBlendMode(_bitmap->_context, kCGBlendModeCopy);
        //CGContextRef context = _bitmap->_context;
        //CGContextSetFillColorWithColor(context, rgba(colour));
        //CGContextFillRect(context, CGRectMake(0,0,_size.width,_size.height));
        //CGContextSetFillColorWithColor(context, _fillColor);
        //CGContextSetLineCap(context, kCGLineCapRound);
        _bitmap->_needsUpload = true;
        //CGContextSetBlendMode(_bitmap->_context, kCGBlendModeNormal);
    }

    void setFillColour(COLOUR colour) {
        //_fillColor = rgba(colour);
        //CGContextSetFillColorWithColor(_bitmap->_context, _fillColor);
    }
    void setStrokeColour(COLOUR colour) {
        //_strokeColor = rgba(colour);
        //CGContextSetStrokeColorWithColor(_bitmap->_context, _strokeColor);
    }
    void setStrokeWidth(float strokeWidth) {
        //_strokeWidth = strokeWidth;
        //CGContextSetLineWidth(_bitmap->_context, strokeWidth);
    }
    void setAffineTransform(AffineTransform* t) {
        //_transform = t;
    }

    void drawRect(RECT rect) {
        /*CGRect cgrect = CGRectMake(rect.origin.x,rect.origin.y,rect.size.width,rect.size.height);
        if (_fillColor) {
            CGContextFillRect(_bitmap->_context, cgrect);
        }
        if (_strokeColor && _strokeWidth>0) {
            CGContextStrokeRect(_bitmap->_context, cgrect);
        }
        _bitmap->_needsUpload = true;*/
    }

    void drawOval(RECT rect) {
        /*CGRect cgrect = CGRectMake(rect.origin.x,rect.origin.y,rect.size.width,rect.size.height);
        if (_fillColor) {
            CGContextFillEllipseInRect(_bitmap->_context, cgrect);
        }
        if (_strokeColor && _strokeWidth>0) {
            CGContextStrokeEllipseInRect(_bitmap->_context, cgrect);
        }
        _bitmap->_needsUpload = true;*/
    }

    void drawPath(Path* ospath) {
        /*CGPathRef path = (CGPathRef)ospath;
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
        _bitmap->_needsUpload = true;*/
    }
    Path* createPath() {
        //CGMutablePathRef path = CGPathCreateMutable();
        // return path;
        return new LinuxPath();
    }

};




Canvas* Canvas::create() {
    return new LinuxCanvas();
}


#endif
