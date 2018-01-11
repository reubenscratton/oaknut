//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"


VERTEX VERTEX_Make(GLfloat x,
                   GLfloat y,
                   GLfloat s,
                   GLfloat t,
                   uint32_t colour) {
    VERTEX v;
    v.x = x;
    v.y = y;
    v.s = s;
    v.t = t;
    uint8_t r = colour&255;
    uint8_t b = (colour&0xff0000)>>16;
    v.colour = (colour & 0xFF00FF00) | b | (r<<16); // swap red & blue. Might be iOS specific, not sure
    return v;
}

QUAD QUADFromRECT(const RECT& rect, uint32_t colour) {
    QUAD q;
    q.tl = VERTEX_Make(rect.left(), rect.top(), 0, 0, colour);
    q.tr = VERTEX_Make(rect.right(), rect.top(), 1, 0, colour);
    q.bl = VERTEX_Make(rect.left(), rect.bottom(), 0, 1, colour);
    q.br = VERTEX_Make(rect.right(), rect.bottom(), 1, 1, colour);
    return q;
}

POINT POINT_Make(float x, float y) {
	POINT p;
	p.x = x;
	p.y = y;
	return p;
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


SIZE SIZE_Make(float width, float height) {
	SIZE size = {width, height};
	return size;
}
SIZEI SIZEI_Make(int width, int height) {
    SIZEI size = {width, height};
    return size;
}

float RECT::midX() {
	return origin.x + size.width/2;
}
float RECT::midY() {
	return origin.y + size.height/2;
}

float RECT_left(const RECT& r) {
	return r.origin.x;
}
float RECT_right(const RECT& r) {
	return r.origin.x + r.size.width;
}
float RECT_top(const RECT& r) {
	return r.origin.y;
}
float RECT_bottom(const RECT& r) {
	return r.origin.y + r.size.height;
}

void RECT_inset(RECT& r, float dx, float dy) {
	r.origin.x += dx;
	r.origin.y += dy;
	r.size.width -= dx*2;
	r.size.height -= dy*2;
}

RECT RECT_Make(float x, float y, float width, float height) {
	RECT rect;
	rect.origin.x = x;
	rect.origin.y = y;
	rect.size.width = width;
	rect.size.height = height;
	return rect;
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
	return POINT_Make(origin.x+size.width, origin.y);
}
POINT RECT::bottomLeft() const {
	return POINT_Make(origin.x, origin.y+size.height);
;
}
POINT RECT::bottomRight() const {
	return POINT_Make(origin.x+size.width, origin.y+size.height);
}

bool RECT::contains(const RECT& r) {
   return contains(r.origin) && contains(r.bottomRight());
}

bool RECT::contains(const POINT& p) {
   return (p.x>=origin.x && p.x<(origin.x+size.width)) && (p.y>=origin.y && p.y<(origin.y+size.height));
}

bool RECT_contains(RECT& r, POINT& p) {
    return (p.x>=r.origin.x && p.x<(r.origin.x+r.size.width)) && (p.y>=r.origin.y && p.y<(r.origin.y+r.size.height));
}

bool RECT_equal(const RECT& r, const RECT& r2) {
	return r.origin.x==r2.origin.x
		&& r.origin.y==r2.origin.y
		&& r.size.width==r2.size.width
		&& r.size.height==r2.size.height;
}

void RECT_scale(RECT& r, float sx, float sy) {
	r.origin.x *= sx;
	r.origin.y *= sy;
	r.size.width *= sx;
	r.size.height *= sy;
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


string RECTtoString(const RECT& r) {
	// "{{0, 0}, {100, 100}}"
	char ach[64];
	snprintf(ach, sizeof(ach), "{{%f, %f}, {%f, %f}}", r.origin.x, r.origin.y, r.size.width, r.size.height);
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

