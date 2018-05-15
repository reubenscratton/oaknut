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

MEASURESPEC::MEASURESPEC(StyleValue* value) {
    if (value->type == StyleValue::Type::Int || value->type == StyleValue::Type::Double) {
        *this = Abs(value->getAsFloat());
        return;
    }
    assert(value->type == StyleValue::Type::String);
    if (value->str == "wrap_content") { *this = WrapContent(); return; }
    if (value->str == "fill_parent") { *this = FillParent(); return; }
    if (value->str == "match_parent") { *this = FillParent(); return; }
    if (stringStartsWith(value->str, "aspect(", true)) {
        *this = UseAspect(stringParseDouble(value->str));
        return;
    }
    assert(false); // unknown measurespec
    *this =  WrapContent();
}