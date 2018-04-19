//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>



/**
 Path2D is not supported on mobile browsers, yet, hence we have to manually track Paths
 */
enum PathElementType {
    TypeMoveTo,
    TypeLineTo,
    TypeCurveTo,
};

class PathElement {
public:
    enum PathElementType type;
    PathElement(PathElementType type) {
        this->type = type;
    }
};
class PathElementMoveTo : public PathElement {
public:
    PathElementMoveTo(const POINT& pt) : PathElement(TypeMoveTo) {
        this->pt = pt;
    }
    POINT pt;
};
class PathElementLineTo : public PathElement {
public:
    PathElementLineTo(const POINT& pt) : PathElement(TypeLineTo) {
        this->pt = pt;
    }
    POINT pt;
};
class PathElementCurveTo : public PathElement {
public:
    PathElementCurveTo(const POINT& ctrl1, const POINT& ctrl2, const POINT& pt) : PathElement(TypeCurveTo) {
        this->ctrl1 = ctrl1;
        this->ctrl2 = ctrl2;
        this->pt = pt;
    }
    POINT ctrl1,ctrl2,pt;
};

class WebPath : public Path {
public:
    WebPath() {
    }
    ~WebPath() {
        for (auto it : _pathElements) {
            delete (PathElement*)it;
        }
    }
    void moveTo(POINT pt) {
        _pathElements.push_back(new PathElementMoveTo(pt));
    }
    void lineTo(POINT pt) {
        _pathElements.push_back(new PathElementLineTo(pt));
    }
    void curveTo(POINT ctrl1, POINT ctrl2, POINT pt) {
        _pathElements.push_back(new PathElementCurveTo(ctrl1, ctrl2, pt));
    }
    
    vector<PathElement*> _pathElements;
};

class WebCanvas : public Canvas, public Bitmap {
public:
    val _canvas;
    val _ctxt;
    float _strokeWidth;
    COLOUR _strokeColour;
    COLOUR _fillColour;
    bool _hasChanged;
    AffineTransform _transform;

    
    WebCanvas() : _canvas(val::null()), _ctxt(val::null()) {
        _format = BITMAPFORMAT_RGBA32;
        _canvas = val::global("document").call<val>("createElement", val("canvas"));
    }

    Bitmap* getBitmap() {
        return this; // for web at least, the canvas *is* a kind of bitmap...
    }

    void resize(int width, int height) {
        _width = width;
        _height = height;
        _canvas.set("width", width);
        _canvas.set("height", height);
        _ctxt = _canvas.call<val>("getContext", val("2d"));
        _hasChanged = true;
    }
    void clear(COLOUR colour) {
        _hasChanged = true;

    }
    void setStrokeWidth(float strokeWidth) {
        _strokeWidth = strokeWidth;
    }
    void setStrokeColour(COLOUR colour) {
        _strokeColour = colour;
    }
    void setFillColour(COLOUR colour) {
        _fillColour = colour;
    }
    string cssColourStr(COLOUR colour) {
        char ach[16];
        // CSS ordering of 32-bit hex string is RGBA
        colour = ((colour&0xFF000000)>>24) // i.e. move A to least significant byte
               | ((colour&0xFF0000)<<8)
               | ((colour&0xFF00)<<8)
               | ((colour&0xFF)<<8);
        sprintf(ach, "#%08X", colour);
        return string(ach);
    }
    void setAffineTransform(AffineTransform* t) {
        _transform = t ? (*t) : AffineTransform();
        _hasChanged = true;
    }
    void drawRect(RECT rect) {
        _ctxt.call<void>("fillRect", val(rect.left()), val(rect.top()), val(rect.size.width), val(rect.size.height));
        if (_strokeWidth > 0) {
            _ctxt.set("lineWidth", val(_strokeWidth));
            _ctxt.set("strokeStyle", val(cssColourStr(_strokeColour)));
            _ctxt.call<void>("strokeRect", val(rect.left()), val(rect.top()), val(rect.size.width), val(rect.size.height));
        }
        _hasChanged = true;
    }
    void drawOval(RECT rect) {
        _ctxt.call<void>("save");
        _ctxt.call<void>("beginPath");
        _ctxt.call<void>("ellipse", val(rect.midX()), val(rect.midY()), val(rect.size.width/2), val(rect.size.height/2), val(0), val(0),val(2*M_PI));
        _ctxt.set("lineWidth", val(_strokeWidth));
        _ctxt.set("strokeStyle", val(cssColourStr(_strokeColour)));
        _ctxt.call<void>("stroke");
        _ctxt.call<void>("restore");
        _hasChanged = true;
    }
    void drawPath(Path* apath) {
        WebPath* path = (WebPath*)apath;
        _ctxt.call<void>("save");
        _ctxt.call<void>("beginPath");
        _ctxt.set("lineCap", val("round"));
        _ctxt.set("lineJoin", val("round"));
        _ctxt.set("lineWidth", val(_strokeWidth));
        _ctxt.set("strokeStyle", val(cssColourStr(_strokeColour)));
        for (auto it : path->_pathElements) {
            switch (it->type) {
                case TypeMoveTo: {
                    PathElementMoveTo* moveTo = (PathElementMoveTo*)it;
                    POINT pt =  _transform.applyToPoint(moveTo->pt);
                    _ctxt.call<void>("moveTo", val(pt.x), val(pt.y));
                }
                    break;
                case TypeLineTo: {
                    PathElementLineTo* lineTo = (PathElementLineTo*)it;
                    POINT pt =  _transform.applyToPoint(lineTo->pt);
                    _ctxt.call<void>("lineTo", val(pt.x), val(pt.y));
                }                    break;
                case TypeCurveTo: {
                    PathElementCurveTo* curveTo = (PathElementCurveTo*)it;
                    POINT ctrl1 =  _transform.applyToPoint(curveTo->ctrl1);
                    POINT ctrl2 =  _transform.applyToPoint(curveTo->ctrl2);
                    POINT pt =  _transform.applyToPoint(curveTo->pt);
                    _ctxt.call<void>("bezierCurveTo", val(ctrl1.x), val(ctrl1.y), val(ctrl2.x), val(ctrl2.y), val(pt.x), val(pt.y));

                }
                    break;
            }
        }
        _ctxt.call<void>("stroke");
        _ctxt.call<void>("restore");
        _hasChanged = true;
    }

    virtual void drawBitmap(Bitmap* bitmap, const RECT& rectSrc, const RECT& rectDst) {
        _ctxt.call<void>("drawImage", bitmap->_img,
                         val(rectSrc.origin.x), val(rectSrc.origin.y), val(rectSrc.size.width), val(rectSrc.size.height),
                         val(rectDst.origin.x), val(rectDst.origin.y), val(rectDst.size.width), val(rectDst.size.height));
    }

    Path* createPath() {
        return new WebPath();
    }

    // Bitmap API
    virtual void lock(PIXELDATA* pixelData, bool forWriting) {
        assert(0); // Canvas should never be locked, you draw to them with the canvas API!
    }
    virtual void unlock(PIXELDATA* pixelData, bool pixelsChanged) {
        assert(0);
    }
    virtual void bind() {
        Bitmap::bind();
        if (_hasChanged) {
            val gl = val::global("gl");
            gl.call<void>("texImage2D", GL_TEXTURE_2D, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, _canvas);
            _hasChanged = false;
        }
    }


};

Canvas* Canvas::create() {
    return new WebCanvas();
}

#endif
