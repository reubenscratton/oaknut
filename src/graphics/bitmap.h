//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#pragma once




/**
 Supported bitmap formats. Bitmaps in other formats must be converted by platform code
 into one of these (see Bitmap::createXXX()).
*/
#define PIXELFORMAT_UNKNOWN 0
#define PIXELFORMAT_RGBA32  1
#define PIXELFORMAT_BGRA32  2   // iOS only
#define PIXELFORMAT_RGB24   3
#define PIXELFORMAT_RGB565  4
#define PIXELFORMAT_A8      5   // Not supported in WebGL, convert to RGBA instead

#if PLATFORM_APPLE
#define PIXELFORMAT_DEFAULT32 PIXELFORMAT_BGRA32
#else
#define PIXELFORMAT_DEFAULT32 PIXELFORMAT_RGBA32
#endif

typedef struct {
    void* data;
    int cb;
    int stride;
} PIXELDATA;



/**
 * @ingroup graphics_group
 * @class Bitmap
 * @brief The standard 2D array of color values.
 *
 * Bitmap is a platform type, i.e. apps must instantiate them via one of the
 * Bitmap::createXXX() APIs, never with operator new().
 *
 * The Bitmap class does not directly own a block of RAM for pixel data, the
 * storage of pixel data is left to the platform layer. When application or
 * Oaknut code needs to access the pixel data it has to lock() and unlock() the
 * bitmap. This may or may not cause an expensive blit, it's up to the platform.
 * Only access pixel data when you really need to.
 *
 * Bitmap formats
 * ==============
 * Oaknut bitmaps have three core formats: RGBA32, RGB16, and A8. Apple platforms
 * allows a fourth format, BGRA32, in order to avoid needless conversion.
 *
 * On the web the only format supported is RGBA32 so bitmap data in other formats
 * will be converted.
 *
 */
class Bitmap : public Object, public ISerializeToVariant {
public:
    sp<Texture> _texture;
    int32_t _width;
    int32_t _height;
    int32_t _format;
    bool _hasPremultipliedAlpha;
    bool _sampleNearest;

    // Constructor is protected, use Bitmap::createXXX() APIs to instantiate
protected:
    Bitmap(int width, int height, int format);
    Bitmap();

public:

    // Accessing pixel data
    virtual void lock(PIXELDATA* pixelData, bool forWriting)=0;
    virtual void unlock(PIXELDATA* pixelData, bool pixelsChanged)=0;
    uint8_t* pixelAddress(PIXELDATA* pixelData, int x, int y);

    int getBytesPerPixel() const;
    bool hasAlpha();
    bool hasPremultipliedAlpha();
    
    virtual uint32_t getRamCost() const override;
    Bitmap* convertToFormat(int newFormat);
    
    // ISerializeToVariant
    void fromVariant(const variant& v) override;
    void toVariant(variant& v) override;

    void texInvalidate();


    // Platform-specific instantiation
    static Bitmap* create(int width, int height, int format);
    static Bitmap* createFromData(const bytearray& data);

};


