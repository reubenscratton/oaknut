//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


DECLARE_DYNCREATE(SegmentedControl);


SegmentedControl::SegmentedControl() {
    applyStyle("SegmentedControl");
	_selectedIndex = _pressedIndex = -1;
}

bool SegmentedControl::applyStyleValue(const string &name, const StyleValue *value) {
    if (name == "stroke-width") {
        _lineWidth = value->floatVal();
        return true;
    }
    if (name=="font-name") {
        setFontName(value->stringVal());
        return true;
    }
    if (name=="font-size") {
        setFontSize(value->floatVal());
        return true;
    }
    if (name=="corner-radius") {
        _cornerRadius = value->floatVal();
        updateBorders();
        return true;
    }
    return View::applyStyleValue(name, value);
}

void SegmentedControl::setFontName(const string& fontName) {
    if (_fontName != fontName) {
        _fontName = fontName;
        _fontValid = false;
        invalidateContentSize();
    }
}
void SegmentedControl::setFontSize(float fontSize) {
    if (_fontSize != fontSize) {
        _fontSize = fontSize;
        _fontValid = false;
        invalidateContentSize();
    }
}

void SegmentedControl::onEffectiveTintColorChanged() {
    View::onEffectiveTintColorChanged();
    for (auto i : _segments) {
        i.rectOp->setStrokeColor(_effectiveTintColor);
        COLOR actualTextColor = _textColor ? _textColor : _effectiveTintColor;
        i.label->setColor(actualTextColor);
    }
}

void SegmentedControl::addSegment(const string& label) {
    Segment segment;
    segment.label = new TextRenderer();
	segment.label->setText(label);
    segment.label->setGravity({GRAVITY_CENTER,GRAVITY_CENTER});
    COLOR actualColor = _textColor ? _textColor : _effectiveTintColor;
    segment.label->setColor(actualColor);
    float r = _cornerRadius;
    segment.rectOp = new RectRenderOp(this);
    segment.rectOp->setFillColor(0);
    addRenderOp(segment.rectOp);
	_segments.push_back(segment);
    updateBorders();
    _fontValid = false;
	invalidateContentSize();
}

void SegmentedControl::updateBorders() {
    for (int i=0 ; i<_segments.size() ; i++) {
        auto& op = _segments[i].rectOp;
        bool first = i==0;
        bool last = (i==_segments.size()-1);
        op->setStrokeWidth(_lineWidth);
        op->setStrokeColor(_effectiveTintColor);
        if (first && last) {
            op->setCornerRadius(_cornerRadius);
        } else if (first && !last) {
            op->setCornerRadii({_cornerRadius,0,_cornerRadius,0}); // left
        } else if (last && !first) {
            op->setCornerRadii({0,_cornerRadius,0,_cornerRadius}); // right
        } else {
            op->setCornerRadii({0,0,0,0}); // middle
        }
    }
}

void SegmentedControl::setTextColor(COLOR color) {
    _textColor = color;
    COLOR actualTextColor = _textColor ? _textColor : _effectiveTintColor;
    for (int i=0 ; i<_segments.size() ; i++) {
        Segment& segment = _segments.at(i);
        segment.label->setColor(actualTextColor);
    }
}
void SegmentedControl::setSelectedTextColor(COLOR color) {
	_selectedTextColor = color;
    if (_selectedIndex >= 0) {
        Segment& selectedSegment = _segments.at(_selectedIndex);
        selectedSegment.label->setColor(color);
    }
}

void SegmentedControl::setSelectedSegment(int segmentIndex) {
	if (segmentIndex != _selectedIndex) {
        if (_selectedIndex >= 0) {
            Segment& selectedSegment = _segments.at(_selectedIndex);
            if (selectedSegment.label) {
                COLOR actualTextColor = _textColor ? _textColor : _effectiveTintColor;
                selectedSegment.label->setColor(actualTextColor);
                selectedSegment.rectOp->setColor(0);
            }
            // todo: rect fill color
        }
		_selectedIndex = segmentIndex;
		onSegmentTap(segmentIndex);
        if (_selectedIndex >= 0) {
            Segment& selectedSegment = _segments.at(_selectedIndex);
            if (selectedSegment.label) {
                selectedSegment.label->setColor(_selectedTextColor);
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
    
    if (!_fontValid) {
        _font = Font::get(_fontName, _fontSize);
        for (auto& segment : _segments) {
            segment.label->setFont(_font);
        }
        _fontValid = true;
    }
    
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
