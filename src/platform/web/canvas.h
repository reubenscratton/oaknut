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
    COLOUR _strokeColour;
    COLOUR _fillColour;
    bool _hasChanged;
    AffineTransform _transform;

    
    WebCanvas();

    Bitmap* getBitmap();

    void resize(int width, int height);
    void clear(COLOUR colour);
    void setStrokeWidth(float strokeWidth);
    void setStrokeColour(COLOUR colour);
    void setFillColour(COLOUR colour);
    void setAffineTransform(AffineTransform* t);
    void drawRect(RECT rect);
    void drawOval(RECT rect);
    void drawPath(Path* apath);

    virtual void drawBitmap(Bitmap* bitmap, const RECT& rectSrc, const RECT& rectDst);
    Path* createPath();
};

#endif
