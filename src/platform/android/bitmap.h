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

    Bitmap();
    Bitmap(int width, int height, int format);
    Bitmap(jobject androidBitmap);
    Bitmap(GLuint textureId);
    ~Bitmap();

    // Overrides
    void lock(PIXELDATA* pixelData, bool forWriting) override;
    void unlock(PIXELDATA* pixelData, bool pixelDataChanged) override;
    void bind() override;
    void fromVariant(const variant& v) override;
    void toVariant(variant& v) override;
};
