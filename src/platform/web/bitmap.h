//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
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

class BitmapWeb : public Bitmap {
public:
    val _img;
    val _buff;
    PIXELDATA _pixelData;
    class CanvasWeb* _canvas;
    bool _isPng; // TODO: remove, now we have _format
    
    BitmapWeb();
    BitmapWeb(int width, int height, int format);
    BitmapWeb(CanvasWeb* canvas);
    BitmapWeb(val img, bool isPng);
    ~BitmapWeb();

    void lock(PIXELDATA* pixelData, bool forWriting) override;
    void unlock(PIXELDATA* pixelData, bool pixelsChanged) override;
    
    // ISerializeToVariant
    void fromVariant(const variant& v) override;
    void toVariant(variant& v) override;

};


