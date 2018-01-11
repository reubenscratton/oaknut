//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"



Font::Font(const string& name, float size) {
    _name = name;
	_size = size;
}

Font::~Font() {
}



static ObjPtr<Atlas> atlas;

static bool s_inited;


Glyph* Font::getGlyph(char32_t ch) {
    if (!s_inited) {
        s_inited = true;
        atlas = new Atlas(512, 512, BITMAPFORMAT_A8); // 64KB per page
    }
    auto it = _glyphs.find(ch);
    if (it != _glyphs.end()) {
        return it->second;
    }
    Glyph* glyph = createGlyph(ch, atlas);
    assert(glyph);
    _glyphs.insert(std::make_pair(ch, glyph));
    return glyph;
}

Glyph::Glyph(Font* font, char32_t ch, uint16_t glyphIndex) : _font(font) {
    this->charCode = ch;
    //this->glyphIndex = glyphIndex;
}
