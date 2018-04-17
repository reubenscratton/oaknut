#pragma once
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

