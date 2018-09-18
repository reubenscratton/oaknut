//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



DECLARE_DYNCREATE(View);

View::View() : _alpha(1.0f),
                _alignspecHorz(ALIGNSPEC::None()),
                _alignspecVert(ALIGNSPEC::None()),
                _widthMeasureSpec(MEASURESPEC::None()),
                _heightMeasureSpec(MEASURESPEC::None())
{
    _visibility = Visible;
}

View::~View() {
//	app.log("~View()");
    if (_statemapStyleValues) {
        delete _statemapStyleValues;
    }
}

void View::applyStyle(const string& style) {
    auto value = app.getStyleValue(style);
    if (value) {
        applyStyleValues(*value);
    }
}

void View::applyStyleValues(const StyleValue& value) {
    
    auto& compound = value.compoundVal();
    // Ensure the 'style' attribute, if present, gets processed first, because the others may override it
    for (auto& field : compound) {
        if (field.first == "style") {
            applyStyleValues(field.second);
            break;
        }
    }

    for (auto& field : compound) {
        if (field.first == "style" || field.first == "class") {
            continue;
        }
        const StyleValue& val = field.second;
        if (!applyStyleValue(field.first, &val)) {
            if (_parent && !_parent->applyStyleValueFromChild(field.first, &val, this)) {
                app.warn("Ignored unknown attribute '%s'", field.first.data());
            }
        }
    }
}



bool View::applyStyleValue(const string& name, const StyleValue* value) {
    if (name == "id") {
        _id = value->stringVal();
        return true;
    } else if (name == "height") {
        _heightMeasureSpec = MEASURESPEC(value);
        return true;
    } else if (name == "width") {
        _widthMeasureSpec = MEASURESPEC(value);
        return true;
    } else if (name == "alignX") {
        _alignspecHorz = ALIGNSPEC(value, this);
        return true;
    } else if (name == "alignY") {
        _alignspecVert = ALIGNSPEC(value, this);
        return true;
    } else if (name == "background") {
        if (handleStatemapDeclaration(name, value)) {
            return true;
        }
        setBackground(processDrawable(value));
        return true;
    } else if (name == "enabled") {
        setEnabled(value->boolVal());
        return true;
    }
    if (name=="gravityX") {
        uint8_t horz = 0;
        string str = value->stringVal();
        if (str == "left") {
            horz = GRAVITY_LEFT;
        } else if (str == "right") {
            horz = GRAVITY_RIGHT;
        } else if (str == "center" || str == "centre") {
            horz = GRAVITY_CENTER;
        } else {
            assert(0);
        }
        setGravity({horz, _gravity.vert});
        return true;
    }
    if (name=="gravityY") {
        uint8_t vert = 0;
        string str = value->stringVal();
        if (str == "top") {
            vert = GRAVITY_TOP;
        } else if (str == "bottom") {
            vert = GRAVITY_BOTTOM;
        } else if (str == "center" || str == "centre") {
            vert = GRAVITY_CENTER;
        } else {
            assert(0);
        }
        setGravity({_gravity.horz, vert});
        return true;
    }
    if (name == "padding") {
        EDGEINSETS padding = value->edgeInsetsVal();
        setPadding(padding);
        return true;
    } else if (name == "paddingHorz") {
        float pad = value->floatVal();
        setPadding(EDGEINSETS(pad,_padding.top,pad,_padding.bottom));
        return true;
    } else if (name == "paddingVert") {
        float pad = value->floatVal();
        setPadding(EDGEINSETS(_padding.left,pad,_padding.right,pad));
        return true;
    } else if (name == "tint") {
        setTintColor(value->colorVal());
        return true;
    }
    return false;
}

bool View::applyStyleValueFromChild(const string& name, const StyleValue* value, View* subview) {
    return false;
}

void processFill(RectRenderOp* op, const StyleValue& value) {
    if (value.type == StyleValue::Type::Compound) {
        assert(0); // todo: support compound values
        return;
    }
    op->setFillColor(value.colorVal());
}

RenderOp* View::processDrawable(const StyleValue* value) {
    if (value->isEmpty()) {
        return NULL;
    }

    /*if (value->type == StyleValue::Type::String) {
        // todo: handle image
        assert(0);
    }*/

    RectRenderOp* op = new RectRenderOp();
    
    
    if (value->type == StyleValue::Type::Compound) {
        auto& compoundVal = value->compoundVal();
        for (auto& it : compoundVal) {
            auto& subval = it.second;
 
            if (it.first == "fill") {
                processFill(op, subval);
            } else if (it.first == "stroke") {
                op->setStrokeColor(subval.colorVal());
            } else if (it.first == "stroke-width") {
                op->setStrokeWidth(subval.floatVal());
            } else if (it.first == "corner-radius" || it.first == "corner-radii") {
                op->setCornerRadii(subval.cornerRadiiVal());
            } else if (it.first == "inset") {
                EDGEINSETS insets = subval.edgeInsetsVal();
                op->setInset(insets);
            } else {
                app.warn("Unknown drawable attribute: %s", it.first.data());
            }
        }
    } else {
        op->setFillColor(value->colorVal());
    }
    return op;
}


bool View::handleStatemapDeclaration(const string& name, const StyleValue* value) {
    if (value->type != StyleValue::Compound) {
        return false;
    }
    
    // Inspect map keys to see if its a statemap or not
    auto& compoundVal = value->compoundVal();
    int c = 0;
    for (auto& k : compoundVal) {
        if (k.first == "enabled") c++;
        else if (k.first == "disabled") c++;
        else if (k.first == "pressed") c++;
        else if (k.first == "unpressed") c++;
        else if (k.first == "selected") c++;
        else if (k.first == "checked") c++;
        else if (k.first == "unchecked") c++;
        else if (k.first == "focused") c++;
    }
    if (!c) {
        return false;
    }
    
    // Its a statemap.
    if (_statemapStyleValues) {
        _statemapStyleValues->erase(name);
    }
    if (!_statemapStyleValues) {
        _statemapStyleValues = new map<string, StyleValue>();
    }
    auto newval = make_pair(name, *value);
    _statemapStyleValues->insert(newval);
    
    // Choose initial value immediately
    applyStatemapStyleValue(name, value);
    return true;
}

void View::applyStatemapStyleValue(const string& name, const StyleValue* statemap) {
    // Walk the map and find the values which apply. Longest (i.e. most state bits) matching value wins.
    list<StyleValue*> matchingValues;
    const StyleValue* bestMatchValue = NULL;
    int best_pri=0;
    for (auto& it : statemap->compoundVal()) {
        int pri=1;
        
        // Convert the map entry to a stateset
        STATESET stateset = {0,0};
        if (it.first == "enabled") stateset.setBits(STATE_DISABLED, 0);
        else if (it.first == "disabled") {stateset.setBits(STATE_DISABLED, STATE_DISABLED); pri=2;}
        else if (it.first == "unpressed") stateset.setBits(STATE_PRESSED, 0);
        else if (it.first == "focused") stateset.setBits(STATE_FOCUSED, STATE_FOCUSED);
        else if (it.first == "unchecked") stateset.setBits(STATE_CHECKED, 0);
        else if (it.first == "pressed") stateset.setBits(STATE_PRESSED, STATE_PRESSED);
        else if (it.first == "selected") stateset.setBits(STATE_SELECTED, STATE_SELECTED);
        else if (it.first == "checked") stateset.setBits(STATE_CHECKED, STATE_CHECKED);
        
        // If the stateset matches
        if ((_state & stateset.mask) == stateset.state && pri>best_pri) {
            bestMatchValue = &it.second;
            best_pri = pri;
        }
    }
    
    if (bestMatchValue) {
        applyStyleValue(name, bestMatchValue);
    } else {
        StyleValue nullVal;
        applyStyleValue(name, &nullVal);
    }
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
			_parent->invalidateRect(_rect);
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
			prect.origin.x += _rect.origin.x;
			prect.origin.y += _rect.origin.y;
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


float View::getWidth() const {
    return _rect.size.width;
}
float View::getHeight() const {
    return _rect.size.height;
}

RECT View::getRect() const {
    return _rect;
}

void View::setRectOrigin(const POINT& origin) {
    POINT d;
    d.x = origin.x - _rect.origin.x;
    d.y = origin.y - _rect.origin.y;
    if (d.x || d.y) {
        _rect.origin = origin;
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

void View::setRectSize(const SIZE& size) {
    SIZE d;
    d.width = size.width - _rect.size.width;
    d.height = size.height - _rect.size.height;
    if (d.width || d.height) {
        _rect.size = size;
        updateBackgroundRect();
        setNeedsFullRedraw();
    }
}

void View::updateBackgroundRect() {
    if (_backgroundOp) {
        RECT rect = getOwnRect();
        rect.origin.x += _contentOffset.x;
        rect.origin.y += _contentOffset.y;
        _backgroundOp->_inset.applyToRect(rect);
        _backgroundOp->setRect(rect);
    }
}

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
	if (_window) {
		_window->setNeedsLayout();
	}
}

void View::invalidateContentSize() {
	_contentSizeValid = false;
    if (_widthMeasureSpec.refType==MEASURESPEC::RefTypeContent || _heightMeasureSpec.refType==MEASURESPEC::RefTypeContent) {
		setNeedsLayout();
    } else {
        setNeedsFullRedraw();
    }
}

void View::updateContentSize(float parentWidth, float parentHeight) {
	// no-op
}

bool View::getClipsContent() const {
    return _clipsContent;
}
void View::setClipsContent(bool clipsContent) {
    if (clipsContent != _clipsContent) {
        _clipsContent = clipsContent;
        setNeedsFullRedraw();
    }
}


void View::setGravity(GRAVITY gravity) {
	_gravity = gravity;
	setNeedsLayout();
}

void View::setVisibility(Visibility visibility) {
    if (_visibility != visibility) {
        if ((_visibility==Gone && visibility!=Gone) ||
            (_visibility!=Gone && visibility==Gone)) {
            setNeedsLayout();
        }
        _visibility = visibility;
        if (visibility == Visible) {
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
	if (MEASURESPEC::RefTypeContent == _widthMeasureSpec.refType) {
        _rect.size.width = _contentSize.width + _padding.left + _padding.right;
	} else if (MEASURESPEC::RefTypeAbs == _widthMeasureSpec.refType) {
        _rect.size.width = _widthMeasureSpec.abs;
    } else if (MEASURESPEC::RefTypeView == _widthMeasureSpec.refType) {
        float refWidth = (_widthMeasureSpec.refView == NULL) ? parentWidth : _widthMeasureSpec.refView->_rect.size.width;
        _rect.size.width = refWidth * _widthMeasureSpec.refSizeMultiplier;
		_rect.size.width += _widthMeasureSpec.abs;
    }

    
    // Height
	if (MEASURESPEC::RefTypeContent == _heightMeasureSpec.refType) {
		_rect.size.height = _contentSize.height + _padding.top + _padding.bottom;
	} else if (MEASURESPEC::RefTypeAbs == _heightMeasureSpec.refType) {
		_rect.size.height = _heightMeasureSpec.abs;
	} else if (MEASURESPEC::RefTypeView == _heightMeasureSpec.refType) {
		float refHeight = (_heightMeasureSpec.refView == NULL) ? parentHeight : _heightMeasureSpec.refView->_rect.size.height;
		_rect.size.height = refHeight * _heightMeasureSpec.refSizeMultiplier;
		_rect.size.height += _heightMeasureSpec.abs;
	}
	
    // Aspect
    if (MEASURESPEC::RefTypeAspect == _widthMeasureSpec.refType) {
        _rect.size.width = _rect.size.height * _widthMeasureSpec.refSizeMultiplier;
    } else if (MEASURESPEC::RefTypeAspect == _heightMeasureSpec.refType) {
        _rect.size.height = _rect.size.width * _heightMeasureSpec.refSizeMultiplier;
    }
	
	
	    
    // At this point we've done relative-to-parent sizing. But if a size value depends on
    // wrapping subview size(s) then pass down the parent size.
	bool wrapWidth = _contentSize.width<=0 && _widthMeasureSpec.refType == MEASURESPEC::RefTypeContent;
	bool wrapHeight =  _contentSize.height<=0 && _heightMeasureSpec.refType == MEASURESPEC::RefTypeContent;
	float widthForSubviewMeasure = wrapWidth ? parentWidth : _rect.size.width;
	float heightForSubviewMeasure = wrapHeight ? parentHeight : _rect.size.height;

	// Adjust for padding
	widthForSubviewMeasure -= (_padding.left+_padding.right);
	heightForSubviewMeasure -= (_padding.top+_padding.bottom);

	// Measure subviews and track largest sizes
    float largestChildWidth = 0;
    float largestChildHeight = 0;
    for (int i=0 ; i<_subviews.size() ; i++) {
        View* view = _subviews.at(i);
        view->measure(widthForSubviewMeasure, heightForSubviewMeasure);
        largestChildWidth = fmaxf(largestChildWidth, view->_rect.size.width);
        largestChildHeight = fmaxf(largestChildHeight, view->_rect.size.height);
    }
    
    // wrap_content
    if (wrapWidth) {
        _rect.size.width = _padding.left + largestChildWidth + _padding.right;
    }
    if (wrapHeight) {
        _rect.size.height = _padding.bottom + largestChildHeight + _padding.top;
    }
	
	// Align view sizes to pixel grid
	_rect.size.width = floorf(_rect.size.width);
	_rect.size.height = floorf(_rect.size.height);

}


float View::getAlignspecVal(const ALIGNSPEC& spec, bool isVertical) {

    View* anchor = (spec.anchor?spec.anchor:_parent);
    float anchorVal = 0;
    RECT& refRect = anchor ? anchor->_rect : _window->_surfaceRect;
    float anchorSize = isVertical ? refRect.size.height : refRect.size.width;
    float ownSize = isVertical ? _rect.size.height : _rect.size.width;
    if (anchor) {
        if (anchor == _parent) {
            anchorVal = isVertical ? _parent->_padding.top : _parent->_padding.left;
            anchorSize -= isVertical ? (anchor->_padding.top+anchor->_padding.bottom):  (anchor->_padding.left+anchor->_padding.right);
        } else {
            if (anchor->_parent != _parent) {
                app.warn("View layout is not clever enough for non-sibling anchors");
            }
            anchorVal = isVertical ? anchor->_rect.origin.y : anchor->_rect.origin.x;
        }
    }
    float val = 0;
    if (anchor) {
        val = anchorVal + (spec.multiplierAnchor * anchorSize)
                         + (spec.multiplierSelf * ownSize)
                         + spec.margin;
        val = floorf(val);
    } else {
        val = isVertical ? _rect.origin.y : _rect.origin.x;
    }
    return val;

}

void View::layout() {
    if (!_window) {
        return;
    }
    POINT pt;
    pt.x = getAlignspecVal(_alignspecHorz, false);
    pt.y = getAlignspecVal(_alignspecVert, true);
    setRectOrigin(pt);

    /*if (_backgroundOp) {
        _backgroundOp->setRect(RECT(0, 0, _rect.size.width, _rect.size.height));
    }*/
    updateBackgroundRect();

    for (int i=0 ; i<_subviews.size() ; i++) {
        View* view = _subviews.at(i);
        view->layout();
    }
	
    updateScrollbarVisibility();
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
    if (_visibility != Visible) {
        return;
    }
	_window = window;
    if (_parent) {
        _surfaceOrigin = {_parent->_surfaceOrigin.x + _rect.origin.x, _parent->_surfaceOrigin.y + _rect.origin.y};
        _contentOffsetAccum = _contentOffset + _parent->_contentOffsetAccum;
    } else {
        _surfaceOrigin = _rect.origin;
    }

    updatePrivateSurface(false);
    updateEffectiveAlpha();
    updateEffectiveTint();
    
    if (_surface) {
        _surface->attachViewOps(this);
    }
    
	for (auto it = _subviews.begin(); it!=_subviews.end() ; it++) {
		ObjPtr<View> subview = *it;
        if (subview->_visibility == Visible) {
            subview->attachToWindow(window);
        }
	}
}


void View::detachFromWindow() {
	if (!_window) {
        return;
    }
    _scrollHorz.detach();
    _scrollVert.detach();
    _window->detachView(this);
    
    
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
        _surface->cleanup();
        _surface->_op = NULL;
    }

    _surface = nullptr;
    _window = nullptr;
    _contentOffsetAccum = _contentOffset;
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

Window* View::getWindow() const {
    return _window;
}

View* View::getParent() const {
    return _parent;
}

void View::addSubview(View* subview) {
	insertSubview(subview, (int)_subviews.size());
}
void View::insertSubview(View* subview, int index) {
    
    // If adding to end of list, make sure we don't draw on top of scrollbars view
    if (_scrollbarsView && (index >= _subviews.size())) {
        index = (int)_subviews.size() - 1;
    }
    
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
    //assert(!rightmost->_nextView);
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
    if (!_surface) {
        assert(!_surface->_renderInProgress); // naughty!
    }
	if (_parent) {
		_parent->removeSubview(this);
	}
}
void View::removeSubviewsNotInVisibleArea() {
    for (int i=0 ; i<_subviews.size() ; i++) {
        auto it = _subviews.at(i);
        View* subview = it;
        if (subview == _scrollbarsView) continue;
        if (subview->_rect.bottom() < _contentOffset.y
            || subview->_rect.top() >= _rect.size.height+_contentOffset.y) {
            i--;
            removeSubview(subview);
        }
    }
}


void View::setBackground(RenderOp* op) {
    if (_backgroundOp != op) {
        if (_backgroundOp != NULL) {
            removeRenderOp(_backgroundOp);
        }
        _backgroundOp = op;
        if (_backgroundOp) {
            updateBackgroundRect();
            addRenderOp(_backgroundOp, true);
        }
    }
}

void View::setBackgroundColor(COLOR color) {
    auto op = new RectRenderOp();
    op->setColor(color);
    setBackground(op);
}


bool View::isPressed() const {
    return (_state & STATE_PRESSED)!=0;
}
void View::setPressed(bool isPressed) {
    setState(STATE_PRESSED, (STATE)(isPressed?STATE_PRESSED:0));
}
bool View::isEnabled() const {
    return (_state & STATE_DISABLED)==0;
}
void View::setEnabled(bool isEnabled) {
    setState(STATE_DISABLED, (STATE)(isEnabled?0:STATE_DISABLED));
}
bool View::isChecked() const {
    return (_state & STATE_CHECKED)!=0;
}
void View::setChecked(bool checked) {
    setState(STATE_CHECKED, (STATE)(checked?STATE_CHECKED:0));
}


void View::setState(STATE mask, STATE value) {
    uint16_t oldstate = _state;
    uint16_t newstate = (_state & ~mask) | value;
    if (oldstate != newstate) {
        _state = newstate;
        onStateChanged({(STATE)(oldstate^newstate), newstate});
        setNeedsFullRedraw();
    }
}
void View::onStateChanged(STATESET changedStates) {
    if (_statemapStyleValues) {
        for (auto& it : *_statemapStyleValues) {
            applyStatemapStyleValue(it.first, &it.second);
        }
    }
}

void View::addScrollbarOp(RenderOp* renderOp) {
    bool layoutValid = _window->_viewLayoutValid; // we preserve this flag to avoid scrollbars triggering layout
    if (!_scrollbarsView) {
        ScrollbarsView* scrollbarsView = new ScrollbarsView();
        //scrollbarsView->setMeasureSpecs(MEASURESPEC::FillParent(), MEASURESPEC::FillParent());
        //scrollbarsView->setAlignSpecs(ALIGNSPEC::Top(), ALIGNSPEC::Left());
        addSubview(scrollbarsView);
        scrollbarsView->_rect = getOwnRect();
        _scrollbarsView = scrollbarsView;
    }
    //renderOp->_view = _scrollbarsView;
    _scrollbarsView->addRenderOp(renderOp);
    _window->_viewLayoutValid = layoutValid;
}

void View::removeScrollbarOp(RenderOp* renderOp) {
    bool layoutValid = _window->_viewLayoutValid; // we preserve this flag to avoid scrollbars triggering layout
    assert(_scrollbarsView);
    _scrollbarsView->removeRenderOp(renderOp);
    if (_scrollbarsView->_renderList.size() == 0) {
        removeSubview(_scrollbarsView);
        _scrollbarsView = NULL;
    }
    _window->_viewLayoutValid = layoutValid;
}


void View::addRenderOp(RenderOp* renderOp) {
    addRenderOp(renderOp, false);
}
void View::addRenderOp(RenderOp* renderOp, bool atFront) {
    if (renderOp->_view) {
        assert(renderOp->_view == this);
        return;
    }
    assert(!renderOp->_view);
    renderOp->_view = this;
    renderOp->_viewListIterator = _renderList.insert(atFront ? _renderList.begin() : _renderList.end(), renderOp);
    if (_surface) {
        _surface->addRenderOp(renderOp);
    }
}
void View::removeRenderOp(RenderOp* renderOp) {
    assert(renderOp->_view);
    if (renderOp->_viewListIterator != _renderList.end()) {
         if (_surface) {
             _surface->removeRenderOp(renderOp);
         }
         _renderList.erase(renderOp->_viewListIterator);
        renderOp->_viewListIterator = _renderList.end();
    }
    renderOp->_view = NULL;
}


RECT View::getOwnRect() {
	return RECT(0,0,_rect.size.width,_rect.size.height);
}
RECT View::getOwnRectPadded() {
    return RECT(_padding.left,_padding.top, _rect.size.width-(_padding.left+_padding.right), _rect.size.height - (_padding.top+_padding.bottom));
}

void View::setScrollInsets(EDGEINSETS scrollInsets) {
	_scrollInsets = scrollInsets;
	setNeedsLayout();
}

SIZE View::getContentSize() const {
    return _contentSize;
}

POINT View::getContentOffset() const {
    return _contentOffset;
}

void View::setContentOffset(POINT contentOffset) {
    POINT d = _contentOffset - contentOffset;
	if (!d.isZero()) {
		_contentOffset = contentOffset;
        adjustContentOffsetAccum(d);
        updateScrollbarVisibility();
        updateBackgroundRect();
	}
    if (_window) {
        _window->requestRedraw();
    }
}
void View::adjustContentOffsetAccum(const POINT& d) {
    _contentOffsetAccum += d;
    for (auto subview : _subviews) {
        subview->adjustContentOffsetAccum(d);
    }
}

void View::scrollBy(POINT scrollAmount) {
    POINT origContentOffset = _contentOffset;
    Animation::start(this, 350, [=](float a) { // todo: style
        POINT d = {a*scrollAmount.x, a*scrollAmount.y};
        POINT newOffset = origContentOffset + d;
        setContentOffset(newOffset);
    }, linear);

}


bool View::canScrollVertically() {
    return _scrollVert.canScroll(this, true);
}
bool View::canScrollHorizontally() {
    return _scrollHorz.canScroll(this, false);
}

void View::updateScrollbarVisibility() {
    _scrollVert.updateVisibility(this, true);
    _scrollHorz.updateVisibility(this, false);
}

void View::updateScrollOffsets() {
    if (_scrollVert._fling || _scrollHorz._fling) {
        POINT newContentOffset = _contentOffset;
        if (_scrollHorz._fling) {
            newContentOffset.x = _scrollHorz.flingUpdate();
        }
        if (_scrollVert._fling) {
            newContentOffset.y = _scrollVert.flingUpdate();
        }
        setContentOffset(newContentOffset);
    }
}

View* View::subviewContainingPoint(POINT pt) {
	for (long i=_subviews.size()-1 ; i>=0 ; i--) {
		View* subview = _subviews.at(i);
		if (subview->_visibility == Visible) {
			if (subview->_rect.contains(pt)) {
				return subview;
			}
		}
	}
	return NULL;
}

int View::indexOfSubviewContainingPoint(POINT pt) {
	for (long i=_subviews.size()-1 ; i>=0 ; i--) {
		View* subview = _subviews.at(i);
		if (subview->_visibility == Visible) {
			if (subview->_rect.contains(pt)) {
				return (int)i;
			}
		}
	}
	return -1;
}


View* View::hitTest(POINT& pt) {
	
	// Find the leaf view that corresponds to the given point
	if (_visibility==Visible && _rect.contains(pt)) {
        pt += _contentOffset;
        pt -= _rect.origin;
        for (long i=_subviews.size()-(_scrollbarsView?2:1) ; i>=0 ; i--) {
			View* subview = _subviews.at(i);
			auto hitTestSub = subview->hitTest(pt);
			if (hitTestSub) {
				return hitTestSub;
			}
		}
		return this;
	}
	return NULL;
}

//
// dispatches an input event to a view.
//
View* View::dispatchInputEvent(INPUTEVENT* event) {
	if (_state & STATE_DISABLED) {
        return NULL;
    }
    
	// Find the most distant leaf view containing the point
    POINT hitPoint = event->pt;
	View* hitView = hitTest(hitPoint);
	
	// Offer the event to the leaf view and if it doesn't handle it pass
	// it up the view tree until a view handles it.
	while (hitView) {
        event->ptLocal = hitPoint;
		if (hitView->onInputEvent(event)) {
			return hitView;
		}
        hitPoint.x += hitView->_rect.origin.x;
        hitPoint.y += hitView->_rect.origin.y;
		hitView = hitView->_parent;
        if (hitView) {
            hitPoint.x -= hitView->_contentOffset.x;
            hitPoint.y -= hitView->_contentOffset.y;
        }
	}

	return onInputEvent(event) ? this : NULL;
}

void ScrollbarsView::measure(float parentWidth, float parentHeight) {
    _rect = {0,0,parentWidth, parentHeight};
}
void ScrollbarsView::layout() {
}

bool View::onInputEvent(INPUTEVENT* event) {

    bool retVal = false;
    
    if (isEnabled()) {
        if (event->type == INPUT_EVENT_DOWN) {
            setPressed(true);
        }
        if (event->type == INPUT_EVENT_CANCEL || event->type==INPUT_EVENT_UP) {
            setPressed(false);
        }

        if ((onInputEventDelegate || onClickDelegate)) {
            retVal = true;
        }
    }
    
    //app.log("%X y=%f", this, event->pt.y);
    bool scrolled = _scrollVert.handleEvent(this, true, event);
    scrolled |= _scrollHorz.handleEvent(this, false, event);
    
    if (onClickDelegate) {
        retVal = true;
        if (event->type == INPUT_EVENT_TAP) {
            onClickDelegate(this);
        }
    }
	if (onInputEventDelegate) {
        return onInputEventDelegate(this, event);
	}


	return retVal || scrolled;
}

IKeyboardInputHandler* View::getKeyboardInputHandler() {
    return NULL;
}
ITextInputReceiver* View::getTextInputReceiver() {
    return NULL;
}

bool View::setFocused(bool focused) {
    if (focused) {
        if (_window) {
            return _window->setFocusedView(this);
        }
    } else {
        if (isFocused()) {
            _window->setFocusedView(NULL);
            return true;
        }
    }
    return false;
}
bool View::isFocused() {
    if (_window) {
        return _window->_focusedView == this;
    }
    return false;
}


POINT View::mapPointToWindow(POINT pt) {
	if (_window) {
		View* view = this;
		while (view) {
			pt.x += view->_rect.origin.x;
			pt.y += view->_rect.origin.y;
			view = view->_parent;
		}
	}
	return pt;
}

float View::getAlpha() {
    return _alpha;
}

void View::setAlpha(float alpha) {
    if (alpha != _alpha) {
        _alpha = alpha;
        updateEffectiveAlpha();
        setNeedsFullRedraw();
    }
}

COLOR View::getTintColor() {
    return _tintColor;
}

void View::setTintColor(COLOR tintColor) {
    if (tintColor != _tintColor) {
        _tintColor = tintColor;
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
    COLOR parentVal = _parent ? _parent->_effectiveTintColor : 0;
    COLOR newTint = _tintColor ? _tintColor : parentVal;
    if (newTint != _effectiveTintColor) {
        _effectiveTintColor = newTint;
        onEffectiveTintColorChanged();
    }
    for (auto it = _subviews.begin() ; it!=_subviews.end() ; it++) {
        (*it)->updateEffectiveTint();
    }
}

void View::onEffectiveTintColorChanged() {
    // no-op. ImageView and Label etc might like to change color though
}



void View::animateAlpha(float target, float duration) {
	if (duration <= 0) {
		setAlpha(target);
	} else {
		if (_window) {
            _window->startAnimation(new AlphaAnimation(this, target), duration);
		}
	}
}

void View::animateTranslate(POINT translation, float duration) {
    if (duration <= 0) {
        setTranslate(translation);
    } else {
        Animation::start(this, duration, [=](float val) {
            setTranslate({translation.x*val, translation.y*val});
        });
    }
}


void View::setTranslate(POINT translation) {
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
    for (int i=0 ; i<depth ; i++) line.append("-");
    line.append(debugDescription());
    app.log(line.data());

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

