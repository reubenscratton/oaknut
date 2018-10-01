//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



MEASURESPEC::MEASURESPEC(RefType refType, View* refView, float refSizeMultiplier, float abs) {
    this->refType = refType;
    this->refView = refView;
    this->refSizeMultiplier = refSizeMultiplier;
    this->abs = abs;
}

MEASURESPEC MEASURESPEC::None() { return MEASURESPEC(RefTypeNone, NULL, 0, 0); }
MEASURESPEC MEASURESPEC::Abs(float x) { return MEASURESPEC(RefType::RefTypeAbs, NULL, 0, x); }
MEASURESPEC MEASURESPEC::WrapContent() { return MEASURESPEC(RefTypeContent, NULL, 1, 0); }
MEASURESPEC MEASURESPEC::UseAspect(float x) { return MEASURESPEC(RefTypeAspect, NULL,  x, 0); }
MEASURESPEC MEASURESPEC::FillParent() { return MEASURESPEC(RefTypeView, NULL, 1, 0); }

MEASURESPEC::MEASURESPEC(const StyleValue* value) {
    if (value->isNumeric()) {
        *this = Abs(value->floatVal());
        return;
    }
    string str = value->stringVal();
    string type = str.tokenise("(");
    if (type == "wrap" || type=="wrap_content") {
        *this = WrapContent();
    }
    else if (type == "fill" || type=="fill_parent" || type=="match_parent") {
        *this = FillParent();
    }
    else if (type == "aspect") {
        *this = UseAspect(atof(str.data()));
        assert(str.length() > 0); // aspect must have supplementary vals
    } else {
        assert(false); // unknown measurespec
        *this =  WrapContent();
    }
    if (str.length() > 0) {
        StringProcessor proc(str);
        StyleValue mul;
        mul.parse(proc);
        refSizeMultiplier = mul.floatVal();
        proc.skipWhitespace();
        if (proc.peek()!=')') {
            StyleValue offset;
            offset.parse(proc);
            abs = offset.floatVal();
            proc.skipWhitespace();
        }
        assert(proc.peek()==')');
    }
}

float MEASURESPEC::calc(View* view, float parentSize, float otherSize, bool isVertical) const {
    if (RefTypeContent == refType) {
        if (isVertical) {
            return view->_contentSize.height + view->_padding.top + view->_padding.bottom;
        } else {
            return view->_contentSize.width + view->_padding.left + view->_padding.right;
        }
    } else if (RefTypeAbs == refType) {
        return abs;
    } else if (RefTypeView == refType) {
        float refWidth = (refView == NULL) ? parentSize : refView->_rect.size.width;
        return refWidth * refSizeMultiplier + abs;
    } else if (RefTypeAspect == refType) {
        return otherSize * refSizeMultiplier + abs;
    } else {
        assert(0);
    }

}
