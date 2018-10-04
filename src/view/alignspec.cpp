//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


ALIGNSPEC::ALIGNSPEC() {
}
ALIGNSPEC::ALIGNSPEC(View* anchor, float multiplierAnchor, float multiplierSelf, float margin) {
    this->anchor = anchor;
    this->multiplierAnchor = multiplierAnchor;
    this->multiplierSelf = multiplierSelf;
    this->margin = margin;
}

ALIGNSPEC ALIGNSPEC::None()   { return ALIGNSPEC(NO_ANCHOR, 0,0,0); }
ALIGNSPEC ALIGNSPEC::Left()   { return ALIGNSPEC(NULL, 0.0f, 0.0f, 0); }
ALIGNSPEC ALIGNSPEC::Center() { return ALIGNSPEC(NULL, 0.5f,-0.5f, 0); }
ALIGNSPEC ALIGNSPEC::Right()  { return ALIGNSPEC(NULL, 1.0f,-1.0f, 0); }
ALIGNSPEC ALIGNSPEC::Top()    { return ALIGNSPEC(NULL, 0.0f, 0.0f, 0); }
ALIGNSPEC ALIGNSPEC::Bottom() { return ALIGNSPEC(NULL, 1.0f,-1.0f, 0); }

ALIGNSPEC ALIGNSPEC::ToLeftOf(View* view, float margin) {
    return ALIGNSPEC(view, 1.0f,  -1.0f, -margin);
}
ALIGNSPEC ALIGNSPEC::ToRightOf(View* view, float margin) {
    return ALIGNSPEC(view, 1.0f,  0.0f, margin);
}
ALIGNSPEC ALIGNSPEC::Above(View* view, float margin) {
    return ALIGNSPEC(view, 1.0f,  -1.0f, -margin);
}
ALIGNSPEC ALIGNSPEC::Below(View* view, float margin) {
    return ALIGNSPEC(view, 1.0f,  0.0f, margin);
}


ALIGNSPEC::ALIGNSPEC(const StyleValue* value, View* view) {
    anchor = NULL;
    if (value->isNumeric()) {
        multiplierAnchor = 0;
        multiplierSelf = 0;
        margin = value->floatVal();
        return;
    }
    string str = value->stringVal();
    string type = str.tokenise("(");

    bool anchorMandatory = false;
    if (type=="center") *this=Center();
    else if (type=="centre") *this=Center();
    else if (type=="left") *this=Left();
    else if (type=="right") *this=Right();
    else if (type=="top") *this=Top();
    else if (type=="bottom") *this=Bottom();
    else if (type=="toLeftOf") {multiplierAnchor=1.0f; multiplierSelf=-1.0f; anchorMandatory=true; }
    else if (type=="toRightOf") {multiplierAnchor=1.0f; multiplierSelf=0.0f; anchorMandatory=true; }
    else if (type=="above") {multiplierAnchor=1.0f; multiplierSelf=-1.0f; anchorMandatory=true; }
    else if (type=="below") {multiplierAnchor=1.0f; multiplierSelf=0.0f; anchorMandatory=true; }
    else assert(false); // unknown alignspec type

    margin = 0;

    // Arguments: if only one argument then its either anchor or margin.
    if (str.length() > 0) {
        string anchorName;
        StringProcessor proc(str);
        StyleValue arg1, arg2;
        arg1.parse(proc, PARSEFLAG_IS_ARGUMENT);
        proc.skipWhitespace();
        if (proc.peek()==')') {
            if (arg1.isString()) {
                anchorName = arg1.stringVal();
            } else {
                margin  = arg1.floatVal();
            }
        } else {
            arg2.parse(proc, PARSEFLAG_IS_ARGUMENT);
            assert(arg1.isString()); // if two args provided, first must be anchor
            assert(arg2.isNumeric()); // and second must be margin
            anchorName = arg1.stringVal();
            margin = arg2.floatVal();
            assert(proc.peek()==')');
        }
        if (anchorName.length()) {
            anchor = view->getParent()->findViewById(anchorName);
            assert(anchor); // NB: anchor must be previously declared. TODO: remove this restriction
        }
    }
    
    // If an anchor is required but none was declared, implicitly anchor to previously-declared view
    if (anchorMandatory && !anchor) {
        int index = view->getParent()->indexOfSubview(view);
        assert(index>=1);
        anchor = view->getParent()->getSubview(index-1);
    }
}

float ALIGNSPEC::calc(float measuredSize, float containingOrigin, float containingSize) const {
    
    if (anchor == NO_ANCHOR) {
        return containingOrigin;
    }
    float val = containingOrigin + (multiplierAnchor * containingSize)
              + (multiplierSelf * measuredSize)
              + margin;
    return floorf(val);
    /*float anchorVal = 0;
    if (anchor == _parent) {
        anchorVal = isVertical ? _parent->_padding.top : _parent->_padding.left;
        anchorSize -= isVertical ? (anchor->_padding.top+anchor->_padding.bottom):  (anchor->_padding.left+anchor->_padding.right);
    } else {
        anchorVal = isVertical ? anchor->_rect.origin.y : anchor->_rect.origin.x;
    }
    }
    float val = 0;
    if (anchor) {
        val =
    } else {
        val = containingOrigin;
    }
    return val;*/
    
}

