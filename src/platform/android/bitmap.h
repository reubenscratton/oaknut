//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


class Bitmap : public BitmapBase {
public:
    jobject _androidBitmap;

    Bitmap(int width, int height, int format);
    Bitmap(jobject androidBitmap);
    Bitmap(GLuint textureId);
    ~Bitmap();

    // Overrides
    virtual void lock(PIXELDATA* pixelData, bool forWriting);
    virtual void unlock(PIXELDATA* pixelData, bool pixelDataChanged);
    virtual void bind();
};

