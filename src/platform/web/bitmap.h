//
//  Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//

#pragma once
#include <oaknut.h>


class Bitmap : public BitmapBase {
public:
    val _img;
    val _buff;
    PIXELDATA _pixelData;
    bool _isPng; // TODO: remove, now we have _format
    std::function<void(Bitmap*)> _tmp;
    
    Bitmap();
    Bitmap(int width, int height, int format);
    Bitmap(val img, bool isPng);
    ~Bitmap();

    virtual void lock(PIXELDATA* pixelData, bool forWriting);
    virtual void unlock(PIXELDATA* pixelData, bool pixelsChanged);

    virtual void bind();
    
    // ISerializable
    Bitmap(const VariantMap* map);
    virtual void writeSelf(VariantMap* map);

};


