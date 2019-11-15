//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


const RECT RECT::zero() {
    return {0,0,0,0};
}


static float parseFloat(const string& str, uint32_t& p) {
    str.readUpToOneOf(p, "0123456789.");
    return str.readNumber(p).asFloat();
}

RECT::RECT(const string& str) {
	// "{{0, 0}, {100, 100}}"
	uint32_t p = 0;
	origin.x = parseFloat(str, p);
	origin.y = parseFloat(str, p);
	size.width = parseFloat(str, p);
	size.height = parseFloat(str, p);
}

RECT RECT::copyWithInsets(const EDGEINSETS& insets) const {
    RECT rect = *this;
    insets.applyToRect(rect);
    return rect;
}
RECT RECT::copyWithUninsets(const struct EDGEINSETS& insets) const {
    RECT rect = *this;
    insets.unapplyToRect(rect);
    return rect;
}


string RECT::toString() const {
	// "{{0, 0}, {100, 100}}"
	char ach[64];
	snprintf(ach, sizeof(ach), "{{%f, %f}, {%f, %f}}", origin.x, origin.y, size.width, size.height);
	return string(ach);
}



AFFINE_TRANSFORM AFFINE_TRANSFORM::makeScale(float sx, float sy) {
    return AFFINE_TRANSFORM(sx,0,0,sy,0,0);
}
AFFINE_TRANSFORM AFFINE_TRANSFORM::makeTranslate(float tx, float ty) {
    return AFFINE_TRANSFORM(1,0,0,1,tx,ty);
}

