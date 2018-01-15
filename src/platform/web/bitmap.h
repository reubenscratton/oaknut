//
//  Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//

#pragma once
#include <oaknut.h>


class OSBitmap : public Bitmap {
public:
    val _img;
    val _buff;
    PIXELDATA _pixelData;
    bool _isPng; // TODO: remove, now we have _format
    std::function<void(Bitmap*)> _tmp;
    
    OSBitmap();
    OSBitmap(int width, int height, int format);
    OSBitmap(val img, bool isPng);
    ~OSBitmap();

    virtual void lock(PIXELDATA* pixelData, bool forWriting);
    virtual void unlock(PIXELDATA* pixelData, bool pixelsChanged);

    virtual void bind();    
};


