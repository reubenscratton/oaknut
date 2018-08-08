//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


#pragma pack(push)
#pragma pack(1)
typedef struct _VERTEX {
    GLfloat x,y;
    GLfloat s,t; // tried these as int16 but text went wonky
    uint32_t color;
    GLfloat unused[3]; // seems v wasteful. Worth it?
} VERTEX;

VERTEX VERTEX_Make(GLfloat x,
                   GLfloat y,
                   GLfloat s,
                   GLfloat t,
                   uint32_t color);

typedef struct _QUAD {
    VERTEX tl;
    VERTEX tr;
    VERTEX bl;
    VERTEX br;
    bool intersects(const struct _QUAD& r) const;
} QUAD;


typedef struct _POINT {
	float x;
	float y;
	bool equals(struct _POINT& pt);
    bool operator==(const struct _POINT& pt);
    bool operator!=(const struct _POINT& pt);
    const struct _POINT& operator+=(const struct _POINT& d);
    bool isZero() const { return x==0.0f && y==0.0f; }
    const struct _POINT operator+(const struct _POINT& rhs) const { return {x+rhs.x, y+rhs.y}; }
    const struct _POINT operator-(const struct _POINT& rhs) const { return {x-rhs.x, y-rhs.y}; }
} POINT;
POINT POINT_Make(float x, float y);

typedef struct _SIZE {
	float width;
	float height;
} SIZE;
SIZE SIZE_Make(float width, float height);

typedef struct _SIZEI {
    int width;
    int height;
} SIZEI;
SIZEI SIZEI_Make(int width, int height);

typedef struct RECT {
	POINT origin;
	SIZE size;
	float midX();
	float midY();
    
    RECT();
    RECT(float x, float y, float width, float height);

	
	bool contains(const POINT& pt) const;
	bool contains(const struct RECT& r) const;
    bool equal(const RECT& r) const;
	
	float left() const;
	float right() const;
	float top() const;
	float bottom() const;
	POINT topLeft() const;
	POINT topRight() const;
	POINT bottomLeft() const;
	POINT bottomRight() const;
	bool intersectWith(const RECT& r);
	bool intersects(const RECT& r) const;
	bool isEmpty();
	bool operator==(const RECT& r);
    inline RECT& inset(float dx, float dy) {
        origin.x += dx;
        origin.y += dy;
        size.width -= dx*2;
        size.height -= dy*2;
        return *this;
    }
    void scale(float sx, float sy);

    string toString() const;
	
} RECT;

RECT RECTfromString(const string& str);
RECT RECT_union(const RECT&r1, const RECT& r2);

#define RECT_Zero RECT(0,0,0,0)


bool QUADintersectsRECT(const QUAD& q, const RECT& r);


typedef struct _EDGEINSETS {
	float left;
	float top;
	float right;
	float bottom;
	inline _EDGEINSETS() {
	}
	inline _EDGEINSETS(float left, float top, float right, float bottom) {
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
	}
	inline void applyToRect(RECT& rect) {
		rect.origin.x += left;
		rect.origin.y += top;
		rect.size.width -= left+right;
		rect.size.height -= top+bottom;
	}
} EDGEINSETS;

QUAD QUADFromRECT(const RECT& rect, uint32_t color);
QUAD clipQuad(const QUAD& quad, const RECT& clip);

#define EDGEINSETS_Zero EDGEINSETS(0,0,0,0)
#pragma pack(pop)


class AffineTransform {
public:
    float a, b, c, d;
    float tx, ty;
    
    AffineTransform();
    AffineTransform(float a, float b, float c, float d, float tx, float ty);
    POINT applyToPoint(const POINT& point);
    
    static AffineTransform makeScale(float sx, float sy);
    static AffineTransform makeTranslate(float tx, float ty);
};



