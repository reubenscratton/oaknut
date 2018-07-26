//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class Span : public Object {
public:
    int _start;
    int _end;
    
    ObjPtr<Font> _font;
    COLOUR _forecolour;
};

class TextRenderer : public Object {
    
public:
    TextRenderer();
    void measure();
    void measure(SIZE maxSize);
    void layout(RECT rect);
    const SIZE measuredSize() { return _measuredSize; }
    void updateRenderOps(View* view);
    void setText(const string& text);
    const string& getText() { return _text; }
    void setDefaultColour(COLOUR colour);
    COLOUR getDefaultColour() { return _defaultColour; }
    void setDefaultFontName(const string& fontName);
    void setDefaultFontSize(float fontSize);
    void setDefaultFont(Font* font);
    const Font* getDefaultFont() { return _defaultFont; }
    void setGravity(GRAVITY gravity);
    void setMaxLines(int maxLines);
    void getGlyphOrigin(int glyphIndex, POINT* origin, float* ascent, float* descent);
    
protected:
    typedef struct {
        Glyph* glyph; // Glyphs are owned by their font, no need for ownership here
        RECT rect;    // client coords
        TEXTRENDERPARAMS* renderParams;
    } GLYPHINFO;
    
    typedef struct {
        Font* font; // font at start of line only
        RECT bounds; // in client coords
        float baseline; // offset from bounds.top to baseline
        vector<GLYPHINFO> glyphinfos;
        bool ellipsis;
    } TEXTLINE;

    friend class Label;
    
protected:
    string _text;
    vector<ObjPtr<Span>> _spans;
    ObjPtr<Font> _defaultFont;

    COLOUR _defaultColour;
    GRAVITY _gravity;
    bool _measuredSizeValid;
    SIZE _measuredSize;
    set<TEXTRENDERPARAMS> _renderParams;
    list<TextRenderOp*> _textOps;
    bool _renderOpsValid;
    vector<TEXTLINE> _lines;
    int _maxLines; // 0=as many as needed, >=1 = text will ellipsize
    RECT _layoutRect;
    
    TEXTLINE* getLineForGlyphIndex(int& glyphIndex);

};
