//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class Font : public Object {
public:
    string _name;
    float _size;
    map<char32_t, ObjPtr<class Glyph>> _glyphs; // map of character codes to Glyph*
    float _height;
    float _ascent;
    float _descent;
    float _leading;

protected:
    Font(const string& name, float size); // don't try to use this, use oakFontCreate...()
    ~Font();
    virtual Glyph* createGlyph(char32_t ch, Atlas* atlas) = 0;
    
public:
    Glyph* getGlyph(char32_t ch);
};

// Gets a font at a particular size. If asset path is zero-length then the system font is used. If size
// is zero then 12 is the default.
Font* oakFontGet(const string& fontAssetPath, float size);


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


