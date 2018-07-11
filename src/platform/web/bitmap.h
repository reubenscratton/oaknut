//
//  Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//

#pragma once
#include <oaknut.h>

/**
 Web Bitmaps have 3 underlying types:
  - backed by a HTMLImageElement (_img is valid)
  - backed by a byte buffer (_pixelData and _buff are valid);
  - backed by a Canvas (i.e. the ImageData in the Canvas' HTMLCanvasElement)
 
  Clients that need direct access to pixel data will use the lock() and unlock()
  APIs. If a bitmap is backed by a HTMLImageElement it will convert itself to
  a byte buffer type in lock(). This is obviously an expensive operation.
 */

class Bitmap : public BitmapBase {
public:
    val _img;
    val _buff;
    PIXELDATA _pixelData;
    class WebCanvas* _canvas;
    bool _isPng; // TODO: remove, now we have _format
    std::function<void(Bitmap*)> _tmp;
    
    Bitmap();
    Bitmap(int width, int height, int format);
    Bitmap(WebCanvas* canvas);
    Bitmap(val img, bool isPng);
    ~Bitmap();

    virtual void lock(PIXELDATA* pixelData, bool forWriting);
    virtual void unlock(PIXELDATA* pixelData, bool pixelsChanged);

    virtual void bind();
    
    // ISerializeToVariantMap
    Bitmap(const VariantMap& map);
    virtual void writeSelfToVariantMap(VariantMap& map);

};


