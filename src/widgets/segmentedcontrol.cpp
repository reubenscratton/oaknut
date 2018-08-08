//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


DECLARE_DYNCREATE(SegmentedControl);


SegmentedControl::SegmentedControl() {
	_font = app.getStyleFont("segmentedcontrol");
    _lineWidth = app.getStyleFloat("segmentedcontrol.stroke-width");
	_selectedIndex = _pressedIndex = -1;
	_selectedTextColor = 0xff000000;
}

void SegmentedControl::onEffectiveTintColorChanged() {
    View::onEffectiveTintColorChanged();
    for (auto i : _segments) {
        i.rectOp->setStrokeColor(_effectiveTintColor);
        COLOR actualTextColor = _textColor ? _textColor : _effectiveTintColor;
        i.label->setDefaultColor(actualTextColor);
    }
}

void SegmentedControl::addSegment(const string& label) {
    Segment segment;
    segment.label = new TextRenderer();
	segment.label->setDefaultFont(_font);
	segment.label->setText(label);
    segment.label->setGravity({GRAVITY_CENTER,GRAVITY_CENTER});
    COLOR actualColor = _textColor ? _textColor : _effectiveTintColor;
    segment.label->setDefaultColor(actualColor);
    float r = app.dp(4); // todo: style!
    segment.rectOp = new RectRenderOp(this);
    segment.rectOp->setFillColor(0);
    segment.rectOp->setStrokeWidth(_lineWidth);
    segment.rectOp->setStrokeColor(_effectiveTintColor);
    if (_segments.size() == 0) {
        segment.rectOp->setCornerRadius(4);
    } else {
        segment.rectOp->setCornerRadii({r,r,0,0});
    }
    addRenderOp(segment.rectOp);
	_segments.push_back(segment);

	invalidateContentSize();
}

void SegmentedControl::setTextColor(COLOR color) {
    _textColor = color;
    COLOR actualTextColor = _textColor ? _textColor : _effectiveTintColor;
    for (int i=0 ; i<_segments.size() ; i++) {
        Segment& segment = _segments.at(i);
        segment.label->setDefaultColor(actualTextColor);
    }
}
void SegmentedControl::setSelectedTextColor(COLOR color) {
	_selectedTextColor = color;
    if (_selectedIndex >= 0) {
        Segment& selectedSegment = _segments.at(_selectedIndex);
        selectedSegment.label->setDefaultColor(color);
    }
}

void SegmentedControl::setSelectedSegment(int segmentIndex) {
	if (segmentIndex != _selectedIndex) {
        if (_selectedIndex >= 0) {
            Segment& selectedSegment = _segments.at(_selectedIndex);
            if (selectedSegment.label) {
                COLOR actualTextColor = _textColor ? _textColor : _effectiveTintColor;
                selectedSegment.label->setDefaultColor(actualTextColor);
                selectedSegment.rectOp->setColor(0);
            }
            // todo: rect fill color
        }
		_selectedIndex = segmentIndex;
		onSegmentTap(segmentIndex);
        if (_selectedIndex >= 0) {
            Segment& selectedSegment = _segments.at(_selectedIndex);
            if (selectedSegment.label) {
                selectedSegment.label->setDefaultColor(_selectedTextColor);
                selectedSegment.rectOp->setColor(_effectiveTintColor);
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
		_contentSize.height = MAX(_contentSize.height, segment.rect.size.height);
	}
	// Adjust cos rects overlap by a line width
	_contentSize.width -= (_segments.size()-1) * _lineWidth;
}

void SegmentedControl::measure(float parentWidth, float parentHeight) {
	View::measure(parentWidth, parentHeight);
	if (1==(((int)_rect.size.height)&1)) {
		_rect.size.height+=1;
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
        segment.rectOp->setColor((_pressedIndex==_selectedIndex) ? _effectiveTintColor : 0);
        invalidateRect(segment.rect);
    }
    _pressedIndex = pressedIndex;
    if (pressedIndex >=0) {
        Segment& segment = _segments.at(pressedIndex);
        segment.rectOp->setColor(0xc0000000 | (_effectiveTintColor&0xffffff));
        invalidateRect(segment.rect);
    }
}

bool SegmentedControl::onInputEvent(INPUTEVENT* event) {
	if (event->type == INPUT_EVENT_DOWN) {
		_pressedIndex = -1;
		for (int i=0 ; i<_segments.size() ; i++) {
			Segment& segment = _segments.at(i);
			if (segment.rect.contains(event->pt)) {
                setPressedIndex(i);
				break;
			}
		}
	} else if (event->type == INPUT_EVENT_UP) {
		if (_pressedIndex >= 0) {
            Segment& segment = _segments.at(_pressedIndex);
			if (segment.rect.contains(event->pt)) {
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
