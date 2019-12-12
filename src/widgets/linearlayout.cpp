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

bool LinearLayout::applySingleStyle(const string& name, const style& value) {
    if (name=="orientation") {
        auto str = value.stringVal();
        if (str == "horizontal") {
            _orientation = Horizontal;
            return true;
        }
        if (str == "vertical") {
            _orientation = Vertical;
            return true;
        }
    } else if (name == "spacing") {
        setSpacing(value.floatVal());
        return true;
    } else if (name == "weights") {
        assert(value.isArray());
        auto weights = value.arrayVal();
        for (auto& weight : weights) {
            float f = weight.floatVal();
            _weights.push_back(f);
            _weightsTotal += f;
        }
        return true;
    }

    return View::applySingleStyle(name, value);
}


void LinearLayout::layout(RECT constraint) {
    if (_updateSubviewAligns) {
        _updateSubviewAligns = false;
        
        for (int i=1 ; i<_subviews.size() ; i++) {
            ALIGNSPEC& alignspec = (_orientation == Vertical) ? _subviews[i]->_alignspecVert : _subviews[i]->_alignspecHorz;
            alignspec.anchor = _subviews[i-1];
            alignspec.multiplierAnchor = 1;
            alignspec.margin = _spacing;
        }
    }
    View::layout(constraint);
}

void LinearLayout::layoutSubviews(RECT constraint) {
    
    // NB: Padding has already been applied!
    
    
    // Do an initial layout to work out the minimum total size of all our subviews arranged linearly
    float totalSize = 0;//paddingPre + paddingPost;
    int numSubviews = getSubviewCount();
    for (int i=0 ; i<numSubviews ; i++) {
        View* view = _subviews.at(i);
        view->layout(constraint);
        totalSize += (_orientation == Vertical) ? view->getHeight() : view->getWidth();
    }
    totalSize += _spacing * (numSubviews-1);

    // Reposition subviews linearly, also distributing leftover space according to weights. Subviews
    // that receive extra space will be layout()d a second time.
    float excess = (_orientation == Vertical) ? constraint.size.height : constraint.size.width;
    excess -= totalSize;
    if (excess > 0) {
        for (int i=0 ; i<_subviews.size() ; i++) {
            View* view = _subviews.at(i);
            float excessForThisSubview = _weightsTotal ? (_weights[i]/_weightsTotal)*excess : 0;
            if (excessForThisSubview > 0) {
                MEASURESPEC& spec = (_orientation == Vertical) ? view->_heightMeasureSpec : view->_widthMeasureSpec;
                float currentSize = (_orientation == Vertical) ? view->_rect.size.height : view->_rect.size.width;
                MEASURESPEC realspec = spec;
                spec = MEASURESPEC::Abs(currentSize + excessForThisSubview);
                view->_layoutValid = false;
                //view->setNeedsLayout();
                //view->invalidateContentSize();
                view->layout(constraint);
                spec = realspec;
            } else {
                view->layout(constraint);
            }
        }
    }
}

void LinearLayout::insertSubview(View* subview, int index) {
    _updateSubviewAligns = true;
    View::insertSubview(subview, index);
    if (_weights.size() < _subviews.size()) {
        _weights.insert(_weights.begin()+index, 0);
    }
}

void LinearLayout::removeSubview(View *subview) {
    _updateSubviewAligns = true;
	int index = indexOfSubview(subview);
	View::removeSubview(subview);
    if (_weights.size() > _subviews.size()) {
        _weights.erase(_weights.begin() + index);
    }
}

void LinearLayout::setSubviewWeight(View* subview, float weight) {
	int index = indexOfSubview(subview);
    assert(index>=0 && index<_weights.size());
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
