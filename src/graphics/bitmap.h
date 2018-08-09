//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#pragma once



/*
 * @ingroup graphics_group
 * \class Bitmap
 * Bitmap is a platform type, i.e. apps always instantiate them via one of the
 * Bitmap::createXXX() APIs, never with operator new().
 *
 * The Bitmap class does not directly own a block of RAM for its pixels, the
 * storage of pixel data is left to the platform layer. When application or
 * Oaknut code needs to access the pixel data it has to lock() and unlock() the
 * bitmap. This may or may not cause an expensive blit, it's up to the platform.
 * Only access pixel data when you really need to.
 *
 * Bitmap formats
 * ==============
 * Oaknut bitmaps have three core formats: RGBA32, RGB16, and A8. Apple platform
 * allows a fourth format, BGRA32, in order to avoid needless conversion.
 *
 * On the web the only format supported is RGBA32 so bitmap data in other formats
 * will be converted.
 *
 * Platform notes
 * ==============
 iOS: Two types of Bitmap: there is the CoreGraphics CGBitmapContext
 we all know and love, and then there is CoreVideo's CVImageBufferRef which
 has the amazingly useful property of being usable as texture data, i.e. we can
 directly write texture without having to glTexSubImage2D etc.
 
 Android: Only one type of bitmap, ye olde android.graphics.Bitmap which is
 generally speaking a pain to work with. Every Bitmap on android always has
 an associated android.graphics.Bitmap java object.
 
 */
class Bitmap; // defined in platform layer

/**
 Supported bitmap formats. Bitmaps in other formats must be converted by platform code
 into one of these (see Bitmap::createXXX()).
*/
#define BITMAPFORMAT_UNKNOWN 0
#define BITMAPFORMAT_RGBA32  1
#define BITMAPFORMAT_RGB565  2
#define BITMAPFORMAT_BGRA32  3   // iOS only
#define BITMAPFORMAT_A8      4   // Not supported in WebGL, convert to RGBA instead

typedef struct {
    void* data;
    int cb;
    int stride;
} PIXELDATA;

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif


class BitmapBase : public Object, public ISerializeToVariantMap {
public:
    int32_t _width;
    int32_t _height;
    int32_t _format;
    GLuint _texSampleMethod; // default GL_LINEAR
    GLuint _texTarget;       // default GL_TEXTURE_2D
    GLuint _textureId;
    bool _allocdTexData;
    bool _paramsValid;
    bool _needsUpload;

    BitmapBase(int width, int height, int format);
    // Constructor is protected, use Bitmap::createXXX() APIs to instantiate
protected:
    BitmapBase();
    ~BitmapBase();
public:

    // Accessing pixel data
    virtual void lock(PIXELDATA* pixelData, bool forWriting)=0;
    virtual void unlock(PIXELDATA* pixelData, bool pixelsChanged)=0;
    uint8_t* pixelAddress(PIXELDATA* pixelData, int x, int y);

    // Rendering
    virtual void bind();

    int getBytesPerPixel();
    GLenum getGlFormat();
    GLenum getGlInternalFormat();
    int getGlPixelType();
    bool hasAlpha();
    
    int sizeInBytes();
    Bitmap* convertToFormat(int newFormat);
    
    // ISerializeToVariantMap
    BitmapBase(const VariantMap& map);
    virtual void writeSelfToVariantMap(VariantMap& map);
    

    // Platform-specific instantiation
    static void createFromData(const void* data, int cb, std::function<void(Bitmap*)> callback);

};


