//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


/**
 Generic text layout component. Converts a string of codepoints (i.e. a string)
 into a same-size array of RENDER_GLYPHs. Can be used for measuring and laying
 out text outside of a graphical context.
 
 Inputs:
  - String (+attributes)
  - Bounding rect
  - Font (name, size, weight)
  - Forecolor (not used for measuring)
  - Line spacing
  - Gravity
  - Text direction
 
 Outputs:
  - Overall bounding box within the bounding rect
  - Array of glyphs, same size as string length.
  - Array of lines

A layout can also maintain a list of RenderOps for rendering. Views that render text
may use the updateRenderOps() method.
 
 At present it is embarrassingly simple in the following ways:
 
   - Left-to-right latin script only. Apologies to the rest of world, this will be addressed soon.
   - Soft linebreaks are based on whitespace only. The proper logic for this is horrific
     (see http://www.unicode.org/reports/tr14/) and will be implemented some day, maybe by integrating Pango.
   - No composite glyphs. The platform layer renders whole characters, not individual glyphs.
   - No stylistic ligatures. The platform layer does not yet provide a way to get inside fonts to find out what ligatures are available. Also am not sure how to render them on any platform... there must be APIs that returns a glyph index given TWO characters ('ff', 'fi', etc) but damned if I know what they are.
   - No kerning. Again, the platform layer does not yet provide a way to get at this font information.
 */


class TextLayout : public Object {
    
public:
    TextLayout();

    // Text
    void setText(const attributed_string& text);
    const attributed_string& getText() { return _text; }
    
    // Properties that affect text measurement, layout, and rendering
    void setFont(Font* font);
    const Font* getFont() const { return _defaultParams.font; }
    void setColor(COLOR color);
    COLOR color() const { return _defaultParams.forecolor; }
    void setGravity(GRAVITY gravity);
    void setMaxLines(int maxLines);
    int maxLines() const { return _maxLines; }
    bool ellipsize() const {return _ellipsize; }
    void setEllipsize(bool ellipsize);
    // float lineHeight() const { return _defaultParams.lineHeight; }
    void setLineHeight(float mul, float abs);
    void setParagraphMetrics(const PARAGRAPH_METRICS& metrics);
    
    // Measurement
    SIZE measure(SIZE& constrainingSize);
    int lineCount() const { return (int)_renderLines.size(); };

    // Layout
    void layout(RECT& containingRect);
    
    // Get text rect
    RECT rect();
    // TODO: add API for getting glyph and range rects
    
    // APIs for mapping between geometric points and text offsets
    void charOriginFromIndex(int32_t charIndex, POINT* origin, float* ascent, float* descent) const;
    int32_t charIndexFromPoint(const POINT& pt) const;
    int32_t lineIndexFromCharIndex(int32_t charIndex, int dLine) const;
    int32_t charIndexMove(int32_t charIndex, int dx, int dy) const;
    RECT lineRect(int32_t lineIndex);

    // Generates or updates a set of RenderOps belonging to the given View that will render the text
    void updateRenderOpsForView(View* view);
    void resetRenderOps();
    void invalidateOps(); // used by multiline Label when scrolled
    
    bool glyphsNeedsPositioning();
    bool opsValid();
    
protected:
    
    // A single font glyph, at some position
    struct RENDER_GLYPH {
        Glyph* glyph;
        COLOR forecolor;  // TODO: this is inefficient, and not even used if text color is in a uniform
        POINT topLeft;
        int32_t cluster; // 0 for primary glyph in character, 1 for secondary, etc.
        RECT rect() const;
    };
    
    
    // A sequence of consecutive glyphs that are all on the same baseline
    struct RENDER_LINE {
        int32_t start;   // offset into _renderGlyphs
        int32_t count;   // not including any terminating break character
        RECT rect;
        float baseline;  // distance from layoutRect.top to baseline
        Font* tallestFont;
        float leftIndent;
    };
    

    vector<RENDER_GLYPH> _renderGlyphs;
    vector<RENDER_LINE> _renderLines;
    vector<sp<RenderOp>> _renderOps;
    
    uint32_t _invalid;

    attributed_string _text;
    struct RENDER_PARAMS {
        Font* font;
        COLOR forecolor;
        float lineHeightMul;
        float lineHeightAbs;
        PARAGRAPH_METRICS paragraphMetrics;
        bool underline;
    } _defaultParams;
    GRAVITY _gravity;
    int _maxLines; // 0=as many as needed, >=1 implies _ellipsize=true
    bool _ellipsize; // Text will ellipsize rather than overflow bounds and become scrollable
    SIZE _constrainingSize;
    RECT _containingRect;
    RECT _rect;

    friend class Label;
};
