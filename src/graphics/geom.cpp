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

void RECT::transform(const class MATRIX4 &m) {
    VECTOR4 tl = {origin.x, origin.y, 0, 1};
    VECTOR4 tr = {origin.x+size.width, origin.y, 0, 1};
    VECTOR4 bl = {origin.x, origin.y+size.height, 0, 1};
    VECTOR4 br = {origin.x+size.width, origin.y+size.height, 0, 1};
    tl = m.operator*(tl);
    tr = m.operator*(tr);
    bl = m.operator*(bl);
    br = m.operator*(br);
    origin.x = fmin(tl.x, bl.x);
    origin.y = fmin(tl.y, tr.y);
    size.width = fmax(tr.x, br.x) - origin.x;
    size.height = fmax(bl.y, br.y) - origin.y;
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
AFFINE_TRANSFORM AFFINE_TRANSFORM::makeRotate(float rad) {
    float s = sin(rad);
    float c = cos(rad);
    return AFFINE_TRANSFORM(c,-s,s,c,0,0);
}

