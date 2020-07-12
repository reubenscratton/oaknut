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
                _widthMeasureSpec(MEASURESPEC::Abs(0)),
                _heightMeasureSpec(MEASURESPEC::Abs(0)),
                _clipsContents(true)
{
    _visibility = Visible;
}

View::~View() {
//	log_dbg("~View()");
    if (_dynamicStyles) {
        delete _dynamicStyles;
    }
}

void View::inflate(const string& layoutFile) {
    app->layoutInflateExistingView(this, layoutFile);
}


const style* View::resolveQualifiedStyle(const style* val) {
    const style* defaultVal = nullptr;
    const style* bestVal = nullptr;
    for (auto& it : *val->qualmap) {
        uint32_t qual = it.first;
        if (0 == qual) {
            defaultVal = &it.second;
            continue;
        }
        assert((qual & 0xff) == 1); // Q_VIEWSTATE
        VIEWSTATE viewstate((qual & 0xFF0000)>>16, (qual&0xFF00)>>8);
        bool applies = (_state & viewstate.mask) == viewstate.state;
        if (applies) {
            bestVal = &it.second;
        }
    }
    return style::resolve(bestVal ? bestVal : defaultVal);
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
    } else if (name == "background" || name=="background-color") {
        /*if (value->type == StyleValue::Type::String) {
             // todo: handle image
             assert(0);
        }*/
        setBackgroundColor(value.colorVal());
        return true;
    } else if (name == "background-alpha" || name == "fill-alpha") {
        setBackgroundColorAlpha(value.floatVal());
        return true;
    } else if (name == "stroke-color" || name == "stroke") {
        setBackgroundStrokeColor(value.colorVal());
        return true;
    } else if (name == "stroke-width") {
        setBackgroundStrokeWidth(value.floatVal());
        return true;
    } else if (name == "corner-radius" || name == "corner-radii") {
        setBackgroundCornerRadii(value.cornerRadiiVal());
        return true;
    } else if (name == "background-insets") {
        setBackgroundEdgeInsets(value.edgeInsetsVal());
        return true;
    } else if (name == "ink" || name == "ink-color") {
        setInkColor(value.colorVal());
        return true;
    } else if (name == "ink-color-alpha") {
        setInkColorAlpha(value.floatVal());
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
            log_warn("Invalid visibility: '%s'", str.c_str());
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
    } else if (name == "padding-left") {
        setPadding(EDGEINSETS(value.floatVal(),_padding.top,_padding.right,_padding.bottom));
        return true;
    } else if (name == "padding-right") {
        setPadding(EDGEINSETS(_padding.left,_padding.top,value.floatVal(),_padding.bottom));
        return true;
    } else if (name == "padding-top") {
        setPadding(EDGEINSETS(_padding.left,value.floatVal(),_padding.right,_padding.bottom));
        return true;
    } else if (name == "padding-bottom") {
        setPadding(EDGEINSETS(_padding.left,_padding.top,_padding.right,value.floatVal()));
        return true;
    } else if (name == "tint") {
        setTintColor(value.colorVal());
        return true;
    } else if (name == "scroll-insets") {
        setScrollInsets(value.edgeInsetsVal());
        return true;
    } else if (name == "elevation") {
        setElevation(value.floatVal(), true);
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
        if (!_ownsSurface) {
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
	if (usePrivateSurface != _ownsSurface) {
        if (_surface) {
            detachFromSurface();
        }
		_ownsSurface = usePrivateSurface;
        _surface = NULL;
        setNeedsFullRedraw();
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

void View::setLayoutRect(const RECT& rect) {
    setRectSize(rect.size);
    setRectOrigin(rect.origin);
    _widthMeasureSpec = MEASURESPEC::Abs(_rect.size.width);
    _heightMeasureSpec = MEASURESPEC::Abs(_rect.size.height);
    _alignspecHorz = {0,0,0, _rect.origin.x};
    _alignspecVert = {0,0,0, _rect.origin.y};
}

void View::setRectOrigin(const POINT& origin) {
    POINT o = origin;
    o.x = floorf(o.x);
    o.y = floorf(o.y);

    POINT d;
    d.x = o.x - _rect.origin.x;
    d.y = o.y - _rect.origin.y;
    if (d.x || d.y) {
        _rect.origin = o;
        adjustSurfaceOrigin(d);
    }
}

void View::adjustSurfaceOrigin(const POINT& d) {
    _surfaceOrigin += d;
    
    // If this view is a surface owner, no need to propagate the change to renderlist or subviews
    if (_ownsSurface) {
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
        if (_shadowOp) {
            _shadowOp->setRect(_rect);
        }
        _window->requestRedraw();
    }
    
    // Propagate change in surface origin to subviews
    for (auto it=_subviews.begin() ; it!=_subviews.end() ; it++) {
        (*it)->adjustSurfaceOrigin(d);
    }
}

void View::setRectSize(const SIZE& size) {
    SIZE s;
    s.width = ceilf(size.width);
    s.height = ceilf(size.height);
    SIZE d;
    d.width = s.width - _rect.size.width;
    d.height = s.height - _rect.size.height;
    if (d.width || d.height) {
        _rect.size = s;
        if (_shadowOp) {
            _shadowOp->setRect(_rect);
        }
        updateBackgroundRect();
        setNeedsFullRedraw();
    }
}

void View::updateBackgroundRect() {
    RECT rect = getOwnRect();
    if (_inkOp) {
        _inkOp->setRect(rect);
    }
    if (_backgroundOp) {
        rect.origin.x += (int)_contentOffset.x;
        rect.origin.y += (int)_contentOffset.y;
        _backgroundOp->_inset.applyToRect(rect);
        _backgroundOp->setRect(rect);
    }
}

void View::setLayoutSize(MEASURESPEC widthMeasureSpec, MEASURESPEC heightMeasureSpec) {
	_widthMeasureSpec = widthMeasureSpec;
	_heightMeasureSpec = heightMeasureSpec;
	setNeedsLayout();
}
void View::setLayoutSize(const SIZE& size) {
    _widthMeasureSpec = MEASURESPEC::Abs(size.width);
    _heightMeasureSpec = MEASURESPEC::Abs(size.height);
    setNeedsLayout();
}

void View::setLayoutOrigin(ALIGNSPEC alignspecHorz, ALIGNSPEC alignspecVert) {
	_alignspecHorz = alignspecHorz;
	_alignspecVert = alignspecVert;
	setNeedsLayout();
}


void View::setNeedsLayout() {
    if (!_layoutValid) {
        return;
    }
    
    // Invalidate all ancestors layouts to ensure next layout pass reaches this view
    View* view = this;
    while (view) {
        view->_layoutValid = false;
        view = view->_parent;
    }
    if (_window) {
        _window->requestRedraw();
    }
}

void View::invalidateIntrinsicSize() {
	_intrinsicSizeValid = false;
    if (_widthMeasureSpec.type==MEASURESPEC::TypeContent || _heightMeasureSpec.type==MEASURESPEC::TypeContent) {
        for (auto subview : _subviews) {
            subview->invalidateIntrinsicSize();
        }
        setNeedsLayout();
    } else {
        if (_layoutValid) {
            SIZE constraint = _rect.size;
            constraint.width -= (_padding.left + _padding.right);
            constraint.height -= (_padding.top + _padding.bottom);
            updateIntrinsicSize(constraint);
            _intrinsicSizeValid = true;
            setNeedsLayout();
        }
    }
    setNeedsFullRedraw();
}

void View::updateIntrinsicSize(SIZE constrainingSize) {
    _intrinsicSize.width = _intrinsicSize.height = 0;
}

void View::sizeToFit(float widthConstraint/*=FLT_MAX*/) {
    updateIntrinsicSize({widthConstraint,FLT_MAX});
    SIZE size = _intrinsicSize;
    size.width += _padding.left + _padding.right;
    size.height += _padding.top + _padding.bottom;
    setRectSize(size);
}

bool View::clipsContents() const {
    return _clipsContents;
}
void View::setClipsContents(bool clipsContents) {
    if (clipsContents != _clipsContents) {
        _clipsContents = clipsContents;
        setNeedsFullRedraw();
    }
}


void View::setGravity(GRAVITY gravity) {
	_gravity = gravity;
    setNeedsLayout(); // todo: gravity shouldn't affect layout, it should only trigger a repaint
}

bool View::isHidden() const {
    return _visibility!=Visible;
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
                log_warn("View layout is not clever enough for non-sibling anchors");
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

void View::layout(RECT containingRect) {
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
        alignRect.origin.x = containingRect.origin.x;
        alignRect.size.width = containingRect.size.width;
    }
    if (_alignspecVert.anchor) {
        assert(_alignspecVert.anchor->_layoutValid);
        alignRect.origin.y = _alignspecVert.anchor->_rect.origin.y;
        alignRect.size.height = _alignspecVert.anchor->_rect.size.height;
    } else {
        alignRect.origin.y = containingRect.origin.y;
        alignRect.size.height = containingRect.size.height;
    }
    

    // Handle relative width & height early as it may involve laying out a sibling view before this
    if (_widthMeasureSpec.type==MEASURESPEC::TypeRelative) {
        if (_widthMeasureSpec.ref) {
            if (!_widthMeasureSpec.ref->_layoutValid) {
                RECT constraintForSubviews = containingRect;
                constraintForSubviews.origin = {0,0};
                _padding.applyToRect(constraintForSubviews);
                _widthMeasureSpec.ref->layout(constraintForSubviews);
            }
            refSize.width = _widthMeasureSpec.ref->getWidth();
        } else {
            refSize.width = containingRect.size.width;

            // If aligning to a sibling view AND the reference view is the parent, then
            // subtract the sibling size so that the obvious intention (fill space not
            // used by sibling) is easy to do. You're never going to want to match parent
            // size while aligning to a sibling, and if you ever *do* want that then simply
            // explicitly set the 'ref' to the parent.
            if (_alignspecHorz.anchor && _widthMeasureSpec.mul>=1.0f) {
                float x = floorf(_alignspecHorz.calc(containingRect.size.width, alignRect.origin.x, alignRect.size.width));
                refSize.width -= fabs(x);
            }
        }
    }
    
    if (_heightMeasureSpec.type==MEASURESPEC::TypeRelative) {
        if (_heightMeasureSpec.ref) {
            if (!_heightMeasureSpec.ref->_layoutValid) {
                RECT constraintForSubviews = containingRect;
                constraintForSubviews.origin = {0,0};
                _padding.applyToRect(constraintForSubviews);
                _heightMeasureSpec.ref->layout(constraintForSubviews);
            }
            refSize.height = _heightMeasureSpec.ref->getHeight();
        } else {
            refSize.height = containingRect.size.height;
            
            // If using a sibling anchor, subtract sibling size
            if (_alignspecVert.anchor && _heightMeasureSpec.mul>=1.0f) {
                float y = floorf(_alignspecVert.calc(containingRect.size.height, alignRect.origin.y, alignRect.size.height));
                refSize.height -= fabs(y);
            }
        }
        
        // assert(refSize.height >= 0); // relative-size to content-wrapping parent not allowed!
    }

    // Work out the constraining size. If our sizespec is relative to parent or sibling then the
    // constraining size is whatever the spec works out to, but if it's content based then the
    // constraining size is the one passed into this function.
    SIZE constrainingSize = containingRect.size;
    if (_widthMeasureSpec.type != MEASURESPEC::TypeContent) {
        constrainingSize.width = floorf(refSize.width * _widthMeasureSpec.mul + _widthMeasureSpec.con);
    }
    if (_heightMeasureSpec.type != MEASURESPEC::TypeContent) {
        constrainingSize.height = floorf(refSize.height * _heightMeasureSpec.mul + _heightMeasureSpec.con);
    }

    // If content size is currently invalid, now's the time to update it
    if (!_intrinsicSizeValid) {
        SIZE constrainingSizeMinusPadding = constrainingSize;
        constrainingSizeMinusPadding.width -= _padding.left+_padding.right;
        constrainingSizeMinusPadding.height -= _padding.top+_padding.bottom;
        updateIntrinsicSize(constrainingSizeMinusPadding);
        _intrinsicSizeValid = true;
    }
#if DEBUG
    if (_debugTag.length()) {
        log_dbg("%s: _intrinsicSize is %f x %f", _debugTag.c_str(), _intrinsicSize.width, _intrinsicSize.height);
    }
#endif

    // Handle intrinsic content-based width and/or height
    if (_widthMeasureSpec.type==MEASURESPEC::TypeContent && _intrinsicSize.width>0) {
        refSize.width = _padding.left + _intrinsicSize.width + _padding.right;
    }
    if (_heightMeasureSpec.type==MEASURESPEC::TypeContent && _intrinsicSize.height>0) {
        refSize.height = _padding.top + _intrinsicSize.height + _padding.bottom;
    }

    _layoutValid = true;


    // Layout subviews
    if (_subviews.size() == 0) {
        _contentSize.width = ceilf(_intrinsicSize.width);
        _contentSize.height = ceilf(_intrinsicSize.height);
    } else {
        // If wrapping content but then use the constraint size and update afterwards
        if (_widthMeasureSpec.type==MEASURESPEC::TypeContent && _intrinsicSize.width<=0) {
            refSize.width = containingRect.size.width;
        }
        if (_heightMeasureSpec.type==MEASURESPEC::TypeContent && _intrinsicSize.height<=0) {
            refSize.height = containingRect.size.height;
        }
        
        RECT constraintForSubviews;
        constraintForSubviews.origin = {0,0};
        constraintForSubviews.size.width = floorf(refSize.width * _widthMeasureSpec.mul + _widthMeasureSpec.con);
        constraintForSubviews.size.height = floorf(refSize.height * _heightMeasureSpec.mul + _heightMeasureSpec.con);
        _padding.applyToRect(constraintForSubviews);
        
#if DEBUG
        if (_debugTag.length()) {
            log_dbg("%s: laying out subviews in %s", _debugTag.c_str(), constraintForSubviews.toString().c_str());
        }
#endif

        layoutSubviews(constraintForSubviews);
        
        // Update content size to be the union of the intrinsic content size and the extent of subviews
        SIZE subviewExtent = {0,0};
        for (int i=0 ; i<_subviews.size() ; i++) {
            View* view = _subviews.at(i);
            subviewExtent.width = fmaxf(subviewExtent.width, view->getRight());
            subviewExtent.height = fmaxf(subviewExtent.height, view->getBottom());
        }
        _contentSize.width = ceilf(fmaxf(_intrinsicSize.width, subviewExtent.width-_padding.left));
        _contentSize.height = ceilf(fmaxf(_intrinsicSize.height, subviewExtent.height-_padding.top));
        
        
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
    rect.size.width = refSize.width * _widthMeasureSpec.mul + _widthMeasureSpec.con;
    rect.size.height = refSize.height * _heightMeasureSpec.mul + _heightMeasureSpec.con;
    rect.origin.x = _alignspecHorz.calc(rect.size.width, alignRect.origin.x, alignRect.size.width);
    rect.origin.y = _alignspecVert.calc(rect.size.height, alignRect.origin.y, alignRect.size.height);

    // Set the view rect
    setRectOrigin(rect.origin);
    setRectSize(rect.size);

    updateScrollbarVisibility();
    
#if DEBUG
    if (_debugTag.length()) {
        log_dbg("%s: < layout() _rect is %s", _debugTag.c_str(), _rect.toString().c_str());
    }
#endif

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

    updateEffectiveAlpha();
    updateEffectiveTint();
    
    // Recurse subviews
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
    
    detachFromSurface();
    

    // Recurse through subviews
    for (vector<sp<View>>::iterator it = _subviews.begin(); it!=_subviews.end() ; it++) {
        sp<View> subview = *it;
        subview->detachFromWindow();
    }
    
    _window = nullptr;
    _contentOffsetAccum = _contentOffset;
}

void View::attachToSurface() {
    
}
void View::detachFromSurface() {
    
    // Unbatch our ops
    if (_surface) {
        if (_renderList) {
            _surface->detachRenderList(_renderList);
        }
        if (_renderListDecor) {
            _surface->detachRenderList(_renderListDecor);
        }
    
        // If this view owns its surface, remove the op that renders it from the render target
        if (_ownsSurface && _surface->_op) {
            _surface->_op = NULL;
        }
        
        _surface = nullptr;

        // Recurse through subviews
        for (vector<sp<View>>::iterator it = _subviews.begin(); it!=_subviews.end() ; it++) {
            sp<View> subview = *it;
            subview->detachFromSurface();
        }

    }

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
    assert(subview != this);
    assert(!subview->_parent);
    _subviews.insert(_subviews.begin()+index, subview);
	subview->_parent = this;
    if (subview->_shadowOp) {
        addRenderOp(subview->_shadowOp);
    }
	if (_window) {
		subview->attachToWindow(_window);
        subview->setNeedsLayout();
	}
}

void View::iterateSubviews(std::function<bool(View*)> callback) {
    for (View* subview : _subviews) {
        if (!callback(subview)) {
            break;
        }
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
    removeSubview(indexOfSubview(subview));
}

void View::removeSubview(int index) {
    bool index_ok = index>=0 && index<_subviews.size();
	assert(index_ok);
	if (index_ok) {
        View* subview = _subviews[index];
        if (subview->_shadowOp) {
            removeRenderOp(subview->_shadowOp);
        }
		if (_window) {
			subview->detachFromWindow();
		}
        subview->_parent = nullptr;
		_subviews.erase(_subviews.begin() + index);
	}
}
void View::removeAllSubviews() {
    for (int i=(int)_subviews.size()-1 ; i>=0 ; i--) {
        removeSubview(i);
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
        _backgroundOp = (RectRenderOp*)op;
        if (_backgroundOp) {
            updateBackgroundRect();
            addRenderOp(_backgroundOp, true);
        }
    }
}

void View::ensureBackgroundOpExists() {
    if (!_backgroundOp) {
        _backgroundOp = new RectRenderOp();
        updateBackgroundRect();
        addRenderOp(_backgroundOp, true);
    }
}

void View::setBackgroundColor(COLOR color) {
    ensureBackgroundOpExists();
    _backgroundOp->setColor(color);
}
void View::setBackgroundColorAlpha(float alpha) {
    ensureBackgroundOpExists();
    _backgroundOp->setFillColorAlpha(alpha);
}
void View::setBackgroundAlpha(float alpha) {
    ensureBackgroundOpExists();
    _backgroundOp->setAlpha(alpha);
}
void View::setBackgroundStrokeColor(COLOR color) {
    ensureBackgroundOpExists();
    _backgroundOp->setStrokeColor(color);
}
void View::setBackgroundStrokeWidth(float strokeWidth) {
    ensureBackgroundOpExists();
    _backgroundOp->setStrokeWidth(strokeWidth);
}
void View::setBackgroundCornerRadii(const VECTOR4& cornerRadii) {
    ensureBackgroundOpExists();
    _backgroundOp->setCornerRadii(cornerRadii);
    if (_inkOp) {
        _inkOp->setCornerRadius(cornerRadii.x);
    }
}
void View::setBackgroundEdgeInsets(const EDGEINSETS& edgeInsets) {
    ensureBackgroundOpExists();
    _backgroundOp->setInset(edgeInsets);
}


void View::ensureInkOpExists() {
    if (!_inkOp) {
        _inkOp = new InkRenderOp();
        _inkOp->setAlpha(1);
        if (_backgroundOp) {
            _inkOp->setCornerRadius(_backgroundOp.as<RectRenderOp>()->_cornerRadii.x);
        }
        addRenderOp(_inkOp);
    }
}

void View::setInkColor(COLOR inkColor) {
    ensureInkOpExists();
    _inkOp->setColor(inkColor);
}
void View::setInkColorAlpha(float inkAlpha) {
    ensureInkOpExists();
    _inkOp->setFillColorAlpha(inkAlpha);
}




bool View::isPressed() const {
    return (_state & STATE_PRESSED)!=0;
}
void View::setPressed(bool isPressed) {
    setState(VIEWSTATE(STATE_PRESSED, (isPressed?STATE_PRESSED:0)));
}
bool View::isEnabled() const {
    return (_state & STATE_DISABLED)==0;
}
void View::setEnabled(bool isEnabled) {
    setState(VIEWSTATE(STATE_DISABLED, (isEnabled?0:STATE_DISABLED)));
}
bool View::isChecked() const {
    return (_state & STATE_CHECKED)!=0;
}
void View::setChecked(bool checked) {
    setState(VIEWSTATE(STATE_CHECKED, (checked?STATE_CHECKED:0)));
}
bool View::isSelected() const {
    return (_state & STATE_SELECTED)!=0;
}
void View::setSelected(bool selected) {
    setState(VIEWSTATE(STATE_SELECTED, (selected?STATE_SELECTED:0)));
}
bool View::isFocused() const {
    return (_state & STATE_FOCUSED)!=0;
}
void View::setFocused(bool focused) {
    setState(VIEWSTATE(STATE_FOCUSED, (focused?STATE_FOCUSED:0)));
}
bool View::isErrored() const {
    return (_state & STATE_ERRORED)!=0;
}
void View::setErrored(bool errored) {
    setState(VIEWSTATE(STATE_ERRORED, (errored?STATE_ERRORED:0)));
}



void View::setState(VIEWSTATE newstate) {
    uint8_t oldval = _state;
    uint8_t newval = (_state & ~newstate.mask) | newstate.state;
    if (oldval != newval) {
        _state = newval;
        onStateChanged(VIEWSTATE(oldval^newval, newval));
        setNeedsFullRedraw();
        if (_subviewsInheritState) {
            for (auto& subview : _subviews) {
                subview->setState(newstate);
            }
        }
    }
}
void View::onStateChanged(VIEWSTATE changes) {
    if (_dynamicStyles) {
        for (auto& it : *_dynamicStyles) {
            auto val = it.second;
            val = resolveQualifiedStyle(val);
            applySingleStyle(it.first, *val);
        }
    }
}

void View::addDecorOp(RenderOp* renderOp) {
    addRenderOpToList(renderOp, false, _renderListDecor);
}

void View::removeDecorOp(RenderOp* renderOp) {
    removeRenderOpFromList(renderOp, _renderListDecor);
}

float View::getElevation() const {
    return _elevation;
}
void View::setElevation(float elevation, bool animate/*=false*/) {
    if (_window && animate) {
        float oldElevation = _elevation;
        Animation::start(this, 100, [=](float val) {
            setElevation(oldElevation + (elevation-oldElevation) * val, false);
        });
        return;
    }
    if (elevation != _elevation) {
        _elevation = elevation;
        if (elevation>0) {
            if (!_shadowOp) {
                _shadowOp = new ShadowRenderOp();
                _shadowOp->setRect(_rect);
                if (_parent) {
                    _parent->addRenderOp(_shadowOp);
                }
            }
            _shadowOp->setSigma(elevation);
        } else {
            if (_shadowOp) {
                if (_parent) {
                    _parent->removeRenderOp(_shadowOp);
                }
                _shadowOp = nullptr;
            }
        }
        setNeedsFullRedraw();
    }
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
    //renderOp->setAlpha(_effectiveAlpha);

    if (!list) {
        list = new RenderList();
    }
    list->addRenderOp(renderOp, atFront);
    
    // If list is already attached to surface and being drawn, add to surface immediately
    if (_surface && list->_renderListsIndex>=0) {
        _surface->addRenderOp(renderOp);
    }
}

void View::removeRenderOpFromList(RenderOp* renderOp, sp<RenderList>& list) {

    assert(renderOp->_view);
    if (_surface && list->_renderListsIndex>=0) {
        _surface->detachRenderListOp(renderOp);
    }
    renderOp->_view = NULL;
    list->removeRenderOp(renderOp);
    if (!list->_ops.size()) {
        if (_surface) {
            _surface->detachRenderList(list);
        }
        list = NULL;
    }
}


RECT View::getOwnRect() {
	return RECT(0, 0, _rect.size.width, _rect.size.height);
}
RECT View::getSurfaceRect() {
    if (_parent) {
        return RECT(_surfaceOrigin.x - _parent->_contentOffsetAccum.x,
                    _surfaceOrigin.y - _parent->_contentOffsetAccum.y,
                    _rect.size.width,
                    _rect.size.height);

    }
    return RECT(_surfaceOrigin.x,
                _surfaceOrigin.y,
                _rect.size.width,
                _rect.size.height);
}
RECT View::getOwnRectPadded() {
    return RECT(_padding.left,_padding.top, _rect.size.width-(_padding.left+_padding.right), _rect.size.height - (_padding.top+_padding.bottom));
}
RECT View::getVisibleRect() {
    return RECT(_contentOffset.x, _contentOffset.y, _rect.size.width, _rect.size.height);
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

void View::setContentOffset(POINT contentOffset, bool animated/*=false*/) {
    assert(!animated); // TODO: implement animation here
    contentOffset.x = floorf(contentOffset.x);
    contentOffset.y = floorf(contentOffset.y);
    POINT d = contentOffset - _contentOffset;
	if (d.isZero()) {
        return;
    }
    _contentOffset = contentOffset;
    adjustContentOffsetAccum(d);
    updateScrollbarVisibility();
    updateBackgroundRect();
	
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
        // Ensure renderloop keeps going if a fling is still going on
        if (_scrollVert._fling || _scrollHorz._fling) {
            _window->requestRedraw();
        }
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

void View::setDirectionalLockEnabled(bool enabled) {
    _directionalLockEnabled = enabled;
}
bool View::getDirectionalLockEnabled() const {
    return _directionalLockEnabled;
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
        hitPoint += hitView->_rect.origin;
		hitView = hitView->_parent;
        if (hitView) {
            hitPoint -= hitView->_contentOffset;
        }
	}

	return handleInputEvent(event) ? this : NULL;
}


bool View::handleInputEvent(INPUTEVENT* event) {

    bool retVal = false;
    
    if (isEnabled()) {

        // Material Design Ink Ripple Animation
        if (_inkOp) {
            if (event->type == INPUT_EVENT_DOWN) {
                POINT dc = event->ptLocal;
                dc.x -= _rect.size.width/2;
                dc.y -= _rect.size.height/2;
                _inkOp->setOrigin(dc);
                _inkOp->setAlpha(1);
                _inkOp->_rippleAnim = Animation::start(this, 500, [=](float val) {
                    _inkOp->setRadius(500*val);
                });
            }
            if (event->type == INPUT_EVENT_UP) {
                _inkOp->_fadeAnim = Animation::start(this, 250, [=](float val) {
                    _inkOp->setAlpha(1-val);
                });
            }
        }

        
        if (event->type == INPUT_EVENT_DOWN) {
            setPressed(true);
        }
        if (event->type == INPUT_EVENT_TAP_CANCEL || event->type==INPUT_EVENT_UP) {
            setPressed(false);
        }
        
        if (event->type == INPUT_EVENT_DRAG_START || event->type == INPUT_EVENT_DRAG_MOVE) {
            if (_directionalLockEnabled) {
                if (event->delta.y > event->delta.x) {
                    
                }
            }
        }
        
        if (event->type == INPUT_EVENT_DRAG_START) {
            bool v = canScrollVertically();
            bool h = canScrollHorizontally();
            if (v || h) {
                if (_directionalLockEnabled) {
                    bool vertScroll = fabsf(event->delta.y) >= fabsf(event->delta.x);
                    if (vertScroll) {
                        retVal = v;
                    } else {
                        retVal = h;
                    }
                } else {
                    retVal = v || h;
                }
            }
        }
        
        bool scrolled = _scrollVert.handleEvent(this, true, event);
        scrolled |= _scrollHorz.handleEvent(this, false, event);
        retVal |= scrolled;

        if (onInputEvent) {
            retVal = true;
        }
    }
    
    
    if (onClick) {
        retVal = true; // To prevent it being passed up the view tree
        if (event->type == INPUT_EVENT_TAP) {
            onClick();
        }
    }
	if (onInputEvent) {
        return onInputEvent(this, event);
	}


	return retVal;
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

void View::setOpaque(bool opaque) {
    if(_opaque != opaque) {
        _opaque = opaque;
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
        applyTranslate(translation.x, translation.y);
    } else {
        Animation::start(this, duration, [=](float val) {
            applyTranslate(translation.x*val, translation.y*val);
        });
    }
}
Animation* View::animateInFromBottom(float duration, InterpolateFunc interpolater/* = strongEaseOut*/) {
    return Animation::start(this, duration, [=](float val) {
        applyTranslate(0, _rect.size.height * (1-val));
    }, interpolater);
}
Animation* View::animateOutToBottom(float duration, InterpolateFunc interpolater/* = strongEaseIn*/) {
    return Animation::start(this, duration, [=](float val) {
        applyTranslate(0, _rect.size.height * (val));
    }, interpolater);
}


void View::applyMatrix(const MATRIX4& m, bool reset/*=false*/) {
    if (!_matrix) {
        _matrix = new MATRIX4(m);
    } else {
        if (reset) {
            *_matrix = m;
        } else {
            _matrix->operator*=(m);
        }
    }
    /*if (translation.x==0 && translation.y==0) {
        if (_matrix) {
            delete _matrix;
            _matrix = NULL;
        }
    }*/
    if (_ownsSurface) {
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
    sprintf(ach, "%lX:%s", (long)this, debugViewType().c_str());
    return string(ach);
}
void View::debugDumpTree(int depth) {
    

    string line;
    for (int i=0 ; i<depth ; i++) line.append("-");
    line.append(debugDescription());
    log_dbg(line.c_str());

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

