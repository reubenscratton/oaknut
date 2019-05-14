//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



DECLARE_DYNCREATE(View);

View::View() : _alpha(1.0f),
                _alignspecHorz(ALIGNSPEC::Left()),
                _alignspecVert(ALIGNSPEC::Top()),
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

void View::inflate(const string& layoutFile) {
    app.layoutInflateExistingView(this, layoutFile);
}



bool View::applySingleStyle(const string& name, const style& value) {
    // Ignore "class" and "subviews" which are handled by layoutInflate
    if (name=="class" || name=="subviews") {
        return true;
    }
    
    if (name == "id") {
        _id = value.stringVal();
        return true;
    } else if (name == "size") {
        processSizeStyleValue(value, &_widthMeasureSpec, &_heightMeasureSpec);
        return true;
    } else if (name == "height") {
        _heightMeasureSpec = MEASURESPEC::fromStyle(&value.variantVal(), this);
        return true;
    } else if (name == "width") {
        _widthMeasureSpec = MEASURESPEC::fromStyle(&value.variantVal(), this);
        return true;
    } else if (name == "align") {
        processAlignStyleValue(value, &_alignspecHorz, &_alignspecVert);
        return true;
    } else if (name == "alignX") {
        _alignspecHorz = ALIGNSPEC(value.variantVal(), this);
        return true;
    } else if (name == "alignY") {
        _alignspecVert = ALIGNSPEC(value.variantVal(), this);
        return true;
    } else if (name == "background") {
        if (handleStatemapDeclaration(name, value)) {
            return true;
        }
        setBackground(processDrawable(value));
        return true;
    } else if (name == "enabled") {
        setEnabled(value.boolVal());
        return true;
    } else if (name == "visibility") {
        string str = value.stringVal();
        if (str == "visible") {
            setVisibility(Visible);
        } else if (str == "hidden" || str == "invisible") {
            setVisibility(Invisible);
        } else if (str == "gone") {
            setVisibility(Gone);
        } else {
            app.warn("Invalid visibility: '%s'", str.data());
        }
        return true;
    } else if (name=="gravity") {
        processGravityStyleValue(value, true, true);
        return true;
    } else if (name=="gravityX") {
        processGravityStyleValue(value, true, false);
        return true;
    } else if (name=="gravityY") {
        processGravityStyleValue(value, false, true);
        return true;
    } else if (name == "padding") {
        EDGEINSETS padding = value.edgeInsetsVal();
        setPadding(padding);
        return true;
    } else if (name == "paddingX") {
        float pad = value.floatVal();
        setPadding(EDGEINSETS(pad,_padding.top,pad,_padding.bottom));
        return true;
    } else if (name == "paddingY") {
        float pad = value.floatVal();
        setPadding(EDGEINSETS(_padding.left,pad,_padding.right,pad));
        return true;
    } else if (name == "tint") {
        setTintColor(value.colorVal());
        return true;
    }
#if DEBUG
    if (name == "debug") {
        _debugTag = value.stringVal();
        return true;
    }
#endif
    return false;
}

void View::processSizeStyleValue(const style& sizeValue, MEASURESPEC* widthspec, MEASURESPEC* heightspec) {
    if (sizeValue.isArray()) {
        auto arrayVal = sizeValue.arrayVal();
        assert(arrayVal.size()==2);
        *widthspec = MEASURESPEC::fromStyle(&arrayVal[0].variantVal(), this);
        *heightspec = MEASURESPEC::fromStyle(&arrayVal[1].variantVal(), this);
    } else {
        *widthspec = MEASURESPEC::fromStyle(&sizeValue.variantVal(), this);
        *heightspec = MEASURESPEC::fromStyle(&sizeValue.variantVal(), this);
    }
}

void View::processAlignStyleValue(const style& alignValue, ALIGNSPEC* horzspec, ALIGNSPEC* vertspec) {
    if (alignValue.isArray()) {
        auto arrayVal = alignValue.arrayVal();
        assert(arrayVal.size()==2);
        *horzspec = ALIGNSPEC(arrayVal[0].variantVal(), this);
        *vertspec = ALIGNSPEC(arrayVal[1].variantVal(), this);
    } else {
        *horzspec = ALIGNSPEC(alignValue.variantVal(), this);
        *vertspec = ALIGNSPEC(alignValue.variantVal(), this);
    }
}

static uint8_t gravityValueFromString(const string& str) {
    if (str == "left") {
        return GRAVITY_LEFT;
    }
    if (str == "right") {
        return GRAVITY_RIGHT;
    }
    if (str == "center" || str == "centre") {
        return GRAVITY_CENTER;
    }
    if (str == "top") {
        return GRAVITY_TOP;
    }
    if (str == "bottom") {
        return GRAVITY_BOTTOM;
    }
    assert(0);
}

void View::processGravityStyleValue(const style& gravityValue, bool applyToHorz, bool applyToVert) {
    uint8_t horz = _gravity.horz;
    uint8_t vert = _gravity.vert;
    if (gravityValue.isArray()) {
        auto arrayVal = gravityValue.arrayVal();
        assert(arrayVal.size()==2);
        horz = gravityValueFromString(arrayVal[0].stringVal());
        vert = gravityValueFromString(arrayVal[1].stringVal());
    } else {
        string str = gravityValue.stringVal();
        if (applyToHorz) {
            horz = gravityValueFromString(str);
        }
        if (applyToVert) {
            vert = gravityValueFromString(str);
        }
    }
    setGravity({horz, vert});
}

void processFill(RectRenderOp* op, const style& value) {
    if (value.isCompound()) {
        assert(0); // todo: support compound values
        return;
    }
    op->setFillColor(value.colorVal());
}

RenderOp* View::processDrawable(const style& value) {
    if (value.isEmpty()) {
        return NULL;
    }

    /*if (value->type == StyleValue::Type::String) {
        // todo: handle image
        assert(0);
    }*/

    RectRenderOp* op = new RectRenderOp();
    
    
    if (value.isCompound()) {
        for (auto& it : *value.compound) {
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
        op->setFillColor(value.colorVal());
    }
    return op;
}

static bool isStateName(const string& name) {
    if (name == "enabled") return true;
    if (name == "disabled") return true;
    if (name == "focused") return true;
    if (name == "unfocused") return true;
    if (name == "selected") return true;
    if (name == "unselected") return true;
    if (name == "pressed") return true;
    if (name == "unpressed") return true;
    if (name == "checked") return true;
    if (name == "unchecked") return true;
    if (name == "errored") return true;
    return false;

}
bool View::handleStatemapDeclaration(const string& name, const style& value) {
    
    // A style can only be a statemap if it's a map and one or more of it's keys
    // are view state names. Early exit if not dealing with a statemap style.
    if (!value.isCompound()) {
        return false;
    }
    int c = 0;
    for (auto& k : *value.compound) {
        if (isStateName(k.first)) {
            c++;
        }
    }
    if (!c) {
        return false;
    }
    
    // Find all the non-state values and apply them to the state values.
    map<string,style> nonstateVals;
    for (auto& k : *value.compound) {
        if (!isStateName(k.first)) {
            nonstateVals.insert(k);
        }
    }
    if (nonstateVals.size() > 0) {
        for (auto& k : *value.compound) {
            if (isStateName(k.first)) {
                const_cast<style&>(k.second).importNamedValues(nonstateVals);
            }
        }
    }

    // Remove the nonstate values from the statemap
    for (auto& k : nonstateVals) {
        const_cast<map<string, style>&>(*value.compound).erase(k.first);
    }

    // Ensure we have a valid statemap container with no existing entry for the given name
    if (_statemapStyleValues) {
        _statemapStyleValues->erase(name);
    } else {
        _statemapStyleValues = new map<string, style*>();
    }
    
    // Create the new statemap container entry
    _statemapStyleValues->insert(make_pair(name, (style*)&value));
    
    // Choose initial value immediately
    applyStatemapStyleValue(name, value);
    return true;
}

void View::applyStatemapStyleValue(const string& name, const style& statemap) {
    // Walk the map and find the values which apply. Longest (i.e. most state bits) matching value wins.
    list<style*> matchingValues;
    const style* bestMatchValue = NULL;
    int best_pri=0;
    for (auto& it : *statemap.compound) {
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
        else if (it.first == "errored") {stateset.setBits(STATE_ERRORED, STATE_ERRORED); pri=2;}

        // If the stateset matches
        if ((_state & stateset.mask) == stateset.state && pri>best_pri) {
            bestMatchValue = &it.second;
            best_pri = pri;
        }
    }
    
    if (bestMatchValue) {
        applySingleStyle(name, *bestMatchValue);
    } else {
        style nullVal;
        applySingleStyle(name, nullVal);
    }
}

void View::setNeedsFullRedraw() {
	if (!_window) {
		return;
	}
	if (!_needsFullRedraw) {
		_needsFullRedraw = true;
        if (_renderList) {
            for (auto& op : _renderList->_ops) {
                op->_mustRedraw = true;
            }
        }
        if (_renderListDecor) {
            for (auto& op : _renderListDecor->_ops) {
                op->_mustRedraw = true;
            }
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
        if (_renderList) {
            for (auto it : _renderList->_ops) {
                RenderOp* op = it;
                if (surfaceRect.intersects(op->_rect)) {
                    op->_mustRedraw = true;
                }
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
float View::getLeft() const {
    return _rect.origin.x;
}
float View::getRight() const {
    return _rect.origin.x + _rect.size.width;
}
float View::getTop() const {
    return _rect.origin.y;
}
float View::getBottom() const {
    return _rect.origin.y + _rect.size.height;
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
        if (_surface) {
            _surface->markPrivateRenderQuadDirty();
        }
        return;
    }
    
    // If the view is 'live' - ie part of a window and visible - then the
    // render order might be affected and the batch quads must be updated
    if (_window) {
        if (_renderList) {
            for (auto it : _renderList->_ops) {
                it->rebatchIfNecessary();
            }
        }
        if (_renderListDecor) {
            for (auto it : _renderListDecor->_ops) {
                it->rebatchIfNecessary();
            }
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
    if (!_layoutValid) {
        return;
    }
    View* view = this;
    while (view) {
        view->_layoutValid = false;
        view = view->_parent;
    }
    if (_window) {
        _window->requestRedraw();
    }
}

void View::invalidateContentSize() {
	_contentSizeValid = false;
    if (_widthMeasureSpec.type==MEASURESPEC::TypeContent || _heightMeasureSpec.type==MEASURESPEC::TypeContent) {
		setNeedsLayout();
    } else {
        if (_layoutValid) {
            SIZE constraint = _rect.size;
            constraint.width -= (_padding.left + _padding.right);
            constraint.height -= (_padding.top + _padding.bottom);
            updateContentSize(constraint);
            _contentSizeValid = true;
        }
    }
    setNeedsFullRedraw();
}

void View::updateContentSize(SIZE constrainingSize) {
    _contentSize.width = _contentSize.height = 0;
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
    setNeedsLayout(); // todo: gravity shouldn't affect layout, it should only trigger a repaint
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


float View::getAlignspecVal(const ALIGNSPEC& spec, bool isVertical) {

    View* anchor = (spec.anchor == NO_ANCHOR) ? NULL : (spec.anchor?spec.anchor:_parent);
    float anchorVal = 0;
    RECT& refRect = anchor ? anchor->_rect : _window->_rect;
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
        val = spec.calc(ownSize, anchorVal, anchorSize);
    } else {
        val = isVertical ? _rect.origin.y : _rect.origin.x;
    }
    return val;
}

void View::layout(RECT constraint) {
    if (_visibility == Gone) {
        return;
    }

    //assert(!_layoutValid);
    SIZE refSize = {0,0};
    
    // Calculate the rect this view is positioning itself within
    RECT alignRect;
    if (_alignspecHorz.anchor) {
        assert(_alignspecHorz.anchor->_layoutValid);
        alignRect.origin.x = _alignspecHorz.anchor->_rect.origin.x;
        alignRect.size.width = _alignspecHorz.anchor->_rect.size.width;
    } else {
        alignRect.origin.x = constraint.origin.x;
        alignRect.size.width = constraint.size.width;
    }
    if (_alignspecVert.anchor) {
        assert(_alignspecVert.anchor->_layoutValid);
        alignRect.origin.y = _alignspecVert.anchor->_rect.origin.y;
        alignRect.size.height = _alignspecVert.anchor->_rect.size.height;
    } else {
        alignRect.origin.y = constraint.origin.y;
        alignRect.size.height = constraint.size.height;
    }
    

    // Handle relative width & height early as it may involve laying out a sibling view before this
    if (_widthMeasureSpec.type==MEASURESPEC::TypeRelative) {
        if (_widthMeasureSpec.ref) {
            if (!_widthMeasureSpec.ref->_layoutValid) {
                RECT constraintForSubviews = constraint;
                constraintForSubviews.origin = {0,0};
                _padding.applyToRect(constraintForSubviews);
                _widthMeasureSpec.ref->layout(constraintForSubviews);
            }
            refSize.width = _widthMeasureSpec.ref->getWidth();
        } else {
            refSize.width = constraint.size.width;
        }
        assert(refSize.width >= 0); // relative-size to content-wrapping parent not allowed!
    }
    else if (_widthMeasureSpec.type==MEASURESPEC::TypeFill) {
        if (_alignspecHorz.anchor) assert(_alignspecHorz.anchor->_layoutValid);
        float aa = _alignspecHorz.anchor ? alignRect.origin.x : 0;
        float x = floorf(_alignspecHorz.calc(constraint.size.width, aa, alignRect.size.width));
        refSize.width = constraint.size.width - fabs(x);
    }
    if (_heightMeasureSpec.type==MEASURESPEC::TypeRelative) {
        if (_heightMeasureSpec.ref) {
            if (!_heightMeasureSpec.ref->_layoutValid) {
                RECT constraintForSubviews = constraint;
                constraintForSubviews.origin = {0,0};
                _padding.applyToRect(constraintForSubviews);
                _heightMeasureSpec.ref->layout(constraintForSubviews);
            }
            refSize.height = _heightMeasureSpec.ref->getHeight();
        } else {
            refSize.height = constraint.size.height;
        }
        assert(refSize.height >= 0); // relative-size to content-wrapping parent not allowed!
    }
    else if (_heightMeasureSpec.type==MEASURESPEC::TypeFill) {
        if (_alignspecVert.anchor) assert(_alignspecVert.anchor->_layoutValid);
        float aa = _alignspecVert.anchor ? alignRect.origin.y : 0;
        float y = floorf(_alignspecVert.calc(constraint.size.height, aa, alignRect.size.height));
        refSize.height = constraint.size.height - fabs(y);
    }

    // Work out the constraining size. If our sizespec is relative to parent or sibling then the
    // constraining size is whatever the spec works out to, but if it's content based then the
    // constraining size is the one passed into this function.
    SIZE constrainingSize = constraint.size;
    if (_widthMeasureSpec.type != MEASURESPEC::TypeContent) {
        constrainingSize.width = ceilf(refSize.width * _widthMeasureSpec.mul + _widthMeasureSpec.con);
    }
    if (_heightMeasureSpec.type != MEASURESPEC::TypeContent) {
        constrainingSize.height = ceilf(refSize.height * _heightMeasureSpec.mul + _heightMeasureSpec.con);
    }

    // If content size is currently invalid, now's the time to update it
    if (!_contentSizeValid) {
        SIZE constrainingSizeMinusPadding = constrainingSize;
        constrainingSizeMinusPadding.width -= _padding.left+_padding.right;
        constrainingSizeMinusPadding.height -= _padding.top+_padding.bottom;
        updateContentSize(constrainingSizeMinusPadding);
        _contentSizeValid = true;
    }
#if DEBUG
    if (_debugTag.length()) {
        app.log("%s: contentSize is %f x %f", _debugTag.data(), _contentSize.width, _contentSize.height);
    }
#endif

    // Handle intrinsic content-based width and/or height
    if (_widthMeasureSpec.type==MEASURESPEC::TypeContent && _contentSize.width>0) {
        refSize.width = _padding.left + _contentSize.width + _padding.right;
    }
    if (_heightMeasureSpec.type==MEASURESPEC::TypeContent && _contentSize.height>0) {
        refSize.height = _padding.top + _contentSize.height + _padding.bottom;
    }


    // Layout subviews
    if (_subviews.size() > 0) {
        
        // If wrapping content but then use the constraint size and update afterwards
        if (_widthMeasureSpec.type==MEASURESPEC::TypeContent && _contentSize.width<=0) {
            refSize.width = constraint.size.width;
        }
        if (_heightMeasureSpec.type==MEASURESPEC::TypeContent && _contentSize.height<=0) {
            refSize.height = constraint.size.height;
        }
        
        RECT constraintForSubviews;
        constraintForSubviews.origin = {0,0};
        constraintForSubviews.size.width = ceilf(refSize.width * _widthMeasureSpec.mul + _widthMeasureSpec.con);
        constraintForSubviews.size.height = ceilf(refSize.height * _heightMeasureSpec.mul + _heightMeasureSpec.con);
        _padding.applyToRect(constraintForSubviews);
        
#if DEBUG
        if (_debugTag.length()) {
            app.log("%s: laying out subviews in %s", _debugTag.data(), constraintForSubviews.toString().data());
        }
#endif

        layoutSubviews(constraintForSubviews);
        
        // Update content size to include subviews
        SIZE subviewExtent = {0,0};
        for (int i=0 ; i<_subviews.size() ; i++) {
            View* view = _subviews.at(i);
            subviewExtent.width = fmaxf(subviewExtent.width, view->getRight());
            subviewExtent.height = fmaxf(subviewExtent.height, view->getBottom());
        }
        _contentSize.width = fmaxf(_contentSize.width, subviewExtent.width-_padding.left);
        _contentSize.height = fmaxf(_contentSize.height, subviewExtent.height-_padding.top);
        
        
        // Handle subview-based width and/or height
        if (_widthMeasureSpec.type==MEASURESPEC::TypeContent) {
            refSize.width = _padding.left + _contentSize.width + _padding.right;
        }
        if (_heightMeasureSpec.type==MEASURESPEC::TypeContent) {
            refSize.height = _padding.top + _contentSize.height + _padding.bottom;
        }
    }

    
    
    // Handle aspect-ratio measuring
    if (_widthMeasureSpec.type==MEASURESPEC::TypeAspect) {
        refSize.width = refSize.height;
    }
    if (_heightMeasureSpec.type==MEASURESPEC::TypeAspect) {
        refSize.height = refSize.width;
    }
    
    // Calculate the new rect, aligning to pixel grid
    RECT rect;
    rect.size.width = ceilf(refSize.width * _widthMeasureSpec.mul + _widthMeasureSpec.con);
    rect.size.height = ceilf(refSize.height * _heightMeasureSpec.mul + _heightMeasureSpec.con);
    rect.origin.x = floorf(_alignspecHorz.calc(rect.size.width, alignRect.origin.x, alignRect.size.width));
    rect.origin.y = floorf(_alignspecVert.calc(rect.size.height, alignRect.origin.y, alignRect.size.height));

    // Set the view rect
    setRectOrigin(rect.origin);
    setRectSize(rect.size);

    updateScrollbarVisibility();
    
#if DEBUG
    if (_debugTag.length()) {
        app.log("%s: < layout() _rect is %s", _debugTag.data(), _rect.toString().data());
    }
#endif

    _layoutValid = true;
}


void View::layoutSubviews(RECT constraint) {
    for (int i=0 ; i<_subviews.size() ; i++) {
        View* view = _subviews.at(i);
        //if (!view->_layoutValid) {
            view->layout(constraint);
        //}
    }
}

void View::setPadding(EDGEINSETS padding) {
	this->_padding = padding;
    setNeedsLayout();
}

void View::updatePrivateSurface(bool updateSubviews) {
    assert(!_surface);
    _surface = _parent ? _parent->_surface : _window->_surface;
    if (_ownsPrivateSurface) {
        _surface = _window->_renderer->createPrivateSurface();
        _needsFullRedraw = true;
    }

    if (updateSubviews) {
        for (auto it = _subviews.begin(); it!=_subviews.end() ; it++) {
            sp<View> subview = *it;
            subview->updatePrivateSurface(true);
        }
    }
}


void View::attachToWindow(Window *window) {
    if (_visibility != Visible) {
        return;
    }
    if (_window) {
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
        if (_renderList) {
            _surface->attachRenderList(_renderList);
        }
        if (_renderListDecor) {
            _surface->attachRenderList(_renderListDecor);
        }
    }
    
	for (auto it = _subviews.begin(); it!=_subviews.end() ; it++) {
		sp<View> subview = *it;
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
        if (_renderList) {
            _surface->detachRenderList(_renderList);
        }
        if (_renderListDecor) {
            _surface->detachRenderList(_renderListDecor);
        }
    }

    // Recurse through subviews
    for (vector<sp<View>>::iterator it = _subviews.begin(); it!=_subviews.end() ; it++) {
        sp<View> subview = *it;
        subview->detachFromWindow();
    }

    // If this view owns its surface, remove the op that renders it from the render target
    if (_ownsPrivateSurface && _surface->_op) {
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

int View::getSubviewCount() {
    int c = (int)_subviews.size();
    return c;
}

View* View::getSubview(int index) {
    if (index>=0 && index<_subviews.size()) {
        return _subviews[index];
    }
    return NULL;
}

View* View::getRootView() {
    View* v = this;
    while (v->_parent) {
        v = v->_parent;
    }
    return v;
}

void View::insertSubview(View* subview, int index) {

    _subviews.insert(_subviews.begin()+index, subview);
	subview->_parent = this;
	if (_window) {
		subview->attachToWindow(_window);
        subview->setNeedsLayout();
	}
}


int View::indexOfSubview(View* subview) {
	int i=0;
    for (auto& it : _subviews) {
		if (subview == it) {
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
bool View::isSelected() const {
    return (_state & STATE_SELECTED)!=0;
}
void View::setSelected(bool selected) {
    setState(STATE_SELECTED, (STATE)(selected?STATE_SELECTED:0));
}
bool View::isFocused() const {
    return (_state & STATE_FOCUSED)!=0;
}
void View::setFocused(bool focused) {
    setState(STATE_FOCUSED, (STATE)(focused?STATE_FOCUSED:0));
}
bool View::isErrored() const {
    return (_state & STATE_ERRORED)!=0;
}
void View::setErrored(bool errored) {
    setState(STATE_ERRORED, (STATE)(errored?STATE_ERRORED:0));
}



void View::setState(STATE mask, STATE value) {
    uint16_t oldstate = _state;
    uint16_t newstate = (_state & ~mask) | value;
    if (oldstate != newstate) {
        _state = newstate;
        onStateChanged({(STATE)(oldstate^newstate), newstate});
        setNeedsFullRedraw();
        if (_subviewsInheritState) {
            for (auto& subview : _subviews) {
                subview->setState(mask, value);
            }
        }
    }
}
void View::onStateChanged(STATESET changedStates) {
    if (_statemapStyleValues) {
        for (auto& it : *_statemapStyleValues) {
            applyStatemapStyleValue(it.first, *it.second);
        }
    }
}

void View::addDecorOp(RenderOp* renderOp) {
    addRenderOpToList(renderOp, false, _renderListDecor);
}

void View::removeDecorOp(RenderOp* renderOp) {
    removeRenderOpFromList(renderOp, _renderListDecor);
}


void View::addRenderOp(RenderOp* renderOp) {
    addRenderOp(renderOp, false);
}
void View::addRenderOp(RenderOp* renderOp, bool atFront) {
    addRenderOpToList(renderOp, atFront, _renderList);
}
void View::removeRenderOp(RenderOp* renderOp) {
    removeRenderOpFromList(renderOp, _renderList);
}

void View::addRenderOpToList(RenderOp* renderOp, bool atFront, sp<RenderList>& list) {
    if (renderOp->_view) {
        assert(renderOp->_view == this);
        return;
    }
    assert(!renderOp->_view);
    renderOp->_view = this;
    renderOp->setAlpha(_effectiveAlpha);

    if (!list) {
        list = new RenderList();
        if (_surface) {
            _surface->attachRenderList(list);
        }
    }
    list->addRenderOp(renderOp, atFront);
    if (_surface) {
        _surface->addRenderOp(renderOp);
    }
}

void View::removeRenderOpFromList(RenderOp* renderOp, sp<RenderList>& list) {

    assert(renderOp->_view);
    if (_surface) {
        _surface->removeRenderOp(renderOp);
    }
    list->removeRenderOp(renderOp);
    if (!list->_ops.size()) {
        if (_surface) {
            _surface->detachRenderList(list);
        }
        list = NULL;
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
    POINT targetContentOffset = _contentOffset + scrollAmount;
    targetContentOffset.y = MAX(targetContentOffset.y, 0);
    targetContentOffset.y = MIN(targetContentOffset.y, getMaxScrollY());
    scrollAmount = targetContentOffset - origContentOffset;
    if (scrollAmount.y == 0 && scrollAmount.x == 0) {
        return;
    }
    Animation::start(this, 350, [=](float a) { // todo: style
        POINT d = {a*scrollAmount.x, a*scrollAmount.y};
        POINT newOffset = origContentOffset + d;
        setContentOffset(newOffset);
    }, Animation::linear);

}


bool View::canScrollVertically() {
    return _scrollVert.canScroll(this, true);
}
bool View::canScrollHorizontally() {
    return _scrollHorz.canScroll(this, false);
}

float View::getMaxScrollY() {
    return _scrollVert.maxScroll(this, true);
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
        for (long i=_subviews.size()-1 ; i>=0 ; i--) {
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
        event->ptLocal = hitPoint + hitView->_contentOffset;
		if (hitView->handleInputEvent(event)) {
			return hitView;
		}
        event->ptLocal -= hitView->_contentOffset;
        hitPoint.x += hitView->_rect.origin.x;
        hitPoint.y += hitView->_rect.origin.y;
		hitView = hitView->_parent;
        if (hitView) {
            hitPoint.x -= hitView->_contentOffset.x;
            hitPoint.y -= hitView->_contentOffset.y;
        }
	}

	return handleInputEvent(event) ? this : NULL;
}


bool View::handleInputEvent(INPUTEVENT* event) {

    bool retVal = false;
    
    if (isEnabled()) {
        if (event->type == INPUT_EVENT_DOWN) {
            setPressed(true);
        }
        if (event->type == INPUT_EVENT_CANCEL || event->type==INPUT_EVENT_UP) {
            setPressed(false);
        }
        
        if (event->type == INPUT_EVENT_DRAG) {
            if (canScrollVertically() || canScrollHorizontally()) {
                retVal = true;
            }
        }


        if ((onInputEvent || onClick)) {
            retVal = true;
        }
    }
    
    //app.log("%X y=%f", this, event->pt.y);
    bool scrolled = _scrollVert.handleEvent(this, true, event);
    scrolled |= _scrollHorz.handleEvent(this, false, event);
    
    if (onClick) {
        retVal = true;
        if (event->type == INPUT_EVENT_TAP) {
            onClick();
        }
    }
	if (onInputEvent) {
        return onInputEvent(this, event);
	}


	return retVal || scrolled;
}

IKeyboardInputHandler* View::getKeyboardInputHandler() {
    return NULL;
}
ITextInputReceiver* View::getTextInputReceiver() {
    return NULL;
}

bool View::requestFocus() {
    if (_window) {
        return _window->setFocusedView(this);
    }
    return false;
}


POINT View::mapPointToWindow(POINT pt) {
	if (_window) {
		View* view = this;
		while (view) {
			pt.x += view->_rect.origin.x - view->_contentOffset.x;
			pt.y += view->_rect.origin.y - view->_contentOffset.y;
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
    if (_renderList) {
        for (auto it : _renderList->_ops) {
            it->setAlpha(_effectiveAlpha);
        }
    }
    if (_renderListDecor) {
        for (auto it : _renderListDecor->_ops) {
            it->setAlpha(_effectiveAlpha);
        }
    }
    for (auto it = _subviews.begin() ; it!=_subviews.end() ; it++) {
        (*it)->updateEffectiveAlpha();
    }
}

void View::updateEffectiveTint() {
    COLOR parentVal = _parent ? _parent->_effectiveTintColor : COLOR(0);
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
Animation* View::animateInFromBottom(float duration, InterpolateFunc interpolater/* = strongEaseOut*/) {
    return Animation::start(this, duration, [=](float val) {
        setTranslate({0, _rect.size.height * (1-val)});
    }, interpolater);
}
Animation* View::animateOutToBottom(float duration, InterpolateFunc interpolater/* = strongEaseIn*/) {
    return Animation::start(this, duration, [=](float val) {
        setTranslate({0, _rect.size.height * (val)});
    }, interpolater);
}


void View::setTranslate(POINT translation) {
    if (translation.x==0 && translation.y==0) {
        if (_matrix) {
            delete _matrix;
            _matrix = NULL;
        }
    } else {
        if (!_matrix) {
            _matrix = new MATRIX4();
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
    sprintf(ach, "%lX:%s", (long)this, debugViewType().data());
    return string(ach);
}
void View::debugDumpTree(int depth) {
    

    string line;
    for (int i=0 ; i<depth ; i++) line.append("-");
    line.append(debugDescription());
    app.log(line.data());

    if (_renderList) {
        for (auto op : _renderList->_ops) {
            if (op->_batch) {
                assert(op->_batch->_surface == _surface);
            }
        }
    }

    for (int i=0 ; i<_subviews.size() ; i++) {
        _subviews.at(i)->debugDumpTree(depth+2);
    }
}

#endif

