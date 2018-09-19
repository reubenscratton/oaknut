//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

// These values taken from Android & Windows definitions.

const float FONT_WEIGHT_ULTRA_LIGHT = 100;     // -0.800000011920929
const float FONT_WEIGHT_THIN        = 200;     // -0.6000000238418579
const float FONT_WEIGHT_LIGHT       = 300;     // -0.4000000059604645
const float FONT_WEIGHT_REGULAR     = 400;     // 0
const float FONT_WEIGHT_MEDIUM      = 500;     // 0.2300000041723251
const float FONT_WEIGHT_SEMIBOLD    = 600;     // 0.300000011920929
const float FONT_WEIGHT_BOLD        = 700;     // 0.4000000059604645
const float FONT_WEIGHT_HEAVY       = 800;     // 0.5600000023841858
const float FONT_WEIGHT_BLACK       = 900;     // 0.6200000047683716

class Font;
class FontBase : public Object {
public:
    string _name;
    float _size;
    float _weight;
    map<char32_t, ObjPtr<class Glyph>> _glyphs; // map of character codes to Glyph*
    float _height;
    float _ascent;
    float _descent;
    float _leading;

    ~FontBase();

protected:
    // Gets a font at a particular size. If asset path is zero-length then
    // system font is used. If size is zero then 12 is the default.
    FontBase(const string& name, float size, float weight);
    virtual Glyph* createGlyph(char32_t ch, Atlas* atlas) = 0;
    
public:
    Glyph* getGlyph(char32_t ch);
    
    static Font* get(const string& name, float size, float weight=FONT_WEIGHT_REGULAR);
};



class Glyph : public Object {
public:
    Font* _font;
    char32_t charCode;
    //uint16_t glyphIndex;
    int bitmapWidth;
    int bitmapHeight;
	int bitmapLeft;
	int bitmapTop;
    AtlasNode* atlasNode;
    SIZE advance;
    
    Glyph(Font* font, char32_t ch, uint16_t glyphIndex);
};


