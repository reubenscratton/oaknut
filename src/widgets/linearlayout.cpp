//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(LinearLayout);


LinearLayout::LinearLayout() : View(), _orientation(Horizontal) {
}

bool LinearLayout::applyStyleValue(const string& name, const StyleValue* value) {
    if (name=="orientation") {
        auto str = value->stringVal();
        if (str == "horizontal") {
            _orientation = Horizontal;
            return true;
        }
        if (str == "vertical") {
            _orientation = Vertical;
            return true;
        }
    } else if (name == "spacing") {
        setSpacing(value->floatVal());
        return true;
    } else if (name == "weights") {
        assert(value->isArray());
        auto weights = value->arrayVal();
        for (auto& weight : weights) {
            float f = weight.floatVal();
            _weights.push_back(f);
            _weightsTotal += f;
        }
        return true;
    }

    return View::applyStyleValue(name, value);
}


void LinearLayout::measure(float parentWidth, float parentHeight) {

	// Measure all children
    View::measure(parentWidth, parentHeight);
	
	// Belatedly update our own content size to be the size of our linearly arranged subviews
	_contentSize.height = 0;
	_contentSize.width = 0;
	for (int i=0 ; i<_subviews.size() ; i++) {
		View* view = _subviews.at(i);
        if (view == _scrollbarsView) continue;
		if (_orientation == Vertical) {
            _contentSize.height += view->getHeight();
			_contentSize.width = fmaxf(_contentSize.width, view->getWidth());
        } else {
            _contentSize.height = fmaxf(_contentSize.height, view->getHeight());
			_contentSize.width += view->getWidth();
        }
    }
    
    // Add min spacing between views
    float totalSpacing = _spacing * (_subviews.size()-1);
    if (_orientation == Vertical) {
        _contentSize.height += totalSpacing;
    } else {
        _contentSize.width += totalSpacing;
    }
	
	// If we're using weights to distribute leftover space among subviews, do that now
	if (_weightsTotal > 0) {
		if (_orientation == Vertical) {
            if (_heightMeasureSpec.type == MEASURESPEC::TypeRelative && _heightMeasureSpec.ref==nullptr) {
				float excess = (_rect.size.height - _contentSize.height) - (_padding.top+_padding.bottom);
				for (int i=0 ; i<_subviews.size() ; i++) {
					View* view = _subviews.at(i);
                    if (view == _scrollbarsView) continue;
					float excessForThisSubview = (_weights[i]/_weightsTotal)*excess;
                    RECT rect = view->getRect();
					rect.size.height += excessForThisSubview;
                    view->setRectSize(rect.size);
					_contentSize.height += excessForThisSubview;
				}
			}
        } else {
			if (_widthMeasureSpec.type == MEASURESPEC::TypeRelative && _widthMeasureSpec.ref==nullptr) {
				float excess = (_rect.size.width - _contentSize.width) - (_padding.left+_padding.right);
				for (int i=0 ; i<_subviews.size() ; i++) {
					View* view = _subviews.at(i);
                    if (view == _scrollbarsView) continue;
					float excessForThisSubview = (_weights[i]/_weightsTotal)*excess;
                    if (excessForThisSubview > 0) {
                        RECT rect = view->getRect();
                        rect.size.width += excessForThisSubview;
                        view->setRectSize(rect.size);
                        _contentSize.height = fmaxf(_contentSize.height, view->getHeight());
                        _contentSize.width += excessForThisSubview;
                    }
				}
			}
		}
	}

	
	// If we're wrap-content, update to reflect the change in content size
	if (_widthMeasureSpec.type == MEASURESPEC::TypeContent) {
		_rect.size.width = _padding.left + _contentSize.width + _padding.right;
	}
	if (_heightMeasureSpec.type == MEASURESPEC::TypeContent) {
		_rect.size.height = _padding.top + _contentSize.height + _padding.bottom;
	}

}


void LinearLayout::layout() {
    View::layout();
	//_contentSize.height = 0;
	//_contentSize.width = 0;
	POINT pt = {_padding.left,_padding.top};
    if (_orientation == Vertical) {
        for (int i=0 ; i<_subviews.size() ; i++) {
            View* view = _subviews.at(i);
            if (view == _scrollbarsView) continue;
			view->layout();
            view->setRectOrigin(pt);
            pt.y += view->getHeight() + _spacing;
        }
    } else if (_orientation == Horizontal) {
        for (int i=0 ; i<_subviews.size() ; i++) {
            View* view = _subviews.at(i);
            if (view == _scrollbarsView) continue;
			view->layout();
            view->setRectOrigin(pt);
            pt.x += view->getWidth() + _spacing;
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

void LinearLayout::setSpacing(float spacing) {
    if (_spacing != spacing) {
        _spacing = spacing;
        setNeedsLayout();
    }
}
