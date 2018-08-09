//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

static ObjPtr<Atlas> atlas;
static bool s_inited;
static map<string, ObjPtr<Font>> s_loadedFonts;


FontBase::FontBase(const string& name, float size) {
    _name = name;
	_size = size;
}

FontBase::~FontBase() {
    // TODO: remove from s_loadedFonts
}




Font* FontBase::get(const string& name, float size) {
    string fkey = string::format("%f-%s", size, name.data());
    auto it = s_loadedFonts.find(fkey);
    if (it != s_loadedFonts.end()) {
        return it->second;
    }
    Font* font = new Font(name, size);
    s_loadedFonts[fkey] = font;
    return font;
}


Glyph* FontBase::getGlyph(char32_t ch) {
    if (!s_inited) {
        s_inited = true;
#ifdef PLATFORM_LINUX
        atlas = new Atlas(512, 512, BITMAPFORMAT_RGBA32); //  TODO: why won't A8 work on Linux? Test Cairo
#else
        atlas = new Atlas(512, 512, BITMAPFORMAT_A8); // 64KB per page
#endif
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
