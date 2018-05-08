//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



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

/*static ALIGNSPEC ToLeftOf(View* view, float margin);
static ALIGNSPEC ToRightOf(View* view, float margin);
static ALIGNSPEC Above(View* view, float margin);
static ALIGNSPEC Below(View* view, float margin);*/



ALIGNSPEC::ALIGNSPEC(StyleValue* value, View* view) {
    assert(value->type == StyleValue::Type::String);
    string str = value->str;
    string type = stringExtractUpTo(str, "(", true);
    if (type.size() == 0) {
        type = str;
        str = "";
    } else {
        str = stringExtractUpTo(str, ")", true);
    }
    if (type=="center") *this=Center();
    else if (type=="centre") *this=Center();
    else if (type=="left") *this=Left();
    else if (type=="right") *this=Right();
    else if (type=="top") *this=Top();
    else if (type=="bottom") *this=Bottom();
    else if (type=="toLeftOf") *this=ALIGNSPEC(NULL, 1.0f,  -1.0f, 0);
    else if (type=="toRightOf") *this=ALIGNSPEC(NULL, 1.0f,  0.0f, 0);
    else if (type=="above") *this=ALIGNSPEC(NULL, 1.0f,  -1.0f, 0);
    else if (type=="below") *this=ALIGNSPEC(NULL, 1.0f,  0.0f, 0);
    else assert(false); // unknown alignspec
    
    if (str.size() > 0) {
        string anchorId = stringExtractUpTo(str, ",", true);
        if (anchorId.size()==0) {
            anchorId=str;
            str="";
        }
        anchor = view->getParent()->findViewById(anchorId);
        assert(anchor); // NB: anchor must be previously declared. TODO: remove this restriction
        stringTrim(str);
        if (str.size() > 0) {
            margin = stringParseDimension(str);
        }
    }
}

