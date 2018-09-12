//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

// These values taken from iOS UIFontWeight definitions. They may well change.
#define FONT_WEIGHT_ULTRA_LIGHT -0.800000011920929
#define FONT_WEIGHT_THIN -0.6000000238418579
#define FONT_WEIGHT_LIGHT -0.4000000059604645
#define FONT_WEIGHT_REGULAR 0
#define FONT_WEIGHT_MEDIUM 0.2300000041723251
#define FONT_WEIGHT_SEMIBOLD 0.300000011920929
#define FONT_WEIGHT_BOLD 0.4000000059604645
#define FONT_WEIGHT_HEAVY 0.5600000023841858
#define FONT_WEIGHT_BLACK 0.6200000047683716

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


