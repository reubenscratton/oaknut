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


void LinearLayout::layoutSubviews(RECT constraint) {
    
    // Do an initial layout to work out the minimum total size of all our subviews arranged linearly
    float totalSize = 0;
    int numSubviews = getSubviewCount();
    for (int i=0 ; i<numSubviews ; i++) {
        View* view = _subviews.at(i);
        //if (!view->_layoutValid) {
            view->layout(constraint);
        //}
        totalSize += (_orientation == Vertical) ? view->getHeight() : view->getWidth();
    }
    totalSize += _spacing * (numSubviews-1);

    // Reposition subviews linearly, also distributing leftover space according to weights. Subviews
    // that receive extra space will be layout()d a second time.
    float excess;
    if (_orientation == Vertical) {
        excess = (constraint.size.height - totalSize) - (_padding.top+_padding.bottom);
    } else {
        excess = (constraint.size.width - totalSize) - (_padding.left+_padding.right);
    }
    POINT origin = {_padding.left, _padding.top};
    for (int i=0 ; i<_subviews.size() ; i++) {
        View* view = _subviews.at(i);
        if (view == _scrollbarsView) continue;
        float excessForThisSubview = (_weights[i]/_weightsTotal)*excess;
        if (excessForThisSubview > 0) {
            MEASURESPEC& spec = (_orientation == Vertical) ? view->_heightMeasureSpec : view->_widthMeasureSpec;
            spec.con += excessForThisSubview;
            //app.log("Growing by %f, con=%f", excessForThisSubview, constraint.size.width);
            view->_layoutValid = false;
            view->invalidateContentSize();
            view->layout(constraint);
            spec.con -= excessForThisSubview;
        }
        view->setRectOrigin(origin);
        if (_orientation == Vertical) {
            origin.y += view->getHeight() + _spacing;
        } else {
            origin.x += view->getWidth() + _spacing;
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
