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

ALIGNSPEC ALIGNSPEC::None()   { return ALIGNSPEC(NULL, 0,0,0); }
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
    margin = 0;
    string type;
    if (value->isArray()) {
        auto& a = value->arrayVal();
        type = a[0].stringVal();
        if (a.size()>=2) {
            if (a[1].isNumeric()) {
                margin = a[1].floatVal();
            } else if (a[1].isString()) {
                string anchorId = a[1].stringVal();
                anchor = view->getParent()->findViewById(anchorId);
                assert(anchor); // NB: anchor must be previously declared. TODO: remove this restriction
            }
            if (a.size()>=3) {
                assert(a[2].isNumeric());
                margin = a[2].floatVal();
            }
        }
    } else if (value->isNumeric()) {
        multiplierAnchor = 0;
        multiplierSelf = 0;
        margin = value->floatVal();
        return;
    } else {
        type = value->stringVal();
    }
    bool anchorMustBeSibling = false;
    if (type=="center") *this=Center();
    else if (type=="centre") *this=Center();
    else if (type=="left") *this=Left();
    else if (type=="right") *this=Right();
    else if (type=="top") *this=Top();
    else if (type=="bottom") *this=Bottom();
    else if (type=="toLeftOf") {multiplierAnchor=1.0f; multiplierSelf=-1.0f; anchorMustBeSibling=true; }
    else if (type=="toRightOf") {multiplierAnchor=1.0f; multiplierSelf=0.0f; anchorMustBeSibling=true; }
    else if (type=="above") {multiplierAnchor=1.0f; multiplierSelf=-1.0f; anchorMustBeSibling=true; }
    else if (type=="below") {multiplierAnchor=1.0f; multiplierSelf=0.0f; anchorMustBeSibling=true; }
    else assert(false); // unknown alignspec
    
    // Implicit anchoring to previously-declared view
    if (anchorMustBeSibling && !anchor) {
        int index = view->getParent()->indexOfSubview(view);
        assert(index>=1);
        anchor = view->getParent()->getSubview(index-1);
    }
}

