//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB


class WebCanvas : public Canvas {
public:
    val _canvas;
    val _ctxt;
    ObjPtr<Bitmap> _bitmap;
    float _strokeWidth;
    COLOR _strokeColor;
    COLOR _fillColor;
    bool _hasChanged;
    AffineTransform _transform;

    
    WebCanvas();

    Bitmap* getBitmap();

    void resize(int width, int height);
    void clear(COLOR color);
    void setStrokeWidth(float strokeWidth);
    void setStrokeColor(COLOR color);
    void setFillColor(COLOR color);
    void setAffineTransform(AffineTransform* t);
    void drawRect(RECT rect);
    void drawOval(RECT rect);
    void drawPath(Path* apath);

    virtual void drawBitmap(Bitmap* bitmap, const RECT& rectSrc, const RECT& rectDst);
    Path* createPath();
};

#endif
