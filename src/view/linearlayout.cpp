//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(LinearLayout);


LinearLayout::LinearLayout() : View(), _orientation(Horizontal) {
}

bool LinearLayout::applyStyleValue(const string& name, StyleValue* value) {
    if (name=="orientation") {
        if (value->type != StyleValue::Type::String) return false;
        if (value->str == "horizontal") {
            _orientation = Horizontal;
            return true;
        }
        if (value->str == "vertical") {
            _orientation = Vertical;
            return true;
        }
    }
    return View::applyStyleValue(name, value);
}

bool LinearLayout::applyStyleValueFromChild(const string& name, StyleValue* value, View* subview) {
    if (name == "weight") {
        setWeight(subview, value->getAsFloat());
        return true;
    }
    return false;
}

void LinearLayout::measure(float parentWidth, float parentHeight) {

	// Measure all children
    View::measure(parentWidth, parentHeight);
	
	// Belatedly update our own content size to be the size of our linearly arranged subviews
	_contentSize.height = 0;
	_contentSize.width = 0;
	for (int i=0 ; i<_subviews.size() ; i++) {
		View* view = _subviews.at(i);
		if (_orientation == Vertical) {
            _contentSize.height += view->_frame.size.height;
			_contentSize.width = fmaxf(_contentSize.width, view->_frame.size.width);
        } else {
            _contentSize.height = fmaxf(_contentSize.height, view->_frame.size.height);
			_contentSize.width += view->_frame.size.width;
        }
    }
	
	// If we're using weights to distribute leftover space among subviews, do that now
	if (_weightsTotal > 0) {
		if (_orientation == Vertical) {
			if (_heightMeasureSpec.refType == REFTYPE_VIEW && _heightMeasureSpec.refView==nullptr) {
				float excess = (_frame.size.height - _contentSize.height) - (_padding.top+_padding.bottom);
				for (int i=0 ; i<_subviews.size() ; i++) {
					View* view = _subviews.at(i);
					float excessForThisSubview = (_weights[i]/_weightsTotal)*excess;
					view->_frame.size.height += excessForThisSubview;
					_contentSize.height += excessForThisSubview;
				}
			}
        } else {
			if (_widthMeasureSpec.refType == REFTYPE_VIEW && _widthMeasureSpec.refView==nullptr) {
				float excess = (_frame.size.width - _contentSize.width) - (_padding.left+_padding.right);
				for (int i=0 ; i<_subviews.size() ; i++) {
					View* view = _subviews.at(i);
					float excessForThisSubview = (_weights[i]/_weightsTotal)*excess;
					view->_frame.size.width += excessForThisSubview;
					_contentSize.width += excessForThisSubview;
				}
			}
		}
	}

	
	// If we're wrap-content, update to reflect the change in content size
	if (_widthMeasureSpec.refType == REFTYPE_CONTENT) {
		_frame.size.width = _padding.left + _contentSize.width + _padding.right;
	}
	if (_heightMeasureSpec.refType == REFTYPE_CONTENT) {
		_frame.size.height = _padding.top + _contentSize.height + _padding.bottom;
	}

}


void LinearLayout::layout() {
    View::layout();
	_contentSize.height = 0;
	_contentSize.width = 0;
	POINT pt = {_padding.left,_padding.top};
    if (_orientation == Vertical) {
        for (int i=0 ; i<_subviews.size() ; i++) {
            View* view = _subviews.at(i);
			view->layout();
            view->setFrameOrigin(pt);
            pt.y += view->_frame.size.height;
        }
    } else if (_orientation == Horizontal) {
        for (int i=0 ; i<_subviews.size() ; i++) {
            View* view = _subviews.at(i);
			view->layout();
            view->setFrameOrigin(pt);
            pt.x += view->_frame.size.width;
        }
    }
}

void LinearLayout::addSubview(View *subview) {
	View::addSubview(subview);
	_weights.push_back(0);
}

void LinearLayout::removeSubview(View *subview) {
	int index = indexOfSubview(subview);
	View::removeSubview(subview);
	_weights.erase(_weights.begin() + index);
}

void LinearLayout::setWeight(View* subview, float weight) {
	int index = indexOfSubview(subview);
	_weightsTotal += weight - _weights[index];
	_weights[index] = weight;
	setNeedsLayout();	
}
