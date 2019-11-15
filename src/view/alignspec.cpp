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
ALIGNSPEC ALIGNSPEC::Abs(float x)   { return ALIGNSPEC(NULL, 0.0f, 0.0f, x); }
ALIGNSPEC ALIGNSPEC::Center() { return ALIGNSPEC(NULL, 0.5f,-0.5f, 0); }
ALIGNSPEC ALIGNSPEC::Right()  { return ALIGNSPEC(NULL, 1.0f,-1.0f, 0); }
ALIGNSPEC ALIGNSPEC::Top()    { return ALIGNSPEC(NULL, 0.0f, 0.0f, 0); }
ALIGNSPEC ALIGNSPEC::Bottom() { return ALIGNSPEC(NULL, 1.0f,-1.0f, 0); }

ALIGNSPEC ALIGNSPEC::ToLeftOf(View* view, float margin) {
    return ALIGNSPEC(view, 0.0f,  -1.0f, -margin);
}
ALIGNSPEC ALIGNSPEC::ToRightOf(View* view, float margin) {
    return ALIGNSPEC(view, 1.0f,  0.0f, margin);
}
ALIGNSPEC ALIGNSPEC::Above(View* view, float margin) {
    return ALIGNSPEC(view, 0.0f,  -1.0f, -margin);
}
ALIGNSPEC ALIGNSPEC::Below(View* view, float margin) {
    return ALIGNSPEC(view, 1.0f,  0.0f, margin);
}


ALIGNSPEC::ALIGNSPEC(const variant& value, View* view) {
    anchor = NULL;
    
    // A single number is a margin
    if (value.isNumeric()) {
        multiplierAnchor = 0;
        multiplierSelf = 0;
        margin = value.floatVal();
        return;
    }
    
    // The three components of an alignspec are separated by plus or minus signs.
    variant arg1, arg2, arg3;
    const string str = value.stringVal();
    uint32_t it = 0;
    string tmp = str.readUpToOneOf(it, "+-");
    arg1 = variant::parse(tmp, 0);
    if (it<str.lengthInBytes()) {
        char sign=str.readChar(it);
        tmp = str.readUpToOneOf(it, "+-");
        tmp.insert(0, &sign, 1);
        arg2 = variant::parse(tmp, 0);
        if (it<str.lengthInBytes()) {
            tmp = str.readToEndOfLine(it);
            arg3 = variant::parse(tmp, 0);
        }
    }
    
    // Determine which arg is which.
    string ref;
    // All three args present: no ambiguity
    if (!arg3.isEmpty()) {
        ref = arg1.stringVal();
        multiplierSelf = arg2.floatVal();
        margin = arg3.floatVal();
    } else {
        // Two args present: first must be ref, second is either self multiplier OR constant margin, depending on type
        if (!arg2.isEmpty()) {
            ref = arg1.stringVal();
            assert(arg2.isNumeric());
            if (arg2.isMeasurement()) {
                auto m = arg2.measurementVal();
                if (m._unit == measurement::PC) {
                    multiplierSelf = m.val();
                    margin = 0;
                } else {
                    multiplierSelf = 0;
                    margin = m.val();
                }
            } else {
                multiplierSelf = 0;
                margin = arg2.floatVal();
            }
        }
        // Single arg: ref if non-numeric, margin if numeric
        else {
            if (arg1.isNumeric()) {
                multiplierAnchor = 0;
                multiplierSelf = 0;
                margin = arg1.floatVal();
            } else {
                ref = arg1.stringVal();
                multiplierSelf = 0;
                margin = 0;
            }
        }
    }
    
    // The reference contains the anchor view ID
    string anchorId;
    auto dotPos = ref.find(".");
    if (dotPos < ref.lengthInBytes()) {
        anchorId = ref.substr(0, dotPos);
        ref.erase(0, dotPos+1);
    }
    
 
    bool usePreviousSiblingAsDefaultAnchor = false;
    if (ref=="center" || ref=="centre") {
        multiplierAnchor = 0.5f;
        multiplierSelf -=  0.5f;
    } else if (ref=="left" || ref=="top") {
    } else if (ref=="right" || ref=="bottom") {
        multiplierAnchor = 1.0f;
        multiplierSelf -=  1.0f;
    } else if (ref=="toLeftOf" || ref=="above") {
        multiplierAnchor=0.0f;
        multiplierSelf -= 1.0f;
        usePreviousSiblingAsDefaultAnchor=true;
    } else if (ref=="toRightOf" || ref=="below") {
        multiplierAnchor=1.0f;
        multiplierSelf=0.0f;
        usePreviousSiblingAsDefaultAnchor=true;
    } else {
        multiplierAnchor=ref.asFloat();
    }

    if (anchorId.length()) {
        anchor = view->getParent()->findViewById(anchorId);
        assert(anchor); // NB: anchor must be previously declared. TODO: remove this restriction
    }
    
    // If an anchor is required but none was declared, implicitly anchor to previously-declared view
    if (usePreviousSiblingAsDefaultAnchor && !anchor) {
        int index = view->getParent()->indexOfSubview(view);
        assert(index>=1);
        anchor = view->getParent()->getSubview(index-1);
    }
}

float ALIGNSPEC::calc(float measuredSize, float refOrigin, float refSize) const {
    
    if (anchor == NO_ANCHOR) {
        return refOrigin;
    }
    float val = refOrigin + (multiplierAnchor * refSize)
              + (multiplierSelf * measuredSize)
              + margin;
    return floorf(val);
    
}

