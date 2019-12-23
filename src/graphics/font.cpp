//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

sp<Atlas> Font::atlas;
static bool s_inited;
static map<string, sp<Font>> s_loadedFonts;


Font::Font(const string& name, float size, float weight) {
    _name = name;
	_size = size;
    _weight = weight;
}

Font::~Font() {
    // TODO: remove from s_loadedFonts
}




Font* Font::get(const string& name, float size, float weight/*=FONT_WEIGHT_REGULAR*/) {
    size = floorf(size);
    string fkey = string::format("%f-%f", size, weight);
    if (name.length() > 0) {
        fkey.insert(0, name);
    }
    auto it = s_loadedFonts.find(fkey);
    if (it != s_loadedFonts.end()) {
        return it->second;
    }
    Font* font = Font::create(name, size, weight);
    s_loadedFonts[fkey] = font;
    return font;
}


Glyph* Font::getGlyph(char32_t ch) {
    if (!s_inited) {
        s_inited = true;
#ifdef PLATFORM_LINUX
        atlas = new Atlas(512, 512, PIXELFORMAT_RGBA32); //  TODO: why won't A8 work on Linux? Test Cairo
#else
        atlas = new Atlas(512, 512, PIXELFORMAT_A8); // 64KB per page
#endif
    }
    auto it = _glyphs.find(ch);
    if (it != _glyphs.end()) {
        return it->second;
    }
    Glyph* glyph = createGlyph(ch);
    assert(glyph);
    _glyphs.insert(std::make_pair(ch, glyph));
    return glyph;
}

Glyph::Glyph(Font* font, char32_t codepoint) : _font(font), _codepoint(codepoint) {
}

AtlasNode* Glyph::atlasNode() {
    if (!_atlasNode) {
        _font->rasterizeGlyph(this, Font::atlas);
    }
    return _atlasNode;
}
