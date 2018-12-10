//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class FontLinux : public Font {
public:
    
    cairo_scaled_font_t* _scaled_font;
    
    FontLinux(const string& fontAssetPath, float size, float weight);
    Glyph* createGlyph(char32_t ch, Atlas* atlas);
};

