//
// Copyright © 2019 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


LAYOUTSPEC::LAYOUTSPEC() {
}
LAYOUTSPEC::LAYOUTSPEC(View* view, enum ViewPropertyType propertyType, float multProp, float multContentSize, float constant) {
    this->view = view;
    this->propertyType = propertyType;
    this->multProp = multProp;
    this->multContentSize = multContentSize;
    this->constant = constant;
}


LAYOUTSPEC LAYOUTSPEC::Fill(float multParentSize, float constant) { return LAYOUTSPEC(nullptr, PropWidthOrHeight, multParentSize, 0.0f, constant); }
LAYOUTSPEC LAYOUTSPEC::Wrap() { return LAYOUTSPEC(nullptr, PropNone, 0.0f, 1.0f, 0); }
LAYOUTSPEC LAYOUTSPEC::Abs(float x) { return LAYOUTSPEC(nullptr, PropNone, 0.0f, 0.0f, x); }
LAYOUTSPEC LAYOUTSPEC::Match(View* sibling) { return LAYOUTSPEC(sibling, PropWidthOrHeight, 1.0f, 0.0f, 0); }
LAYOUTSPEC LAYOUTSPEC::Left() { return LAYOUTSPEC(nullptr, PropLeftOrTop, 0.0f, 0.0f, 0); }
LAYOUTSPEC LAYOUTSPEC::Top() { return LAYOUTSPEC(nullptr, PropLeftOrTop, 0.0f, 0.0f, 0); }
LAYOUTSPEC LAYOUTSPEC::Right()  { return LAYOUTSPEC(nullptr, PropRightOrBottom, 1.0f, 0.0f, 0); }
LAYOUTSPEC LAYOUTSPEC::Bottom()  { return LAYOUTSPEC(nullptr, PropRightOrBottom, 1.0f, 0.0f, 0); }
LAYOUTSPEC LAYOUTSPEC::ToLeftOf(View* view, float margin) { return LAYOUTSPEC(view, PropLeftOrTop, 1.0f,  0.0f, -margin); }
LAYOUTSPEC LAYOUTSPEC::ToRightOf(View* view, float margin) { return LAYOUTSPEC(view, PropRightOrBottom, 1.0f,  0.0f, -margin); }
LAYOUTSPEC LAYOUTSPEC::Above(View* view, float margin) { return LAYOUTSPEC(view, PropLeftOrTop, 1.0f,  0.0f, -margin); }
LAYOUTSPEC LAYOUTSPEC::Below(View* view, float margin) { return LAYOUTSPEC(view, PropRightOrBottom, 1.0f,  0.0f, -margin); }
LAYOUTSPEC LAYOUTSPEC::Aspect(View* view, float x) { return LAYOUTSPEC(view, PropHeightOrWidth, x, 0.0f, 0); }



LAYOUTSPEC LAYOUTSPEC::fromSizeStyle(const variant* value, class View* view) {
    
    // A 'size' style has been used in layout, therefore this LAYOUTSPEC will be relative to its opposite edge
    LAYOUTSPEC spec(nullptr, PropNone, 0,0,0);
    
    // Handle reference names
    if (value->isString()) {
        uint32_t o = 0;
        auto str = value->stringVal();
        auto firstChar = str.charAt(0);
        bool isID = (firstChar>='A'&&firstChar<='Z')
                 || (firstChar>='a'&&firstChar<='z')
                 || firstChar=='_';
        if (isID) {
            auto ref = str.readUpToOneOf(o, ".*+- \t\r\n");
            if (ref == "wrap") {
                spec.multContentSize = 1;
            }
            else if (ref == "fill") {
                spec.propertyType = PropWidthOrHeight;
                spec.multProp = 1;
            }
            else if (ref == "aspect") {
                spec.propertyType = PropHeightOrWidth;
                spec.multProp = 1;
            }
            else {
                spec.view = view->getParent()->findViewById(ref);
                spec.multProp = 1;
                assert(spec.view); // NB: ref view must be sibling.
            }

            // Eat multiplier delimiter char, if present
            str.skipSpacesAndTabs(o);
            auto delim = str.peekChar(o);
            if (delim == '.' || delim=='*') {
                o++;
            }
        }
            
        // Multiplier follows ref
        str.skipSpacesAndTabs(o);
        variant num = variant::parseNumber(str, o);
        auto m = num.measurementVal();
        if (m._unit == measurement::PC || spec.propertyType == PropHeightOrWidth) {
            spec.multProp = m.val();
        } else {
            spec.constant = m.val();
        }

        // Constant follows multiplier
        str.skipSpacesAndTabs(o);
        if (str.peekChar(o)) {
            if (spec.constant) { // if a second constant is given the first must have been the multiplier
                spec.multProp = spec.constant;
            }
            num = variant::parseNumber(str, o);
            spec.constant = num.floatVal();
        }
    }

    
    // Number/measurement : a %age means parent multiplier, other types of measurement (dp,sp,px) are constant
    else if (value->isMeasurement()) {
        auto m = value->measurementVal();
        if (m._unit == measurement::PC) {
            spec.propertyType = PropWidthOrHeight;
            spec.multProp = m.val();
        } else {
            spec.constant = m.val();
        }
    }
    else if (value->isNumeric()) {
        spec.constant = value->floatVal();
        if (PropHeightOrWidth == spec.propertyType) {
            spec.multProp = spec.constant;
            spec.constant = 0;
        }
    }
    else {
        assert(0); // invalid variant value for LAYOUTSPEC
    }

    return spec;
    
}


LAYOUTSPEC LAYOUTSPEC::fromAlignStyle(const variant* value, class View* view) {

    // An 'align' style has been used in layout, therefore this LAYOUTSPEC will be relative to a View
    LAYOUTSPEC spec(nullptr, PropLeftOrTop, 0,0,0);

    // A single number is a margin
    if (value->isNumeric()) {
        spec.constant = value->floatVal();
        return spec;
    }
    
    // The three components of an alignspec are separated by plus or minus signs.
    variant arg1, arg2, arg3;
    const string str = value->stringVal();
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
        spec.multContentSize = arg2.floatVal();
        spec.constant = arg3.floatVal();
    } else {
        // Two args present: first must be ref, second is either self multiplier OR constant margin, depending on type
        if (!arg2.isEmpty()) {
            ref = arg1.stringVal();
            assert(arg2.isNumeric());
            if (arg2.isMeasurement()) {
                auto m = arg2.measurementVal();
                if (m._unit == measurement::PC) {
                    spec.multContentSize = m.val();
                } else {
                    spec.constant = m.val();
                }
            } else {
                spec.constant = arg2.floatVal();
            }
        }
        // Single arg: ref if non-numeric, margin if numeric
        else {
            if (arg1.isNumeric()) {
                spec.constant = arg1.floatVal();
            } else {
                ref = arg1.stringVal();
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
        spec.propertyType = PropWidthOrHeight;
        spec.multProp = 0.5f;
        spec.multContentSize -=  0.5f;
    } else if (ref=="left" || ref=="top") {
    } else if (ref=="right" || ref=="bottom") {
        spec.propertyType = PropRightOrBottom;
        spec.multProp = 1.0f;
    } else if (ref=="toLeftOf" || ref=="above") {
        spec.multContentSize -= 1.0f;
        usePreviousSiblingAsDefaultAnchor=true;
    } else if (ref=="toRightOf" || ref=="below") {
        spec.multProp=1.0f;
        usePreviousSiblingAsDefaultAnchor=true;
    } else {
        spec.multProp=ref.asFloat();
    }

    if (anchorId.length()) {
        spec.view = view->getParent()->findViewById(anchorId);
        assert(spec.view); // NB: anchor must be previously declared. TODO: remove this restriction
    }
    
    // If an anchor is required but none was declared, implicitly anchor to previously-declared view
    if (usePreviousSiblingAsDefaultAnchor && !spec.view) {
        int index = view->getParent()->indexOfSubview(view);
        assert(index>=1);
        spec.view = view->getParent()->getSubview(index-1);
    }
    
    return spec;
}
/*
float ALIGNSPEC::calc(float measuredSize, float refOrigin, float refSize) const {
    
    if (anchor == NO_ANCHOR) {
        return refOrigin;
    }
    float val = refOrigin + (multiplierAnchor * refSize)
              + (multiplierSelf * measuredSize)
              + margin;
    return floorf(val);
    
}*/





