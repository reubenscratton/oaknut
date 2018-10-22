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


class LinuxCanvas : public Canvas {
public:
    sp<Bitmap> _bitmap;
    //CGSize _size;
    //CGColorRef _fillColor;
    //CGColorRef _strokeColor;
    //CGFloat _strokeWidth;
    //const AFFINE_TRANSFORM* _transform;

    Bitmap* getBitmap() {
        return _bitmap;
    }


    void resize(int width, int height) {
    //    _size = size;
    //_bitmap = new BitmapLinux(size.width, size.height, BITMAPFORMAT_RGBA32, NULL, 0);
    }

    void clear(COLOR color) {
        //CGContextSetBlendMode(_bitmap->_context, kCGBlendModeCopy);
        //CGContextRef context = _bitmap->_context;
        //CGContextSetFillColorWithColor(context, rgba(color));
        //CGContextFillRect(context, CGRectMake(0,0,_size.width,_size.height));
        //CGContextSetFillColorWithColor(context, _fillColor);
        //CGContextSetLineCap(context, kCGLineCapRound);
        _bitmap->_needsUpload = true;
        //CGContextSetBlendMode(_bitmap->_context, kCGBlendModeNormal);
    }

    void setFillColor(COLOR color) {
        //_fillColor = rgba(color);
        //CGContextSetFillColorWithColor(_bitmap->_context, _fillColor);
    }
    void setStrokeColor(COLOR color) {
        //_strokeColor = rgba(color);
        //CGContextSetStrokeColorWithColor(_bitmap->_context, _strokeColor);
    }
    void setStrokeWidth(float strokeWidth) {
        //_strokeWidth = strokeWidth;
        //CGContextSetLineWidth(_bitmap->_context, strokeWidth);
    }
    void setAffineTransform(AFFINE_TRANSFORM* t) {
        //_transform = t;
    }

    void drawRect(RECT rect) {
        /*CGRect cgrect = CGRectMake(rect.origin.x,rect.origin.y,rect.size.width,rect.size.height);
        if (_fillColor) {
            CGContextFillRect(_bitmap->_context, cgrect);
        }
        if (_strokeColor && _strokeWidth>0) {
            CGContextStrokeRect(_bitmap->_context, cgrect);
        }
        _bitmap->_needsUpload = true;*/
    }

    void drawOval(RECT rect) {
        /*CGRect cgrect = CGRectMake(rect.origin.x,rect.origin.y,rect.size.width,rect.size.height);
        if (_fillColor) {
            CGContextFillEllipseInRect(_bitmap->_context, cgrect);
        }
        if (_strokeColor && _strokeWidth>0) {
            CGContextStrokeEllipseInRect(_bitmap->_context, cgrect);
        }
        _bitmap->_needsUpload = true;*/
    }

    void drawPath(Path* ospath) {
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
    Path* createPath() {
        //CGMutablePathRef path = CGPathCreateMutable();
        // return path;
        return new LinuxPath();
    }

};




Canvas* Canvas::create() {
    return new LinuxCanvas();
}


#endif
