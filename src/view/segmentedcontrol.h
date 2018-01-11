//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

typedef std::function<void(int)> SegmentSelectedDelegate;


class SegmentedControl : public View {
public:
	Font* _font;
    typedef struct {
        RECT rect;
        ObjPtr<RoundRectRenderOp> rectOp;
        ObjPtr<TextRenderer> label;
    } Segment;
	vector<Segment> _segments;
    COLOUR _textColour;
	COLOUR _selectedTextColour;
	float _lineWidth;
	int _pressedIndex;
	int _selectedIndex;
	SegmentSelectedDelegate _segmentSelectedDelegate;
	
	SegmentedControl();
	virtual void addSegment(const string& label);
    virtual void setTextColour(COLOUR colour);
	virtual void setSelectedTextColour(COLOUR colour);
	virtual void setSegmentSelectedDelegate(SegmentSelectedDelegate delegate);
	virtual void onSegmentTap(int segmentIndex);
	virtual void setSelectedSegment(int segmentIndex);
    virtual void setPressedIndex(int pressedIndex);
    
	// Overrides
	virtual void updateContentSize(float parentWidth, float parentHeight);
	virtual void measure(float parentWidth, float parentHeight);
	virtual void layout();
    virtual void updateRenderOps();
	virtual bool onTouchEvent(int eventType, int eventSource, POINT pt);
    virtual void onEffectiveTintColourChanged();
};
