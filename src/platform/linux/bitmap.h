//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

class BitmapLinux : public Bitmap {
public:

    Bitmap(GdkPixbuf* pixbuf);
    Bitmap(int width, int height, int format);
    ~Bitmap();
    
    // Overrides
    virtual void lock(PIXELDATA* pixelData, bool forWriting);
    virtual void unlock(PIXELDATA* pixelData, bool pixelDataChanged);
    virtual void bind();

    cairo_t* getCairo();

private:
    cairo_surface_t* _cairo_surface;
    cairo_t* _cairo;

};

