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

