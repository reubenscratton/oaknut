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


ALIGNSPEC::ALIGNSPEC(StyleValue* value, View* view) {
    assert(value->type == StyleValue::Type::String);
    string str = value->str;
    string type = str.extractUpTo("(", true);
    if (type.length() == 0) {
        type = str;
        str = "";
    } else {
        str = str.extractUpTo(")", true);
    }
    if (type=="center") *this=Center();
    else if (type=="centre") *this=Center();
    else if (type=="left") *this=Left();
    else if (type=="right") *this=Right();
    else if (type=="top") *this=Top();
    else if (type=="bottom") *this=Bottom();
    else if (type=="toLeftOf") *this=ALIGNSPEC(NO_ANCHOR, 1.0f,  -1.0f, 0);
    else if (type=="toRightOf") *this=ALIGNSPEC(NO_ANCHOR, 1.0f,  0.0f, 0);
    else if (type=="above") *this=ALIGNSPEC(NO_ANCHOR, 1.0f,  -1.0f, 0);
    else if (type=="below") *this=ALIGNSPEC(NO_ANCHOR, 1.0f,  0.0f, 0);
    else assert(false); // unknown alignspec
    
    if (str.length() > 0) {
        if (anchor == NO_ANCHOR) {
            string anchorId = str.extractUpTo(",", true);
            if (anchorId.length()==0) {
                anchorId=str;
                str="";
            }
            anchor = view->getParent()->findViewById(anchorId);
            assert(anchor); // NB: anchor must be previously declared. TODO: remove this restriction
        }
        str.trim();
        if (str.length() > 0) {
            margin = StyleValue::parseDimension(str);
        }
    }
}

