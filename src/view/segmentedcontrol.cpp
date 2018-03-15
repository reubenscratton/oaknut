//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"


DECLARE_DYNCREATE(SegmentedControl);


SegmentedControl::SegmentedControl() {
	_font = Styles::getFont("segmentedcontrol");
    _lineWidth = Styles::getFloat("segmentedcontrol.stroke-width");
	_selectedIndex = _pressedIndex = -1;
	_selectedTextColour = 0xff000000;
}

void SegmentedControl::onEffectiveTintColourChanged() {
    View::onEffectiveTintColourChanged();
    for (auto i : _segments) {
        i.rectOp->setStrokeColour(_effectiveTintColour);
        COLOUR actualTextColour = _textColour ? _textColour : _effectiveTintColour;
        i.label->setDefaultColour(actualTextColour);
    }
}

void SegmentedControl::addSegment(const string& label) {
    Segment segment;
    segment.label = new TextRenderer();
	segment.label->setDefaultFont(_font);
	segment.label->setText(label);
    segment.label->setGravity({GRAVITY_CENTER,GRAVITY_CENTER});
    COLOUR actualColour = _textColour ? _textColour : _effectiveTintColour;
    segment.label->setDefaultColour(actualColour);
    float r[2] = {0,0};
    if (_segments.size() == 0) {
        r[0] = app.dp(4);
    } else {
        r[1] = app.dp(4);
    }
    segment.rectOp = new RoundRectRenderOp(this, 0, _lineWidth, _effectiveTintColour, r);
    addRenderOp(segment.rectOp);
	_segments.push_back(segment);

	invalidateContentSize();
}

void SegmentedControl::setTextColour(COLOUR colour) {
    _textColour = colour;
    COLOUR actualTextColour = _textColour ? _textColour : _effectiveTintColour;
    for (int i=0 ; i<_segments.size() ; i++) {
        Segment& segment = _segments.at(i);
        segment.label->setDefaultColour(actualTextColour);
    }
}
void SegmentedControl::setSelectedTextColour(COLOUR colour) {
	_selectedTextColour = colour;
    if (_selectedIndex >= 0) {
        Segment& selectedSegment = _segments.at(_selectedIndex);
        selectedSegment.label->setDefaultColour(colour);
    }
}

void SegmentedControl::setSelectedSegment(int segmentIndex) {
	if (segmentIndex != _selectedIndex) {
        if (_selectedIndex >= 0) {
            Segment& selectedSegment = _segments.at(_selectedIndex);
            if (selectedSegment.label) {
                COLOUR actualTextColour = _textColour ? _textColour : _effectiveTintColour;
                selectedSegment.label->setDefaultColour(actualTextColour);
                selectedSegment.rectOp->setColour(0);
            }
            // todo: rect fill color
        }
		_selectedIndex = segmentIndex;
		onSegmentTap(segmentIndex);
        if (_selectedIndex >= 0) {
            Segment& selectedSegment = _segments.at(_selectedIndex);
            if (selectedSegment.label) {
                selectedSegment.label->setDefaultColour(_selectedTextColour);
                selectedSegment.rectOp->setColour(_effectiveTintColour);
            }
        }
	}
    invalidateContentSize(); // todo: This is lazy. Size isn't changing.
}

void SegmentedControl::setSegmentSelectedDelegate(SegmentSelectedDelegate delegate) {
	_segmentSelectedDelegate = delegate;
}

void SegmentedControl::updateContentSize(float parentWidth, float parentHeight) {
	_contentSize.width = 0;
	_contentSize.height = 0;
	// Calculate segment rects. NB: These are pixel-aligned so roundrects look as good as poss.
	for (int i=0 ; i<_segments.size() ; i++) {
        Segment& segment = _segments.at(i);
        segment.label->measure();
        SIZE labelSize = segment.label->measuredSize();
		segment.rect.size.width = app.dp(8) + labelSize.width + app.dp(8);
		segment.rect.size.height = app.dp(4) + labelSize.height + app.dp(4);
		_contentSize.width += segment.rect.size.width;
		_contentSize.height = max(_contentSize.height, segment.rect.size.height);
	}
	// Adjust cos rects overlap by a line width
	_contentSize.width -= (_segments.size()-1) * _lineWidth;
}

void SegmentedControl::measure(float parentWidth, float parentHeight) {
	View::measure(parentWidth, parentHeight);
	if (1==(((int)_frame.size.height)&1)) {
		_frame.size.height+=1;
	}
	
}

void SegmentedControl::layout() {
	View::layout();
	float x=0;
	for (int i=0 ; i<_segments.size() ; i++) {
		Segment& segment = _segments.at(i);
		RECT& rect = segment.rect;
		rect.origin.x = x;
        rect.origin.y = 0;
        segment.rectOp->setRect(rect);
		x += rect.size.width - _lineWidth;
	}
    _updateRenderOpsNeeded = true;
}

void SegmentedControl::updateRenderOps() {
    for (int i=0 ; i<_segments.size() ; i++) {
        Segment& segment = _segments.at(i);
        segment.label->layout(segment.rect);
        segment.label->updateRenderOps(this);
    }
}

void SegmentedControl::setPressedIndex(int pressedIndex) {
    if (_pressedIndex >= 0) {
        Segment& segment = _segments.at(_pressedIndex);
        segment.rectOp->setColour((_pressedIndex==_selectedIndex) ? _effectiveTintColour : 0);
        invalidateRect(segment.rect);
    }
    _pressedIndex = pressedIndex;
    if (pressedIndex >=0) {
        Segment& segment = _segments.at(pressedIndex);
        segment.rectOp->setColour(0xc0000000 | (_effectiveTintColour&0xffffff));
        invalidateRect(segment.rect);
    }
}

bool SegmentedControl::onTouchEvent(int eventType, int eventSource, POINT pt) {
	if (eventType == INPUT_EVENT_DOWN) {
		_pressedIndex = -1;
		for (int i=0 ; i<_segments.size() ; i++) {
			Segment& segment = _segments.at(i);
			if (segment.rect.contains(pt)) {
                setPressedIndex(i);
				break;
			}
		}
	} else if (eventType == INPUT_EVENT_UP) {
		if (_pressedIndex >= 0) {
            Segment& segment = _segments.at(_pressedIndex);
			if (segment.rect.contains(pt)) {
				onSegmentTap(_pressedIndex);
				if (_pressedIndex != _selectedIndex) {
					if (_selectedIndex >= 0) {
						invalidateRect(_segments.at(_selectedIndex).rect);
					}
                    setSelectedSegment(_pressedIndex);
				}
			}
			setPressedIndex(-1);
		}
	}
	return true;
}

void SegmentedControl::onSegmentTap(int segmentIndex) {
	if (_segmentSelectedDelegate) {
		_segmentSelectedDelegate(segmentIndex);
	}
}
