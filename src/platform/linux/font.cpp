//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_LINUX

#include <oaknut.h>

static FT_Library ft_library;
static cairo_t* cairo_for_measuring;


Font::Font(const string& fontAssetPath, float size) : FontBase(fontAssetPath, size) {
    size = dp(_size);
    //PangoFontMap* fm = pango_cairo_font_map_get_default();
    //PangoFontDescription* pfd = pango_font_description_new();
    //pango_font_description_set_family(pfd, "serif");
    //pango_font_description_set_size(pfd, (int)size);
    //_pangoContext = pango_context_new();
    //_pangoFont = pango_font_map_load_font (fm, _pangoContext, pfd);
    if (!ft_library) {
        FT_Init_FreeType(&ft_library);
        Bitmap* bmp2 = new Bitmap(1,1,BITMAPFORMAT_RGBA32); // lives forever
        cairo_for_measuring = bmp2->getCairo();
    }
    
    FT_Face face;
    FT_New_Face(ft_library, "/usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-R.ttf", 0, &face);
    FT_Set_Char_Size(face, 0, (int)size<<6,0,0);
    _hb_font = hb_ft_font_create(face, NULL);
    _cairo_font_face = cairo_ft_font_face_create_for_ft_face(face, 0);
}
    
Glyph* Font::createGlyph(char32_t ch, Atlas* atlas) {
    app.log("Creating glyph for char code %d in font %X", ch, this);
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
    Bitmap* bitmap = (Bitmap*)glyph->atlasNode->page->_bitmap._obj;
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

#endif
