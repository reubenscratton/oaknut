#pragma once
#include <oaknut.h>


class OSBitmap : public Bitmap {
public:
    jobject _androidBitmap;

    OSBitmap(int width, int height, int format);
    OSBitmap(jobject androidBitmap);
    OSBitmap(GLuint textureId);
    ~OSBitmap();

    // Overrides
    virtual void lock(PIXELDATA* pixelData, bool forWriting);
    virtual void unlock(PIXELDATA* pixelData, bool pixelDataChanged);
    virtual void bind();
};

