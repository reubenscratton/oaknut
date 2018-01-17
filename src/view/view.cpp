//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

map<string, View* (*)()>* s_classRegister;
View* DYNCREATE(const string& className) {
    return s_classRegister->find(className)->second();
}

DECLARE_DYNCREATE(View);

MEASURESPEC MEASURESPEC_Make(int refType, View* refView, float refSizeMultiplier, float abs) {
    MEASURESPEC r;
    r.refType = refType;
    r.refView = refView;
    r.refSizeMultiplier = refSizeMultiplier;
    r.abs = abs;
    return r;    
}

#define NO_ANCHOR ((View*)-1)

ALIGNSPEC ALIGNSPEC_Make(View* anchor, float multiplierAnchor, float multiplierSelf, float margin) {
    ALIGNSPEC r;
    r.anchor = anchor;
    r.multiplierAnchor = multiplierAnchor;
    r.multiplierSelf = multiplierSelf;
    r.margin = margin;
    return r;
}

View::View() : _alpha(1.0f), _touchEnabled(true) {
    _visibility = VISIBILITY_VISIBLE;
	_widthMeasureSpec = MEASURESPEC_None;
	_heightMeasureSpec = MEASURESPEC_None;
	_alignspecHorz = ALIGNSPEC_None;
	_alignspecVert = ALIGNSPEC_None;
	onTouchEventDelegate = NULL;
}

View::~View() {
//	oakLog("~View()");
}

void View::applyStyleValues(const StyleValueList& values) {
    for (auto i : values) {
        if (!applyStyleValue(i.first, i.second)) {
            if (_parent && !_parent->applyStyleValueFromChild(i.first, i.second, this)) {
                oakLog("Warning: ignored unknown attribute '%s'", i.first.data());
            }
        }
    }
}

MEASURESPEC measurespecFromResourceVal(StyleValue* value) {
    if (value->type == StyleValue::Type::Int || value->type == StyleValue::Type::Double) {
        return MEASURESPEC_Abs(value->getAsFloat());
    }
    assert(value->type == StyleValue::Type::String);
    if (value->str == "wrap_content") return MEASURESPEC_WrapContent;
    if (value->str == "fill_parent") return MEASURESPEC_FillParent;
    if (value->str == "match_parent") return MEASURESPEC_FillParent;
    if (stringStartsWith(value->str, "aspect(", true)) {
        return MEASURESPEC_UseAspect(stringParseDouble(value->str));
    }
    assert(false); // unknown measurespec
    return MEASURESPEC_WrapContent;
}

ALIGNSPEC alignspecFromResourceVal(StyleValue* value, View* view) {
    assert(value->type == StyleValue::Type::String);
    ALIGNSPEC spec;
    string str = value->str;
    if (stringStartsWith(str,"center",true)) spec=ALIGNSPEC_Center;
    else if (stringStartsWith(str, "centre", true)) spec=ALIGNSPEC_Center;
    else if (stringStartsWith(str, "left", true)) spec=ALIGNSPEC_Left;
    else if (stringStartsWith(str, "right", true)) spec=ALIGNSPEC_Right;
    else if (stringStartsWith(str, "top", true)) spec=ALIGNSPEC_Top;
    else if (stringStartsWith(str, "bottom", true)) spec=ALIGNSPEC_Bottom;
    else if (stringStartsWith(str, "below(", true)) {
        string anchorId = stringExtractUpTo(str, ")", true);
        View* anchor = view->_parent->findViewById(anchorId);
        assert(anchor); // NB: anchor must be previously declared. TODO: remove this restriction
        spec= ALIGNSPEC_Below(anchor, 0);
    } else {
        assert(false); // unknown alignspec
    }
    stringTrim(str);
    if (stringStartsWith(str, ",", true)) {
        stringTrim(str);
        spec.margin = stringParseDimension(str);
    }
    return spec;
}


bool View::applyStyleValue(const string& name, StyleValue* value) {
    if (name == "id") {
        _id = value->str;
        return true;
    } else if (name == "style") {
        StyleValueList styles;
        for (auto i : value->styleMap->_valuesList) {
            styles.push_back(make_pair(i.first, i.second->select()));
        }
        applyStyleValues(styles);
        return true;
    } else if (name == "height") {
        _heightMeasureSpec = measurespecFromResourceVal(value);
        return true;
    } else if (name == "width") {
        _widthMeasureSpec = measurespecFromResourceVal(value);
        return true;
    } else if (name == "alignX") {
        _alignspecHorz = alignspecFromResourceVal(value, this);
        return true;
    } else if (name == "alignY") {
        _alignspecVert = alignspecFromResourceVal(value, this);
        return true;
    } else if (name == "background") {
        assert(value->type == StyleValue::Type::Int);
        setBackgroundColour(value->i);
        return true;
    } else if (name == "padding") {
        float pad = value->getAsFloat();
        setPadding(EDGEINSETS(pad,pad,pad,pad));
        return true;
    } else if (name == "paddingHorz") {
        float pad = value->getAsFloat();
        setPadding(EDGEINSETS(pad,_padding.top,pad,_padding.bottom));
        return true;
    } else if (name == "paddingVert") {
        float pad = value->getAsFloat();
        setPadding(EDGEINSETS(_padding.left,pad,_padding.right,pad));
        return true;
    } else if (name == "tint") {
        setTintColour(value->i);
        return true;
    }
    return false;
}

bool View::applyStyleValueFromChild(const string& name, StyleValue* value, View* subview) {
    return false;
}

void View::setNeedsFullRedraw() {
	if (!_window) {
		return;
	}
	if (!_needsFullRedraw) {
		_needsFullRedraw = true;
        for (auto it=_renderList.begin() ; it!=_renderList.end() ; it++) {
            (*it)->_mustRedraw = true;
        }
		for (int i=0 ; i<_subviews.size() ; i++) {
			View* view = _subviews.at(i);
			view->setNeedsFullRedraw();
		}
		if (_parent && !_opaque) {
			_parent->invalidateRect(_frame);
		}
	}
    if (!_window->_redrawNeeded) {
        _window->requestRedraw();
    }
}

void View::updateRenderOps() {
    assert(false); // this method should be overridden if needed, hence default version should be never called
}

void View::invalidateRect(const RECT& rect) {
    if (!_surface) {
        return;
    }

    // invalidateRect is only meaningful when partial redraw works (ie private surfaces)
	if (!_surface->_supportsPartialRedraw) {
        setNeedsFullRedraw();
		return;
	}

    // Add the given rect to the invalid region, unless we're already flagged as needing the full redraw
	if (!_needsFullRedraw) {
        
        // Convert view coords to surface coords. If the view owns a private surface then
        // view coords are same as surface coords, no translation is needed
        RECT surfaceRect = rect;
        if (!_ownsPrivateSurface) {
            surfaceRect.origin.x += _surfaceOrigin.x;
            surfaceRect.origin.y += _surfaceOrigin.y;
        }
        _surface->_invalidRegion.addRect(surfaceRect);
        
        // Find the intersecting ops and mark them as needing a redraw
        for (auto it=_renderList.begin() ; it!=_renderList.end() ; it++) {
            RenderOp* op = *it;
            if (surfaceRect.intersects(op->_rect)) {
                op->_mustRedraw = true;
            }
        }
        
        // If this view is not opaque then whatever is behind it must also redraw
		if (_parent && !_opaque) {
			RECT prect = rect;
			prect.origin.x += _frame.origin.x;
			prect.origin.y += _frame.origin.y;
			_parent->invalidateRect(prect);
		}
	}
}
void View::setUsePrivateSurface(bool usePrivateSurface) {
	if (usePrivateSurface != _ownsPrivateSurface) {
		_ownsPrivateSurface = usePrivateSurface;
        if (_window) {
            updatePrivateSurface(true);
            setNeedsFullRedraw();
        }
	}
}


void View::setFrameOrigin(const POINT& pt) {
    POINT d;
    d.x = pt.x - _frame.origin.x;
    d.y = pt.y - _frame.origin.y;
    if (d.x || d.y) {
        _frame.origin = pt;
        adjustSurfaceOrigin(d);
    }
}

void View::adjustSurfaceOrigin(const POINT& d) {
    _surfaceOrigin += d;
    
    // If this view is a surface owner, no need to propagate the change to renderlist or subviews
    if (_ownsPrivateSurface) {
        if (_surface && _surface->_op) {
            _surface->_op->_dirty = true;
        }
        return;
    }
    
    // If the view is 'live' - ie part of a window and visible - then the
    // render order might be affected and the batch quads must be updated
    if (_window) {
        for (auto it : _renderList) {
            it->rebatchIfNecessary();
        }
        _window->requestRedraw();
    }
    
    // Propagate change in surface origin to subviews
    for (auto it=_subviews.begin() ; it!=_subviews.end() ; it++) {
        (*it)->adjustSurfaceOrigin(d);
    }
}

/*void View::setFrameSize(SIZE frameSize) {
	_frame = frame;
	if (_background) {
		_background->setBounds(frame);
	}
	setNeedsRedraw();
}*/

void View::setMeasureSpecs(MEASURESPEC widthMeasureSpec, MEASURESPEC heightMeasureSpec) {
	_widthMeasureSpec = widthMeasureSpec;
	_heightMeasureSpec = heightMeasureSpec;
	setNeedsLayout();
}

void View::setAlignSpecs(ALIGNSPEC alignspecHorz, ALIGNSPEC alignspecVert) {
	_alignspecHorz = alignspecHorz;
	_alignspecVert = alignspecVert;
	setNeedsLayout();
}


void View::setNeedsLayout() {
	//needsLayout = true;
	if (_window) {
		_window->setNeedsLayout();
	}
}

void View::invalidateContentSize() {
	_contentSizeValid = false;
	if (_widthMeasureSpec.refType==REFTYPE_CONTENT || _heightMeasureSpec.refType==REFTYPE_CONTENT) {
		setNeedsLayout();
    } else {
        setNeedsFullRedraw();
    }
}

void View::updateContentSize(float parentWidth, float parentHeight) {
	// no-op
}

void View::setGravity(GRAVITY gravity) {
	_gravity = gravity;
	setNeedsLayout();
}

void View::setVisibility(int visibility) {
    if (_visibility != visibility) {
        if ((_visibility==VISIBILITY_GONE && visibility!=VISIBILITY_GONE) ||
            (_visibility!=VISIBILITY_GONE && visibility==VISIBILITY_GONE)) {
            setNeedsLayout();
        }
        _visibility = visibility;
        if (visibility == VISIBILITY_VISIBLE) {
            if (_parent && _parent->_window) {
                attachToWindow(_parent->_window);
            }
        } else {
            if (_window) {
                detachFromWindow();
            }
        }
    }
}


//
//
void View::measure(float parentWidth, float parentHeight) {

	if (!_contentSizeValid) {
		updateContentSize(parentWidth, parentHeight);
		_contentSizeValid = true;
	}
    
    // Width
	if (REFTYPE_CONTENT == _widthMeasureSpec.refType) {
        _frame.size.width = _contentSize.width + _padding.left + _padding.right;
	} else if (REFTYPE_ABS == _widthMeasureSpec.refType) {
        _frame.size.width = _widthMeasureSpec.abs;
    } else if (REFTYPE_VIEW == _widthMeasureSpec.refType) {
        float refWidth = (_widthMeasureSpec.refView == NULL) ? parentWidth : _widthMeasureSpec.refView->_frame.size.width;
        _frame.size.width = refWidth * _widthMeasureSpec.refSizeMultiplier;
		_frame.size.width += _widthMeasureSpec.abs;
    }

    
    // Height
	if (REFTYPE_CONTENT == _heightMeasureSpec.refType) {
		_frame.size.height = _contentSize.height + _padding.top + _padding.bottom;
	} else if (REFTYPE_ABS == _heightMeasureSpec.refType) {
		_frame.size.height = _heightMeasureSpec.abs;
	} else if (REFTYPE_VIEW == _heightMeasureSpec.refType) {
		float refHeight = (_heightMeasureSpec.refView == NULL) ? parentHeight : _heightMeasureSpec.refView->_frame.size.height;
		_frame.size.height = refHeight * _heightMeasureSpec.refSizeMultiplier;
		_frame.size.height += _heightMeasureSpec.abs;
	}
	
    // Aspect
    if (REFTYPE_ASPECT == _widthMeasureSpec.refType) {
        _frame.size.width = _frame.size.height * _widthMeasureSpec.refSizeMultiplier;
    } else if (REFTYPE_ASPECT == _heightMeasureSpec.refType) {
        _frame.size.height = _frame.size.width * _heightMeasureSpec.refSizeMultiplier;
    }
	
	
	    
    // At this point we've done relative-to-parent sizing. But if a size value depends on
    // wrapping subview size(s) then pass down the parent size.
	bool wrapWidth = _contentSize.width<=0 && _widthMeasureSpec.refType == REFTYPE_CONTENT;
	bool wrapHeight =  _contentSize.height<=0 && _heightMeasureSpec.refType == REFTYPE_CONTENT;
	float widthForSubviewMeasure = wrapWidth ? parentWidth : _frame.size.width;
	float heightForSubviewMeasure = wrapHeight ? parentHeight : _frame.size.height;

	// Adjust for padding
	widthForSubviewMeasure -= (_padding.left+_padding.right);
	heightForSubviewMeasure -= (_padding.top+_padding.bottom);

	// Measure subviews and track largest sizes
    float largestChildWidth = 0;
    float largestChildHeight = 0;
    for (int i=0 ; i<_subviews.size() ; i++) {
        View* view = _subviews.at(i);
        view->measure(widthForSubviewMeasure, heightForSubviewMeasure);
        largestChildWidth = fmaxf(largestChildWidth, view->_frame.size.width);
        largestChildHeight = fmaxf(largestChildHeight, view->_frame.size.height);
    }
    
    // wrap_content
    if (wrapWidth) {
        _frame.size.width = _padding.left + largestChildWidth + _padding.right;
    }
    if (wrapHeight) {
        _frame.size.height = _padding.bottom + largestChildHeight + _padding.top;
    }
	
	// Align view sizes to pixel grid
	_frame.size.width = floorf(_frame.size.width);
	_frame.size.height = floorf(_frame.size.height);

}



void View::layout() {

	// Get anchor view size
	View* anchorHorz = (_alignspecHorz.anchor == NO_ANCHOR) ? NULL : (_alignspecHorz.anchor?_alignspecHorz.anchor:_parent);
	View* anchorVert = (_alignspecVert.anchor == NO_ANCHOR) ? NULL : (_alignspecVert.anchor?_alignspecVert.anchor:_parent);
	
    // Get anchor dimensions
    float anchorY = 0;
    float anchorX = 0;
    float anchorWidth = anchorHorz ? anchorHorz->_frame.size.width : mainWindow->_surfaceRect.size.width;
    float anchorHeight = anchorVert ? anchorVert->_frame.size.height : mainWindow->_surfaceRect.size.height;
	if (anchorHorz) {
		if (anchorHorz == _parent) {
			anchorX = _parent->_padding.left;
			anchorWidth -= (anchorHorz->_padding.left+anchorHorz->_padding.right);
		} else {
			if (anchorHorz->_parent != _parent) {
				oakLog("Warning! View::layout() is not clever enough for non-sibling anchors");
			}
			anchorX = anchorHorz->_frame.origin.x;
		}
    }
	if (anchorVert) {
		if (anchorVert == _parent) {
			anchorY = _parent->_padding.top;
			anchorHeight -= (anchorVert->_padding.top+anchorVert->_padding.bottom);
		} else {
			if (anchorVert->_parent != _parent) {
				oakLog("Warning! View::layout() is not clever enough for non-sibling anchors");
			}
			anchorY = anchorVert->_frame.origin.y;
		}
    }
	
	

    // Positioning
    POINT pt;
    if (anchorHorz) {
        pt.x = anchorX + (_alignspecHorz.multiplierAnchor * anchorWidth)
								  + (_alignspecHorz.multiplierSelf * _frame.size.width)
								  + _alignspecHorz.margin;
        pt.x = floorf(pt.x);
    }
    if (anchorVert) {
        pt.y = anchorY + (_alignspecVert.multiplierAnchor * anchorHeight)
                       + (_alignspecVert.multiplierSelf * _frame.size.height);
        pt.y = floorf(pt.y);
        pt.y += _alignspecVert.margin;
    }
    if (anchorHorz || anchorVert) {
        if (!anchorHorz) pt.x = _frame.origin.x;
        if (!anchorVert) pt.y = _frame.origin.y;
        setFrameOrigin(pt);
    }
	
    if (_currentBackgroundOp) {
        _currentBackgroundOp->setRect(RECT_Make(0, 0, _frame.size.width, _frame.size.height));
    }    


	/*
	if (_widthMeasureSpec.refType != REFTYPE_NONE) {
		_frame.origin.x = 0; // todo: apply grabbity
	}
	if (_heightMeasureSpec.refType != REFTYPE_NONE) {
		_frame.origin.y = 0; // todo: apply grabbity
	}*/
    

    for (int i=0 ; i<_subviews.size() ; i++) {
        View* view = _subviews.at(i);
        view->layout();
        //view->_frame.origin.x += _padding.left;
        //view->_frame.origin.y += _padding.bottom;
    }
	
    updateScrollbars();
    setNeedsFullRedraw();
}



void View::setPadding(EDGEINSETS padding) {
	this->_padding = padding;
    setNeedsLayout();
}

void View::updatePrivateSurface(bool updateSubviews) {
    assert(!_surface);
    _surface = _parent ? _parent->_surface : _window->_surface;
    if (_ownsPrivateSurface) {
        _surface = new Surface(this);
        _needsFullRedraw = true;
    }

    if (updateSubviews) {
        for (auto it = _subviews.begin(); it!=_subviews.end() ; it++) {
            ObjPtr<View> subview = *it;
            subview->updatePrivateSurface(true);
        }
    }
}


void View::attachToWindow(Window *window) {
    if (_visibility != VISIBILITY_VISIBLE) {
        return;
    }
	_window = window;
    if (_parent) {
        _surfaceOrigin = POINT_Make(_parent->_surfaceOrigin.x + _frame.origin.x, _parent->_surfaceOrigin.y + _frame.origin.y);
    } else {
        _surfaceOrigin = _frame.origin;
    }

    updatePrivateSurface(false);
    updateEffectiveAlpha();
    updateEffectiveTint();
    
    if (_surface) {
        _surface->attachViewOps(this);
    }
    
	for (auto it = _subviews.begin(); it!=_subviews.end() ; it++) {
		ObjPtr<View> subview = *it;
        if (subview->_visibility == VISIBILITY_VISIBLE) {
            subview->attachToWindow(window);
        }
	}
}


void View::detachFromWindow() {
	if (!_window) {
        return;
    }
    
    if (_scrollFadeAnim) {
        _scrollFadeAnim->stop();
        _scrollFadeAnim = NULL;
    }
    if (_scrollFadeTimer) {
        _scrollFadeTimer->stop();
        _scrollFadeTimer = NULL;
    }

    
    // Unbatch our ops
    if (_surface) {
        _surface->detachViewOps(this);
    }

    // Recurse through subviews
    for (vector<ObjPtr<View>>::iterator it = _subviews.begin(); it!=_subviews.end() ; it++) {
        ObjPtr<View> subview = *it;
        subview->detachFromWindow();
    }

    // If this view owns its surface, remove the op that renders it from the render target
    if (_ownsPrivateSurface && _surface->_op) {
        _surface->_op = NULL;
    }

    _surface = nullptr;
    _window = nullptr;
    _nextView = nullptr;
    _previousView = nullptr;

}


View* View::findViewById(const string& id) {
    if (id == _id) {
        return this;
    }
    for (auto it : _subviews) {
        if (it->_id == id) {
            return it;
        }
    }
    for (auto it : _subviews) {
        View* view = (it->findViewById(id));
        if (view) {
            return view;
        }
    }
    return NULL;
}

void View::addSubview(View* subview) {
	insertSubview(subview, (int)_subviews.size());
}
void View::insertSubview(View* subview, int index) {
    
    /*
     
     This is a view tree with node labels indicating render order:

          __1__
         /  |  \
        2   5   8
       / \ / \  | \
      3  4 6  7 9  10

     
     Imagine 5 has no parent and is then added as a subview of 1. To implement render order we
     must determine that 4 and 5 are linked, as are 7 and 8.
     
         ______1______
        /             \
       2       5       8
      / \     / \     / \
     3  4    6   7   9  10

     To get from 5 to 4 we start with 5's left sibling, in this case 2, and then find rightmost 
     descendent of 2. If 5 had no left sibling then the prevView would be parent (ie 1).
    
     */
    View* prev = this;
    if (index > 0) {
        prev = _subviews.at(index-1); // left sibling
        while (prev->_subviews.size() > 0) {
            prev = prev->_subviews.at(prev->_subviews.size()-1);
        }
    }
    ObjPtr<View> next = prev->_nextView;  // keep a ref to 8
    subview->_previousView = prev; // link 5 to 4
    prev->_nextView = subview;     // link 4 to 5
    
    // Find rightmost descendent of the subview (7)
    View* rightmost = subview;
    while (rightmost->_subviews.size() > 0) {
        rightmost = rightmost->_subviews.at(rightmost->_subviews.size()-1);
    }
    assert(!rightmost->_nextView);
    rightmost->_nextView = next;     // link 7 to 8
    if (next) {
        next->_previousView = rightmost; // link 8 to 7
    }

    
	_subviews.insert(_subviews.begin()+index, subview);
	subview->_parent = this;
	if (_window) {
		subview->attachToWindow(_window);
        subview->setNeedsLayout();
	}
}


int View::indexOfSubview(View* subview) {
	int i=0;
	for (vector<ObjPtr<View>>::iterator it = _subviews.begin() ; it != _subviews.end() ; it++) {
		if (subview == (*it)) {
			return i;
		}
		i++;
	}
	return -1;
}
void View::removeSubview(View* subview) {
	assert(subview->_parent == this);
	int i=indexOfSubview(subview);
	assert(i>=0);
	if (i>=0) {
        if (i==0) {
            assert(subview->_previousView == this);
        }
        
        // Find rightmost view
        View* rightmost = subview;
        while (rightmost->_subviews.size() > 0) {
            rightmost = rightmost->_subviews.at(rightmost->_subviews.size()-1);
        }

        if (subview->_previousView) {
            subview->_previousView->_nextView = rightmost->_nextView;
        }
        if (rightmost->_nextView) {
            rightmost->_nextView->_previousView = subview->_previousView;
        }
        
		if (_window) {
			subview->detachFromWindow();
		}
		_subviews.erase(_subviews.begin() + i);
        
	}
}
void View::removeFromParent() {
	if (_parent) {
		_parent->removeSubview(this);
	}
}
void View::removeSubviewsNotInVisibleArea() {
    for (int i=0 ; i<_subviews.size() ; i++) {
        auto it = _subviews.at(i);
        View* subview = it;
        if (subview->_frame.bottom() < _contentOffset.y
            || subview->_frame.top() >= _frame.size.height+_contentOffset.y) {
            i--;
            removeSubview(subview);
        }
    }
}


void View::setBackground(RenderOp* drawableOp) {
    setBackground(drawableOp, {0,0});
}

void View::setBackground(RenderOp* drawableOp, STATESET stateset) {
    RenderOp* replacedOp = NULL;
    for (vector<pair<ObjPtr<RenderOp>,STATESET>>::iterator i = _backgroundOps.begin() ; i!=_backgroundOps.end() ; i++) {
        if (i->second.mask == stateset.mask && i->second.state == stateset.state) {
            replacedOp = i->first;
            i->first = drawableOp;
            break;
        }
    }
    if (!replacedOp) {
        _backgroundOps.push_back(make_pair(drawableOp, stateset));
    }
    updateBackgroundOp();
}
void View::updateBackgroundOp() {
    RenderOp* selectedOp = NULL;
    int bestpop = -1;
    for (auto i : _backgroundOps) {
        if ((_state & i.second.mask) == i.second.state) {
            int thispop = __builtin_popcount(i.second.mask);
            if (thispop > bestpop) {
                bestpop = thispop;
                selectedOp = i.first;
            }
        }
    }
    if (selectedOp != _currentBackgroundOp) {
        if (_currentBackgroundOp != NULL) {
            removeRenderOp(_currentBackgroundOp);
        }
        _currentBackgroundOp = selectedOp;
        if (_currentBackgroundOp) {
            _currentBackgroundOp->setRect(getBounds());
            addRenderOp(_currentBackgroundOp, true);
        }
    }

}
void View::setBackgroundColour(COLOUR colour) {
    setBackground(new ColorRectFillRenderOp(this, getBounds(), colour));
}


bool View::isPressed() {
    return (_state & STATE_PRESSED)!=0;
}
void View::setPressed(bool isPressed) {
    setState({STATE_PRESSED, (STATE)(isPressed?STATE_PRESSED:0)});
}
void View::setEnabled(bool isEnabled) {
    setState({STATE_DISABLED, (STATE)(isEnabled?0:STATE_DISABLED)});
}

void View::setState(STATESET stateset) {
    uint16_t oldstate = _state & stateset.mask;
    uint16_t newstate = (_state & ~stateset.mask) | stateset.state;
    if (oldstate != newstate) {
        _state = newstate;
        onStateChanged({(STATE)(oldstate^newstate), newstate});
        setNeedsFullRedraw();
    }
}
void View::onStateChanged(STATESET changedStates) {
    updateBackgroundOp();
}

bool View::isTouchable() {
	return _touchEnabled
		&& _visibility==VISIBILITY_VISIBLE
		&& !(_state&STATE_DISABLED);
}

void View::addRenderOp(RenderOp* renderOp) {
    addRenderOp(renderOp, false);
}
void View::addRenderOp(RenderOp* renderOp, bool atFront) {
    renderOp->_viewListIterator = _renderList.insert(atFront ? _renderList.begin() : _renderList.end(), renderOp);
    if (_surface) {
        _surface->addRenderOp(renderOp);
    }
}
void View::removeRenderOp(RenderOp* renderOp) {
     if (_surface) {
         _surface->removeRenderOp(renderOp);
     }
     _renderList.erase(renderOp->_viewListIterator);
}


RECT View::getBounds() {
	return RECT_Make(0,0,_frame.size.width,_frame.size.height);
}
RECT View::getBoundsWithPadding() {
    return RECT_Make(_padding.left,_padding.top, _frame.size.width-(_padding.left+_padding.right), _frame.size.height - (_padding.top+_padding.bottom));
}

void View::setScrollInsets(EDGEINSETS scrollInsets) {
	_scrollInsets = scrollInsets;
	setNeedsLayout();
}


void View::setContentOffset(POINT contentOffset) {
	if (!contentOffset.equals(_contentOffset)) {
		_contentOffset = contentOffset;
        updateScrollbars();
	}
    if (_window) {
        _window->requestRedraw();
    }
}



void View::scrollStartFadeAnim(float targetAlpha) {
    if (!_scrollFadeAnim) {
        _scrollFadeAnim = new DelegateAnimation();
        _scrollFadeAnim->_delegate = [=](float val) {
            _scrollAlpha = val;
            if (_scrollbarVert) {
                _scrollbarVert->_renderOp->setAlpha(val);
                invalidateRect(_scrollbarVert->_renderOp->_rect);
            }
            if (_scrollbarHorz) {
                _scrollbarHorz->_renderOp->setAlpha(val);
                invalidateRect(_scrollbarHorz->_renderOp->_rect);
            }
        };
    }
    _scrollFadeAnim->stop();
    _scrollFadeAnim->_fromVal = _scrollAlpha;
    _scrollFadeAnim->_toVal = targetAlpha;
    if (_window) {
        _scrollFadeAnim->start(_window, 300);
    }
}




void View::updateScrollbars() {
    //if (!_window->_viewLayoutValid) {
    //    return;
    //}
    float visibleContentHeight = _contentSize.height - (_scrollInsets.top+_scrollInsets.bottom);
    float visibleContentWidth = _contentSize.width - (_scrollInsets.left+_scrollInsets.right);
    bool createdBar = false;
	if (visibleContentHeight>0 && _frame.size.height < visibleContentHeight) {
        if (!_scrollbarVert) {
            _scrollbarVert = new Scrollbar(this);
            createdBar = true;
        }
        _scrollbarVert->updateRect();
    } else {
        if (_scrollbarVert) {
            removeRenderOp(_scrollbarVert->_renderOp);
            _scrollbarVert = NULL;
        }
    }
    if (visibleContentWidth>0 && _frame.size.width < (int)visibleContentWidth) {
        if (!_scrollbarHorz) {
            _scrollbarHorz = new Scrollbar(this);
            createdBar = true;
        }
        _scrollbarHorz->updateRect();
    } else {
        if (_scrollbarHorz) {
            removeRenderOp(_scrollbarHorz->_renderOp);
            _scrollbarHorz = NULL;
        }
    }

    // If scrollbar created, autofade the scrollbars in
    if (_scrollbarHorz || _scrollbarVert) {
        if (_scrollAlpha<1 && (!_scrollFadeAnim || _scrollFadeAnim->_toVal != 1.0f)) {
            scrollStartFadeAnim(1.0f);
        }
    }

    // Schedule scrollbars to autofade
    if (_scrollbarVert || _scrollbarHorz) {
        if (_scrollFadeTimer) {
            _scrollFadeTimer->stop();
        }
        _scrollFadeTimer = Timer::start([=]() {
            scrollStartFadeAnim(0.0f);
        }, 500, false);
    }
}

View* View::subviewContainingPoint(POINT pt) {
	for (long i=_subviews.size()-1 ; i>=0 ; i--) {
		View* subview = _subviews.at(i);
		if (subview->_visibility == VISIBILITY_VISIBLE) {
			if (RECT_contains(subview->_frame, pt)) {
				return subview;
			}
		}
	}
	return NULL;
}

int View::indexOfSubviewContainingPoint(POINT pt) {
	for (long i=_subviews.size()-1 ; i>=0 ; i--) {
		View* subview = _subviews.at(i);
		if (subview->_visibility == VISIBILITY_VISIBLE) {
			if (RECT_contains(subview->_frame, pt)) {
				return (int)i;
			}
		}
	}
	return -1;
}

View* View::hitTest(POINT pt, POINT* ptRel) {
	
	// Find the leaf view that corresponds to the given point
	if (_visibility==VISIBILITY_VISIBLE && RECT_contains(_frame, pt)) {
		POINT ptClient = pt;
		ptClient.x -= _frame.origin.x;
		ptClient.y -= _frame.origin.y;
		for (long i=_subviews.size()-1 ; i>=0 ; i--) {
			View* subview = _subviews.at(i);
			View* hitTestSubview = subview->hitTest(ptClient, ptRel);
			if (hitTestSubview) {
				return hitTestSubview;
			}
		}
		if (ptRel) {
			*ptRel = ptClient;
		}
		return this;
	}
	return NULL;
}

/**
dispatches a touch event to a view.
*/
View* View::dispatchTouchEvent(int eventType, int eventSource, long time, POINT pt) {
	if (_state & STATE_DISABLED) {
        return NULL;
    }

	// Find the most distant leaf view containing the point
	POINT ptRel;
	View* view = hitTest(pt, &ptRel);
	
	// Offer the event to the leaf view and if it doesn't handle it pass
	// it up the view tree until a view handles it.
	while (view) {
		if (view->onTouchEvent(eventType, eventSource, ptRel)) {
			return view;
		}
		ptRel.x += view->_frame.origin.x;
		ptRel.y += view->_frame.origin.y;
		view = view->_parent;
	}

	return onTouchEvent(eventType, eventSource, pt) ? this : NULL;
}


bool View::onTouchEvent(int eventType, int eventSource, POINT pt) {

	// todo: state changes

    //if (!touchable) {
    //    return false;
    //}
    
    if (eventType == TOUCH_EVENT_DOWN) {
        _ptDrag = pt;
        _isDragging = false;
        if (_scrollbarVert) {
            _scrollbarVert->mFinished = true;
        }
    }
    if (eventType == TOUCH_EVENT_MOVE) {
        if (_scrollbarVert) {
            float dy = pt.y - _ptDrag.y;
            setContentOffset(POINT_Make(_contentOffset.x, _contentOffset.y - dy));
        }
        _ptDrag = pt;
    }
    if (eventType == TOUCH_EVENT_DRAG) {
        _isDragging = true;
    }
    if (eventType == TOUCH_EVENT_FLING) {
        //oakLog("fling! %f", -pt.y);
        if (_scrollbarVert) {
            _scrollbarVert->fling(_contentOffset.y, -pt.y, 0, _contentSize.height-_frame.size.height);
            setNeedsFullRedraw();
        }
    }
    
    
    
	if (onTouchEventDelegate) {
		if (onTouchEventDelegate(this, eventType, eventSource, pt)) {
			return true;
		}
	}
	return false;
}

IKeyboardInputHandler* View::getKeyboardInputHandler() {
    return NULL;
}
bool View::becomeFirstResponder() {
    if (_window) {
        return _window->setFirstResponder(this);
    }
    return false;
}
bool View::isFirstResponder() {
    if (_window) {
        return _window->_firstResponder == this;
    }
    return false;
}
void View::resignFirstResponder() {
    if (isFirstResponder()) {
        _window->setFirstResponder(NULL);
    }
}


POINT View::mapPointToWindow(POINT pt) {
	if (_window) {
		View* view = this;
		while (view) {
			pt.x += view->_frame.origin.x;
			pt.y += view->_frame.origin.y;
			view = view->_parent;
		}
	}
	return pt;
}

void View::setAlpha(float alpha) {
    if (alpha != _alpha) {
        _alpha = alpha;
        updateEffectiveAlpha();
    }
}

void View::setTintColour(COLOUR tintColour) {
    if (tintColour != _tintColour) {
        _tintColour = tintColour;
        updateEffectiveTint();
    }
}

void View::updateEffectiveAlpha() {
    float parentAlpha = _parent ? _parent->_effectiveAlpha : 1.0f;
    _effectiveAlpha = parentAlpha * _alpha;
    for (auto it= _renderList.begin() ; it!=_renderList.end() ; it++) {
        (*it)->setAlpha(_effectiveAlpha);
    }
    for (auto it = _subviews.begin() ; it!=_subviews.end() ; it++) {
        (*it)->updateEffectiveAlpha();
    }
}

void View::updateEffectiveTint() {
    COLOUR parentVal = _parent ? _parent->_effectiveTintColour : 0;
    COLOUR newTint = _tintColour ? _tintColour : parentVal;
    if (newTint != _effectiveTintColour) {
        _effectiveTintColour = newTint;
        onEffectiveTintColourChanged();
    }
    for (auto it = _subviews.begin() ; it!=_subviews.end() ; it++) {
        (*it)->updateEffectiveTint();
    }
}

void View::onEffectiveTintColourChanged() {
    // no-op. ImageView and Label etc might like to change colour though
}



void View::animateAlpha(float target, float duration) {
	if (_alphaAnim) {
		_alphaAnim->stop();
		_alphaAnim = NULL;
	}
	if (duration <= 0) {
		_alpha = target;
	} else {
		if (_window) {
			_alphaAnim = new AlphaAnimation(this, target);
            _alphaAnim->_onFinished = [=](Animation* animation) {
                _alphaAnim = NULL;
            };
			_alphaAnim->start(_window, duration);
		}
	}
}


void View::setAnimTranslate(POINT translation) {
    if (translation.x==0 && translation.y==0) {
        if (_matrix) {
            delete _matrix;
            _matrix = NULL;
        }
    } else {
        if (!_matrix) {
            _matrix = new Matrix4();
        }
        _matrix->identity();
        _matrix->translate(translation.x, translation.y, 0);
    }
    if (_ownsPrivateSurface) {
        setNeedsFullRedraw();
    }
    if (!_window->_redrawNeeded) {
        _window->requestRedraw();
    }
}


#ifdef DEBUG

string View::debugViewType() {
    return "View";
}
string View::debugDescription() {
    char ach[256];
    sprintf(ach, "%lX:%s (p:%lX n:%lX)", (long)this, debugViewType().data(), (long)_previousView._obj, (long)_nextView._obj);
    return string(ach);
}
void View::debugDumpTree(int depth) {
    

    string line;
    for (int i=0 ; i<depth ; i++) line.push_back('-');
    line.append(debugDescription());
    oakLog(line.data());

    for (auto it=_renderList.begin() ; it!=_renderList.end() ; it++) {
        RenderOp* op = *it;
        if (op->_batch) {
            assert(op->_batch->_surface == _surface);
            //assert(op->_batch->_renderBatchList->_list.)
        }
    }

    for (int i=0 ; i<_subviews.size() ; i++) {
        _subviews.at(i)->debugDumpTree(depth+2);
    }
}

#endif

