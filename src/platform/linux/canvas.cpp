//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_LINUX

#include <oaknut.h>




class LinuxPath : public Path {
public:

    void moveTo(POINT pt) {
        //CGPathMoveToPoint(path, nil, pt.x, pt.y);
    }
    void lineTo(POINT pt) {
        //CGPathAddLineToPoint(path, nil, pt.x, pt.y);
    }
    void curveTo(POINT ctrl1, POINT ctrl2, POINT pt) {
        //CGPathAddCurveToPoint(path, nil, ctrl1.x, ctrl1.y, ctrl2.x, ctrl2.y, pt.x, pt.y);
    }
};


class CanvasLinux : public Canvas {
public:
    sp<Bitmap> _bitmap;
    //CGSize _size;
    //CGColorRef _fillColor;
    //CGColorRef _strokeColor;
    //CGFloat _strokeWidth;
    //const AFFINE_TRANSFORM* _transform;


    void resize(int width, int height) override {
    //    _size = size;
        _bitmap = new BitmapLinux(width, height, PIXELFORMAT_RGBA32);
    }
    Bitmap* getBitmap() override {
        return _bitmap;
    }
    

    void clear(COLOR color) override {
        //CGContextSetBlendMode(_bitmap->_context, kCGBlendModeCopy);
        //CGContextRef context = _bitmap->_context;
        //CGContextSetFillColorWithColor(context, rgba(color));
        //CGContextFillRect(context, CGRectMake(0,0,_size.width,_size.height));
        //CGContextSetFillColorWithColor(context, _fillColor);
        //CGContextSetLineCap(context, kCGLineCapRound);
        _bitmap->texInvalidate();
        //CGContextSetBlendMode(_bitmap->_context, kCGBlendModeNormal);
    }

    void setFillColor(COLOR color) override {
        //_fillColor = rgba(color);
        //CGContextSetFillColorWithColor(_bitmap->_context, _fillColor);
    }
    void setStrokeColor(COLOR color) override {
        //_strokeColor = rgba(color);
        //CGContextSetStrokeColorWithColor(_bitmap->_context, _strokeColor);
    }
    void setStrokeWidth(float strokeWidth) override {
        //_strokeWidth = strokeWidth;
        //CGContextSetLineWidth(_bitmap->_context, strokeWidth);
    }
    void setAffineTransform(AFFINE_TRANSFORM* t) override {
        //_transform = t;
    }

    void drawRect(RECT rect) override {
        /*CGRect cgrect = CGRectMake(rect.origin.x,rect.origin.y,rect.size.width,rect.size.height);
        if (_fillColor) {
            CGContextFillRect(_bitmap->_context, cgrect);
        }
        if (_strokeColor && _strokeWidth>0) {
            CGContextStrokeRect(_bitmap->_context, cgrect);
        }
        _bitmap->_needsUpload = true;*/
    }

    void drawOval(RECT rect) override {
        /*CGRect cgrect = CGRectMake(rect.origin.x,rect.origin.y,rect.size.width,rect.size.height);
        if (_fillColor) {
            CGContextFillEllipseInRect(_bitmap->_context, cgrect);
        }
        if (_strokeColor && _strokeWidth>0) {
            CGContextStrokeEllipseInRect(_bitmap->_context, cgrect);
        }
        _bitmap->_needsUpload = true;*/
    }

    void drawPath(Path* ospath) override {
        /*CGPathRef path = (CGPathRef)ospath;
        if (_transform) {
            CGAffineTransform cgtransform;
            cgtransform.a = _transform->a;
            cgtransform.b = _transform->b;
            cgtransform.c = _transform->c;
            cgtransform.d = _transform->d;
            cgtransform.tx = _transform->tx;
            cgtransform.ty = _transform->ty;
            path = CGPathCreateCopyByTransformingPath(path, &cgtransform);
        }
        CGContextBeginPath(_bitmap->_context);
        CGContextAddPath(_bitmap->_context, path);
        CGContextClosePath(_bitmap->_context);
        if (_fillColor) {
            CGContextFillPath(_bitmap->_context);
        }
        if (_strokeColor && _strokeWidth>0) {
            CGContextStrokePath(_bitmap->_context);
        }
        _bitmap->_needsUpload = true;*/
    }
    void drawBitmap(Bitmap* bitmap, const RECT& rectSrc, const RECT& rectDst) override {
        
    }

    Path* createPath() override {
        //CGMutablePathRef path = CGPathCreateMutable();
        // return path;
        return new LinuxPath();
    }

};




Canvas* Canvas::create() {
    return new CanvasLinux();
}


#endif
