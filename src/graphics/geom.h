//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


#pragma pack(push)
#pragma pack(1)
struct VERTEX {
    GLfloat x,y;
    GLfloat s,t; // tried these as int16 but text went wonky
    uint32_t color;
    GLfloat unused[3]; // seems v wasteful. Worth it?
};


struct POINT {
	float x;
	float y;

    POINT() {}
    POINT(float ax, float ay) : x(ax), y(ay) {}

    bool isZero() const { return x==0.0f && y==0.0f; }
    const POINT operator+(const POINT& rhs) const { return {x+rhs.x, y+rhs.y}; }
    const POINT operator-(const POINT& rhs) const { return {x-rhs.x, y-rhs.y}; }

    bool equals(POINT& pt) const {
        return x==pt.x && y==pt.y;
    }
    bool operator==(const POINT& pt) const {
        return x==pt.x && y==pt.y;
    }
    bool operator!=(const POINT& pt) const {
        return !(x==pt.x && y==pt.y);
    }
    const POINT& operator+=(const POINT& d) {
        x+=d.x;
        y+=d.y;
        return *this;
    }
    const POINT& operator-=(const POINT& d) {
        x-=d.x;
        y-=d.y;
        return *this;
    }
};

struct POINTI {
    int x;
    int y;
    
    POINTI() {}
    POINTI(int ax, int ay) : x(ax), y(ay) {}
    bool isZero() const { return x==0 && y==0; }
    const POINTI operator+(const POINTI& rhs) const { return {x+rhs.x, y+rhs.y}; }
    const POINTI operator-(const POINTI& rhs) const { return {x-rhs.x, y-rhs.y}; }
    
    bool equals(POINTI& pt) const {
        return x==pt.x && y==pt.y;
    }
    bool operator==(const POINTI& pt) const {
        return x==pt.x && y==pt.y;
    }
    bool operator!=(const POINTI& pt) const {
        return !(x==pt.x && y==pt.y);
    }
    const POINTI& operator+=(const POINTI& d) {
        x+=d.x;
        y+=d.y;
        return *this;
    }
    const POINTI& operator-=(const POINTI& d) {
        x-=d.x;
        y-=d.y;
        return *this;
    }
};

struct SIZE {
	float width;
	float height;
    SIZE() {}
    SIZE(float w, float h) : width(w), height(h) {}
    bool operator==(const SIZE& r) {
        return width==r.width
            && height==r.height;
    }
    bool operator!=(const SIZE& r) {
        return !(width==r.width
                 && height==r.height);
    }
};

struct SIZEI {
    int width;
    int height;
    SIZEI() {}
    SIZEI(int w, int h) : width(w), height(h) {}
    bool operator==(const SIZEI& r) {
        return width==r.width
            && height==r.height;
    }
    bool operator!=(const SIZEI& r) {
        return !(width==r.width
                 && height==r.height);
    }
};

struct RECT {
	POINT origin;
	SIZE size;
    

    RECT() {
        origin.x = 0;
        origin.y = 0;
        size.width = 0;
        size.height = 0;
    }
    RECT(float x, float y, float width, float height) {
        origin.x = x;
        origin.y = y;
        size.width = width;
        size.height = height;
    }
    RECT(const string& str);

    float left() const { return origin.x; }
    float right() const { return origin.x+size.width; }
    float top() const { return origin.y; }
    float bottom() const { return origin.y+size.height; }
    float midX() const { return origin.x + size.width/2; }
    float midY() const { return origin.y + size.height/2; }
    POINT topLeft() const { return origin; }
    POINT topRight() const { return {origin.x+size.width, origin.y}; }
    POINT bottomLeft() const { return {origin.x, origin.y+size.height}; }
    POINT bottomRight() const { return {origin.x+size.width, origin.y+size.height}; }

    bool contains(const RECT& r) const {
        if (!contains(r.origin)) {
            return false;
        }
        auto br=r.bottomRight();
        return (br.x>=origin.x && br.x<=(origin.x+size.width)) && (br.y>=origin.y && br.y<=(origin.y+size.height));
        //contains(r.bottomRight());
    }
    bool contains(const POINT& p) const {
        return (p.x>=origin.x && p.x<(origin.x+size.width)) && (p.y>=origin.y && p.y<(origin.y+size.height));
    }
    bool equal(const RECT& r2) const {
        return origin.x==r2.origin.x
        && origin.y==r2.origin.y
        && size.width==r2.size.width
        && size.height==r2.size.height;
    }
    
#define valueInRange(value, min, max) ((value >= (min)) && (value < (max)))
    
    bool intersects(const RECT& r) const {
        /*bool xOverlap = valueInRange(origin.x, r.origin.x, r.origin.x + r.size.width) ||
                        valueInRange(r.origin.x, origin.x, origin.x+size.width);
        if (!xOverlap) return false;
        bool yOverlap = valueInRange(origin.y, r.origin.y, r.origin.y + r.size.height) ||
                        valueInRange(r.origin.y, origin.y, origin.y+size.height);
        return xOverlap && yOverlap;*/
        return origin.x < r.right() &&
        r.origin.x < right() &&
        origin.y < r.bottom() &&
        r.origin.y < bottom();
    }
    bool intersectWith(const RECT& r) {
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
        size.width = size.height = 0;
        return false;
    }

    bool isEmpty() const {
        return size.width<=0 || size.height<=0;
    }
    bool operator==(const RECT& r) {
        return origin.x==r.origin.x
            && origin.y==r.origin.y
            && size.width==r.size.width
            && size.height==r.size.height;
    }
    bool operator!=(const RECT& r) {
        return !(origin.x==r.origin.x
            && origin.y==r.origin.y
            && size.width==r.size.width
            && size.height==r.size.height);
    }

    inline RECT& inset(float dx, float dy) {
        origin.x += dx;
        origin.y += dy;
        size.width -= dx*2;
        size.height -= dy*2;
        return *this;
    }
    RECT unionWith(const RECT& r2) const {
        RECT r;
        r.origin.x = fminf(origin.x, r2.origin.x);
        r.origin.y = fminf(origin.y, r2.origin.y);
        r.size.width = fmaxf(right(), r2.right()) - r.origin.x;
        r.size.height = fmaxf(bottom(), r2.bottom()) - r.origin.y;
        return r;
    }
    void scale(float sx, float sy) {
        origin.x *= sx;
        origin.y *= sy;
        size.width *= sx;
        size.height *= sy;
    }
    
    RECT copyWithInsets(const struct EDGEINSETS& insets) const;
    RECT copyWithUninsets(const struct EDGEINSETS& insets) const;

    string toString() const;
	
    static const RECT zero();
};

struct QUAD {
    VERTEX tl;
    VERTEX tr;
    VERTEX bl;
    VERTEX br;
    
    QUAD(const RECT& rect, uint32_t color) {
        float l = rect.left(), r=rect.right();
        float t = rect.top(), b=rect.bottom();
        tl = {l, t, 0, 0, color};
        tr = {r, t, 1, 0, color};
        bl = {l, b, 0, 1, color};
        br = {r, b, 1, 1, color};
        /* This rotates by 90 deg
         tl = {r, t, 0, 0, color};
         tr = {r, b, 1, 0, color};
         bl = {l, t, 0, 1, color};
         br = {l, b, 1, 1, color};*/
    }

    
    bool intersects(const QUAD& r) const {
        return tl.x < r.tr.x &&
        r.tl.x < tr.x &&
        tl.y < r.bl.y &&
        r.tl.y < bl.y;
    }
    bool intersectsRECT(const RECT& r) const {
        return tl.x < r.right() &&
        r.origin.x < tr.x &&
        tl.y < r.bottom() &&
        r.origin.y < bl.y;
    }
    QUAD clip(const RECT& clip) const {
        QUAD r = *this;
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
    

};





struct EDGEINSETS {
	float left;
	float top;
	float right;
	float bottom;
	inline EDGEINSETS() {
	}
	inline EDGEINSETS(float left, float top, float right, float bottom) {
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
	}
	inline void applyToRect(RECT& rect) const {
		rect.origin.x += left;
		rect.origin.y += top;
		rect.size.width -= left+right;
		rect.size.height -= top+bottom;
	}
    inline void unapplyToRect(RECT& rect) const {
        rect.origin.x -= left;
        rect.origin.y -= top;
        rect.size.width += left+right;
        rect.size.height += top+bottom;
    }
    inline bool operator==(const EDGEINSETS& other) const {
        return left==other.left
            && top==other.top
            && right==other.right
            && bottom==other.bottom;
    }
    inline bool operator!=(const EDGEINSETS& other) const {
        return left!=other.left
            || top!=other.top
            || right!=other.right
            || bottom!=other.bottom;
    }
};


#define EDGEINSETS_Zero EDGEINSETS(0,0,0,0)
#pragma pack(pop)


class AFFINE_TRANSFORM {
public:
    float a, b, c, d;
    float tx, ty;
    
    AFFINE_TRANSFORM() {
        AFFINE_TRANSFORM(1,0,0,1,0,0);
    }
    AFFINE_TRANSFORM(float a, float b, float c, float d, float tx, float ty) {
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        this->tx = tx;
        this->ty = ty;
    }
    POINT applyToPoint(const POINT& point) const {
        POINT p;
        p.x = (float)((double)a * point.x + (double)c * point.y + tx);
        p.y = (float)((double)b * point.x + (double)d * point.y + ty);
        return p;
    }

    static AFFINE_TRANSFORM makeScale(float sx, float sy);
    static AFFINE_TRANSFORM makeTranslate(float tx, float ty);
};



