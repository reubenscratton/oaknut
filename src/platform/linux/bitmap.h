//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

class BitmapLinux : public Bitmap {
public:

    BitmapLinux(GdkPixbuf* pixbuf);
    BitmapLinux(int width, int height, int format);
    ~BitmapLinux();
    
    // Overrides
    virtual void lock(PIXELDATA* pixelData, bool forWriting);
    virtual void unlock(PIXELDATA* pixelData, bool pixelDataChanged);
    virtual void bind();

    cairo_t* getCairo();

private:
    cairo_surface_t* _cairo_surface;
    cairo_t* _cairo;
    GdkPixbuf* _pixbuf;

};

