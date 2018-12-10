//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_LINUX

#include <oaknut.h>

static cairo_t* cairo_for_measuring;


FontLinux::FontLinux(const string& fontAssetPath, float size, float weight) : Font(fontAssetPath, size, weight) {
    PangoFontDescription* desc = pango_font_description_new();
    pango_font_description_set_family(desc, "sans-serif");
    pango_font_description_set_size(desc, (int)size);
    pango_font_description_set_weight(desc, (PangoWeight)(int)weight);

    auto context = gdk_pango_context_get_for_screen(gdk_screen_get_default ());

    PangoFontMap* fontMap = pango_cairo_font_map_get_default();
    PangoFont* pango_font = pango_font_map_load_font(fontMap, context, desc);
    _scaled_font = pango_cairo_font_get_scaled_font ((PangoCairoFont *)pango_font);

    if (!cairo_for_measuring) {
        BitmapLinux* bmp2 = new BitmapLinux(1,1,BITMAPFORMAT_RGBA32); // lives forever
        cairo_for_measuring = bmp2->getCairo();
    }

    /*PangoFontFamily **families;
    int fontCount = 0;
    pango_context_list_families(context, &families, &fontCount);

    printf("%d fonts found\n", fontCount);
    for(int i=0; i<fontCount; i++)
    {
        printf("[%s]\n", pango_font_family_get_name (families[i]));
    }
    */


}
    
Glyph* FontLinux::createGlyph(char32_t ch, Atlas* atlas) {

    cairo_set_scaled_font (cairo_for_measuring, _scaled_font);

    cairo_glyph_t cairo_glyph[1];
    cairo_glyph_t* pp = &cairo_glyph[0];
    int num_glyphs = 1;
    cairo_scaled_font_text_to_glyphs(_scaled_font, 0,0, (const char*)&ch, 1, &pp, &num_glyphs, NULL, NULL, NULL);


    // Measure the glyph
    cairo_set_font_size(cairo_for_measuring, _size);
    cairo_text_extents_t cairo_text_extents;
    cairo_glyph_extents(cairo_for_measuring, cairo_glyph, 1, &cairo_text_extents);


    // Reserve a space in the glyph atlas
    Glyph* glyph = new Glyph(this, ch, cairo_glyph->index);
    glyph->advance = {(float)cairo_text_extents.x_advance, (float)cairo_text_extents.y_advance};
    glyph->bitmapWidth = ceilf(cairo_text_extents.width);
    glyph->bitmapHeight = ceilf(cairo_text_extents.height);
    glyph->atlasNode = atlas->reserve(glyph->bitmapWidth, glyph->bitmapHeight, 1);
    glyph->bitmapLeft = floorf(cairo_text_extents.x_bearing);
    glyph->bitmapTop = -(glyph->bitmapHeight + floorf(cairo_text_extents.y_bearing));
    
    // Get the atlas bitmap context
    BitmapLinux* bitmap = (BitmapLinux*)glyph->atlasNode->page->_bitmap._obj;
    cairo_t* cr = bitmap->getCairo();
    cairo_glyph->x = glyph->atlasNode->rect.origin.x - glyph->bitmapLeft;
    cairo_glyph->y = glyph->atlasNode->rect.origin.y
            + glyph->atlasNode->rect.size.height
            + glyph->bitmapTop;
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);

    cairo_set_scaled_font (cr, _scaled_font);
    cairo_set_font_size(cr, _size);
    cairo_show_glyphs(cr, cairo_glyph, 1);
    //cairo_surface_flush(bitmap->_cairo_surface);
    
    bitmap->_needsUpload = true;
    return glyph;
}

Font* Font::create(const oak::string &fontAssetPath, float size, float weight) {
    return new FontLinux(fontAssetPath, size, weight);
}

#endif
