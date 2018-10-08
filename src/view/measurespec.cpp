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

MEASURESPEC MEASURESPEC::None() { return MEASURESPEC(TypeNone, NULL, 0, 0); }
MEASURESPEC MEASURESPEC::Abs(float x) { return MEASURESPEC(TypeRelative, NULL, 0, x); }
MEASURESPEC MEASURESPEC::Fill() { return MEASURESPEC(TypeRelative, NULL, 1, 0); }
MEASURESPEC MEASURESPEC::Wrap() { return MEASURESPEC(TypeContent, NULL, 1, 0); }
MEASURESPEC MEASURESPEC::Aspect(float x) { return MEASURESPEC(TypeAspect, NULL,  x, 0); }

MEASURESPEC::MEASURESPEC(const StyleValue* value, View* view) {
    
    StyleValue argsval;
    type = TypeRelative;
    ref = NULL;

    // If the val is a string then it is a type & ref declaration
    if (value->isString()) {
        string str = value->stringVal();
        string ref = str.tokenise("(");
        if (ref == "wrap") {
            *this = Wrap();
        }
        else if (ref == "fill") {
            *this = Fill();
        }
        else if (ref == "aspect") {
            type = TypeAspect;
            mul = 0;
            con = 0;
            assert(str.length() > 0); // aspect must have supplementary vals
        } else {
            assert(false); // unknown measurespec. TODO: handle view refs
            *this =  Wrap();
        }

        // Arguments may be given in a bracketed subexpression
        if (str.length() > 0) {
            StringProcessor proc(str);
            argsval.parse(proc);
            assert(proc.peek()==')');
            value = &argsval;
        }
    }

    // Single argument : %age means multiplier, other types of measurement (dp,sp,px) are constant
    if (value->isMeasurement()) {
        auto m = value->measurementVal();
        if (m._unit == measurement::PC) {
            mul = m.val();
            con = 0;
        } else {
            mul = 0;
            con = m.val();
        }
    }
    else if (value->isNumeric()) {
        mul = 0;
        con = value->floatVal();
        if (TypeAspect == type) {
            mul = con;
            con = 0;
        }
    }
    
    // Array: both mul and constant parts are present
    else if (value->isArray()) {
        auto array = value->arrayVal();
        mul = array[0].floatVal();
        con = array[1].floatVal();
        return;
    }
    
    
}

float MEASURESPEC::calc(View* view, float parentSize, float otherSize, bool isVertical) const {
    if (TypeContent == type) {
        if (isVertical) {
            return view->_contentSize.height + view->_padding.top + view->_padding.bottom;
        } else {
            return view->_contentSize.width + view->_padding.left + view->_padding.right;
        }
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
