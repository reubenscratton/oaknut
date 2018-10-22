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


static float parseFloat(const char*& p) {
	// Scan until we find a digit
	while (!((*p>='0' && *p<='9') || *p=='.' || *p=='\0')) {
		p++;
	}
	// Parse the float value out of the string
	float f = atof(p);
	// Scan past the float
	while (!((*p>='0' && *p<='9') || *p=='.' || *p=='\0')) {
		p++;
	}
	return f;
}

RECT::RECT(const string& str) {
	// "{{0, 0}, {100, 100}}"
	const char* p = str.data();
	origin.x = parseFloat(p);
	origin.y = parseFloat(p);
	size.width = parseFloat(p);
	size.height = parseFloat(p);
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

