//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

typedef std::function<void(int)> SegmentSelectedDelegate;


class SegmentedControl : public View {
public:
	
	SegmentedControl();
    virtual void setFontName(const string& fontName);
    virtual void setFontSize(float fontSize);
	virtual void addSegment(const string& label);
    virtual void setTextColor(COLOR color);
	virtual void setSelectedTextColor(COLOR color);
	virtual void setSegmentSelectedDelegate(SegmentSelectedDelegate delegate);
	virtual void onSegmentTap(int segmentIndex);
	virtual void setSelectedSegment(int segmentIndex);
    virtual int getSelectedIndex() const { return _selectedIndex; }
    virtual void setPressedIndex(int pressedIndex);
    
	// Overrides
    bool applyStyleValue(const string &name, const StyleValue *value) override;
	void updateContentSize(SIZE constrainingSize) override;
	void measure(float parentWidth, float parentHeight) override;
	void layout() override;
    void updateRenderOps() override;
	bool handleInputEvent(INPUTEVENT* event) override;
    void onEffectiveTintColorChanged() override;

protected:
    sp<Font> _font;
    string _fontName;
    float _fontSize;
    bool _fontValid;
    typedef struct {
        RECT rect;
        sp<RectRenderOp> rectOp;
        sp<TextRenderer> label;
    } Segment;
    vector<Segment> _segments;
    COLOR _textColor;
    COLOR _selectedTextColor;
    float _lineWidth;
    float _cornerRadius;
    int _pressedIndex;
    int _selectedIndex;
    SegmentSelectedDelegate _segmentSelectedDelegate;

    void updateBorders();
};
