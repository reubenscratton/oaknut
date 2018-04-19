//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class Font : public FontBase {
public:
    
    hb_font_t* _hb_font;
    cairo_font_face_t* _cairo_font_face;
    
    Font(const string& fontAssetPath, float size);
    Glyph* createGlyph(char32_t ch, Atlas* atlas);
};

