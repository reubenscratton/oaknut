//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



MEASURESPEC::MEASURESPEC(Type type, View* ref, float mul, float con) {
    this->type = type;
    this->ref = ref;
    this->mul = mul;
    this->con = con;
}

MEASURESPEC MEASURESPEC::Abs(float x) { return MEASURESPEC(TypeRelative, NULL, 0, x); }
MEASURESPEC MEASURESPEC::Fill() { return MEASURESPEC(TypeRelative, NULL, 1, 0); }
MEASURESPEC MEASURESPEC::Wrap() { return MEASURESPEC(TypeContent, NULL, 1, 0); }
MEASURESPEC MEASURESPEC::Aspect(float x) { return MEASURESPEC(TypeAspect, NULL,  x, 0); }
MEASURESPEC MEASURESPEC::Match(View* view, float con) { return MEASURESPEC(TypeRelative, view, 1, con); }


float MEASURESPEC::calcConstraint(float parentSize, float otherSize) const {
    if (TypeContent == type) {
        return parentSize;
    } else if (TypeRelative == type) {
        float retval = con;
        if (mul != 0.0f) {
            float refWidth = (ref == NULL) ? parentSize : ref->_rect.size.width;
            retval += refWidth * mul;
        }
        return  retval;
    } else if (TypeAspect == type) {
        return otherSize * mul + con;
    } else {
        assert(0);
    }
}


MEASURESPEC MEASURESPEC::fromStyle(const variant* value, View* view) {
    
    MEASURESPEC spec(TypeRelative, NULL, 0,0);
    
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
                spec.type = TypeContent;
                spec.mul = 1;
            }
            else if (ref == "fill") {
                spec.mul = 1;
            }
            else if (ref == "aspect") {
                spec.type = TypeAspect;
                spec.mul = 1;
            }
            else {
                spec.ref = view->getParent()->findViewById(ref);
                spec.mul = 1;
                assert(spec.ref); // NB: ref view must be sibling.
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
        if (m._unit == measurement::PC || spec.type == TypeAspect) {
            spec.mul = m.val();
        } else {
            spec.con = m.val();
        }

        // Constant follows multiplier
        str.skipSpacesAndTabs(o);
        if (str.peekChar(o)) {
            if (spec.con) { // if a second constant is given the first must have been the multiplier
                spec.mul = spec.con;
            }
            num = variant::parseNumber(str, o);
            spec.con = num.floatVal();
        }
    }

    
    // Number/measurement : a %age means parent multiplier, other types of measurement (dp,sp,px) are constant
    else if (value->isMeasurement()) {
        auto m = value->measurementVal();
        if (m._unit == measurement::PC) {
            spec.mul = m.val();
        } else {
            spec.con = m.val();
        }
    }
    else if (value->isNumeric()) {
        spec.mul = 0;
        spec.con = value->floatVal();
        if (TypeAspect == spec.type) {
            spec.mul = spec.con;
            spec.con = 0;
        }
    }
    else {
        assert(0); // invalid variant type for MEASURESPEC
    }

    return spec;
    
}

