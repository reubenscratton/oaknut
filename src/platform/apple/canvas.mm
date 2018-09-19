//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#include <oaknut.h>




static CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();

static CGColorRef rgba(unsigned int rgba) {
    CGFloat components[4];
    components[0] = ((rgba&0xff0000)>>16)/255.0;
    components[1] = ((rgba&0xff00)>>8)/255.0;
    components[2] = (rgba&0xff)/255.0;
    components[3] = ((rgba&0xff000000)>>24)/255.0;
    return CGColorCreate(rgb, components);
}

class ApplePath : public Path {
public:
    CGMutablePathRef _path;

    ApplePath() {
        _path = CGPathCreateMutable();
    }
    ~ApplePath() {
        CGPathRelease(_path);
    }
    void moveTo(POINT pt) {
        CGPathMoveToPoint(_path, nil, pt.x, pt.y);
    }
    void lineTo(POINT pt) {
        CGPathAddLineToPoint(_path, nil, pt.x, pt.y);
    }
    void curveTo(POINT ctrl1, POINT ctrl2, POINT pt) {
        CGPathAddCurveToPoint(_path, nil, ctrl1.x, ctrl1.y, ctrl2.x, ctrl2.y, pt.x, pt.y);
    }

};

class AppleCanvas : public Canvas {
public:
    ObjPtr<Bitmap> _bitmap;
    CGSize _size;
    CGColorRef _fillColor;
    CGColorRef _strokeColor;
    CGFloat _strokeWidth;
    const AffineTransform* _transform;


    Bitmap* getBitmap() {
        return _bitmap;
    }
    void resize(int width, int height) {
        _size.width = width;
        _size.height = height;
        _bitmap = new Bitmap(width, height, BITMAPFORMAT_RGBA32);
        _bitmap->_hasPremultipliedAlpha = true;
    }
    void clear(COLOR color) {
        CGContextSetBlendMode(_bitmap->_context, kCGBlendModeCopy);
        CGContextRef context = _bitmap->_context;
        CGContextSetFillColorWithColor(context, rgba(color));
        CGContextFillRect(context, CGRectMake(0,0,_size.width,_size.height));
        CGContextSetFillColorWithColor(context, _fillColor);
        CGContextSetLineCap(context, kCGLineCapRound);
        _bitmap->_needsUpload = true;
        CGContextSetBlendMode(_bitmap->_context, kCGBlendModeNormal);
    }
    void setFillColor(COLOR color) {
        _fillColor = rgba(color);
        CGContextSetFillColorWithColor(_bitmap->_context, _fillColor);
    }
    void setStrokeColor(COLOR color) {
       _strokeColor = rgba(color);
        CGContextSetStrokeColorWithColor(_bitmap->_context, _strokeColor);
    }
    void setStrokeWidth(float strokeWidth) {
        _strokeWidth = strokeWidth;
        CGContextSetLineWidth(_bitmap->_context, strokeWidth);
    }
    void setAffineTransform(AffineTransform* t) {
        _transform = t;
    }

    void drawRect(RECT rect) {
        CGRect cgrect = CGRectMake(rect.origin.x,rect.origin.y,rect.size.width,rect.size.height);
        if (_fillColor) {
            CGContextFillRect(_bitmap->_context, cgrect);
        }
        if (_strokeColor && _strokeWidth>0) {
            CGContextStrokeRect(_bitmap->_context, cgrect);
        }
        _bitmap->_needsUpload = true;
    }

    void drawOval(RECT rect) {
        CGRect cgrect = CGRectMake(rect.origin.x,rect.origin.y,rect.size.width,rect.size.height);
        if (_fillColor) {
            CGContextFillEllipseInRect(_bitmap->_context, cgrect);
        }
        if (_strokeColor && _strokeWidth>0) {
            CGContextStrokeEllipseInRect(_bitmap->_context, cgrect);
        }
        _bitmap->_needsUpload = true;
    }

    Path* createPath() {
        return new ApplePath();
    }

    void drawPath(Path* path) {
        ApplePath* applePath = (ApplePath*)path;
        CGPathRef cgpath = applePath->_path;
        if (_transform) {
            CGAffineTransform cgtransform;
            cgtransform.a = _transform->a;
            cgtransform.b = _transform->b;
            cgtransform.c = _transform->c;
            cgtransform.d = _transform->d;
            cgtransform.tx = _transform->tx;
            cgtransform.ty = _transform->ty;
            cgpath = CGPathCreateCopyByTransformingPath(cgpath, &cgtransform);
        }
        CGContextBeginPath(_bitmap->_context);
        CGContextAddPath(_bitmap->_context, cgpath);
        CGContextClosePath(_bitmap->_context);
        if (_fillColor) {
            CGContextFillPath(_bitmap->_context);
        }
        if (_strokeColor && _strokeWidth>0) {
            CGContextStrokePath(_bitmap->_context);
        }
        _bitmap->_needsUpload = true;
        if (_transform) {
            CGPathRelease(cgpath);
        }
    }
    
    virtual void drawBitmap(Bitmap* bitmap, const RECT& rectSrc, const RECT& rectDst) {
        CGRect cgrectBmp = CGRectMake(0,0,bitmap->_width,bitmap->_height);
        CGRect cgrectSrc = CGRectMake(rectSrc.origin.x,rectSrc.origin.y,rectSrc.size.width,rectSrc.size.height);
        CGRect cgrectDst = CGRectMake(rectDst.origin.x,rectDst.origin.y,rectDst.size.width,rectDst.size.height);
        CGImageRef image = CGBitmapContextCreateImage(bitmap->_context);
        
        // Transform source origin from Oaknut coords (top-left origin) to core graphics (bot-left origin)
        cgrectSrc.origin.y = cgrectBmp.size.height - (cgrectSrc.origin.y+cgrectSrc.size.height);
        
        // If we're only drawing a part of the source image then we need to transform the destination rect
        // to be as if we were drawing the whole source image and then clip to the bit we want
        if (!CGRectEqualToRect(cgrectBmp, cgrectSrc)) {
            // Get the transform that maps source rect to destination rect
            CGFloat sx = cgrectDst.size.width / rectSrc.size.width;
            CGFloat sy = cgrectDst.size.height / rectSrc.size.height;
            CGFloat tx = -cgrectSrc.origin.x * sx;
            CGFloat ty = -cgrectSrc.origin.y * sy;
            CGAffineTransform t = CGAffineTransformMake(sx, 0, 0, sy, tx, ty);
            // Apply the transform to the whole source image to get the virtual destination rect
            cgrectDst = CGRectApplyAffineTransform(cgrectBmp, t);
            // todo: clip
        }
        
        // CGContextDrawImage seems to ignore the Y-inversion on the source context, so we have to
        // undo the destination context's Y-inversion to avoid drawing upside-down.
        CGContextScaleCTM(_bitmap->_context, 1, -1);
        CGContextTranslateCTM(_bitmap->_context, 0, -_bitmap->_height);
        CGContextDrawImage(_bitmap->_context, cgrectDst, image);
        CGContextScaleCTM(_bitmap->_context, 1, -1);
        CGContextTranslateCTM(_bitmap->_context, 0, -_bitmap->_height);
    }


};

Canvas* Canvas::create() {
    return new AppleCanvas();
}



#endif
