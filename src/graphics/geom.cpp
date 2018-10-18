//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



QUAD QUADFromRECT(const RECT& rect, uint32_t color) {
    QUAD q;
    float l = rect.left(), r=rect.right();
    float t = rect.top(), b=rect.bottom();
    uint8_t red = color&255;
    uint8_t blue = (color&0xff0000)>>16;
    color = (color & 0xFF00FF00) | blue | (red<<16); // swap red & blue. Might be iOS specific, not sure
    q.tl = {l, t, 0, 0, color};
    q.tr = {r, t, 1, 0, color};
    q.bl = {l, b, 0, 1, color};
    q.br = {r, b, 1, 1, color};
    
    /* This rotates by 90 deg
    q.tl = {r, t, 0, 0, color};
    q.tr = {r, b, 1, 0, color};
    q.bl = {l, t, 0, 1, color};
    q.br = {l, b, 1, 1, color};*/
    return q;
}

bool POINT::equals(POINT& pt) {
	return x==pt.x && y==pt.y;
}
bool POINT::operator==(const struct _POINT& pt) {
    return x==pt.x && y==pt.y;
}
bool POINT::operator!=(const struct _POINT& pt) {
    return !(x==pt.x && y==pt.y);
}
const struct _POINT& POINT::operator+=(const struct _POINT& d) {
    x+=d.x;
    y+=d.y;
    return *this;
}
const struct _POINT& POINT::operator-=(const struct _POINT& d) {
    x-=d.x;
    y-=d.y;
    return *this;
}


float RECT::midX() {
	return origin.x + size.width/2;
}
float RECT::midY() {
	return origin.y + size.height/2;
}

RECT::RECT() {
    origin.x = 0;
    origin.y = 0;
    size.width = 0;
    size.height = 0;
}
RECT::RECT(float x, float y, float width, float height) {
	origin.x = x;
	origin.y = y;
	size.width = width;
	size.height = height;
}

RECT RECT_union(const RECT&r1, const RECT& r2) {
    RECT r;
    r.origin.x = fminf(r1.origin.x, r2.origin.x);
    r.origin.y = fminf(r1.origin.y, r2.origin.y);
    r.size.width = fmaxf(r1.right(), r2.right()) - r.origin.x;
    r.size.height = fmaxf(r1.bottom(), r2.bottom()) - r.origin.y;
    return r;
}
typedef struct RECTA {
	float left;
	float top;
	float right;
	float bottom;
	RECTA(const RECT&r) {
		left = r.origin.x;
		top = r.origin.y;
		right = r.right();
		bottom = r.bottom();
	}
} RECTA;


bool QUAD::intersects(const struct _QUAD& r) const {
    return tl.x < r.tr.x &&
           r.tl.x < tr.x &&
           tl.y < r.bl.y &&
           r.tl.y < bl.y;
}
bool QUADintersectsRECT(const QUAD& q, const RECT& r) {
    return q.tl.x < r.right() &&
            r.origin.x < q.tr.x &&
    q.tl.y < r.bottom() &&
    r.origin.y < q.bl.y;
}

QUAD clipQuad(const QUAD& quad, const RECT& clip) {
    QUAD r = quad;
    if (clip.origin.x > r.tl.x) {
        r.tl.x = r.bl.x = clip.origin.x;
    }
    if (clip.right() < r.tr.x) {
        r.tr.x = r.br.x = clip.right();
    }
    if (clip.origin.y > r.tl.y) {
        r.tl.y = r.tr.y = clip.origin.y;
    }
    if (clip.bottom() < r.bl.y) {
        r.bl.y = r.br.y = clip.bottom();
    }
    return r;
}

bool RECT::intersects(const RECT& r) const {
    return origin.x < r.right() &&
           r.origin.x < right() &&
           origin.y < r.bottom() &&
           r.origin.y < bottom();
}
bool RECT::intersectWith(const RECT& r) {
    RECTA r1(*this), r2(r);
    if (r1.left < r2.right && r2.left < r1.right && r1.top < r2.bottom && r2.top < r1.bottom) {
        if (r1.left < r2.left) r1.left = r2.left;
        if (r1.top < r2.top) r1.top = r2.top;
        if (r1.right > r2.right) r1.right = r2.right;
        if (r1.bottom > r2.bottom) r1.bottom = r2.bottom;
        origin.x = r1.left;
        origin.y = r1.top;
        size.width = r1.right-r1.left;
        size.height = r1.bottom-r1.top;
        return true;
    }
    return false;
}


float RECT::left() const {
	return origin.x;
}
float RECT::right() const {
	return origin.x+size.width;
}
float RECT::top() const {
	return origin.y;
}
float RECT::bottom() const {
	return origin.y+size.height;
}

bool RECT::operator==(const RECT& r) {
	return origin.x==r.origin.x
		&& origin.y==r.origin.y
		&& size.width==r.size.width
		&& size.height==r.size.height;
}

bool RECT::isEmpty() {
	return size.width<=0 || size.height<=0;
}

POINT RECT::topLeft() const {
	return origin;
}
POINT RECT::topRight() const {
    return {origin.x+size.width, origin.y};
}
POINT RECT::bottomLeft() const {
    return {origin.x, origin.y+size.height};
;
}
POINT RECT::bottomRight() const {
    return {origin.x+size.width, origin.y+size.height};
}

bool RECT::contains(const RECT& r) const {
   return contains(r.origin) && contains(r.bottomRight());
}

bool RECT::contains(const POINT& p) const {
   return (p.x>=origin.x && p.x<(origin.x+size.width)) && (p.y>=origin.y && p.y<(origin.y+size.height));
}

bool RECT::equal(const RECT& r2) const {
	return origin.x==r2.origin.x
		&& origin.y==r2.origin.y
		&& size.width==r2.size.width
		&& size.height==r2.size.height;
}

void RECT::scale(float sx, float sy) {
	origin.x *= sx;
	origin.y *= sy;
	size.width *= sx;
	size.height *= sy;
}


float parseFloat(const char*& p) {
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

RECT RECTfromString(const string& str) {
	// "{{0, 0}, {100, 100}}"
	const char* p = str.data();
	RECT rect;
	rect.origin.x = parseFloat(p);
	rect.origin.y = parseFloat(p);
	rect.size.width = parseFloat(p);
	rect.size.height = parseFloat(p);
	return rect;
}


string RECT::toString() const {
	// "{{0, 0}, {100, 100}}"
	char ach[64];
	snprintf(ach, sizeof(ach), "{{%f, %f}, {%f, %f}}", origin.x, origin.y, size.width, size.height);
	return string(ach);
}


AffineTransform::AffineTransform() {
    AffineTransform(1,0,0,1,0,0);
}
AffineTransform::AffineTransform(float a, float b, float c, float d, float tx, float ty) {
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
    this->tx = tx;
    this->ty = ty;
}

POINT AffineTransform::applyToPoint(const POINT& point) {
    POINT p;
    p.x = (float)((double)a * point.x + (double)c * point.y + tx);
    p.y = (float)((double)b * point.x + (double)d * point.y + ty);
    return p;
}

AffineTransform AffineTransform::makeScale(float sx, float sy) {
    return AffineTransform(sx,0,0,sy,0,0);
}
AffineTransform AffineTransform::makeTranslate(float tx, float ty) {
    return AffineTransform(1,0,0,1,tx,ty);
}

