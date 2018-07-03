//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class Font;
class FontBase : public Object {
public:
    string _name;
    float _size;
    map<char32_t, ObjPtr<class Glyph>> _glyphs; // map of character codes to Glyph*
    float _height;
    float _ascent;
    float _descent;
    float _leading;

    ~FontBase();

protected:
    // Gets a font at a particular size. If asset path is zero-length then
    // system font is used. If size is zero then 12 is the default.
    FontBase(const string& name, float size);
    virtual Glyph* createGlyph(char32_t ch, Atlas* atlas) = 0;
    
public:
    Glyph* getGlyph(char32_t ch);
    
    static Font* get(const string& name, float size);
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


