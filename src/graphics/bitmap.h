//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#pragma once

/*
 
 The ideal bitmap is one whose pixel data and texture data are one and the same
 block of RAM. If we want to read or write pixel data we should be able to do
 that without having to copy things around for no damn reason. Unfortunately
 things are seldom that simple.
 
 For this reason the Bitmap class does not directly own a block of RAM, the
 storage of pixel data is left to the platform layer. When application or
 Oaknut code needs to access the pixel data it has to lock() and unlock() the
 bitmap.
 
 On iOS there are two types of Bitmap: there is the CoreGraphics CGBitmapContext
 we all know and love, and then there is CoreVideo's CVImageBufferRef which
 has the amazingly useful property of being usable as texture data, i.e. we can
 directly write texture without having to glTexSubImage2D etc.
 
 On Android there is just ye olde android.graphics.Bitmap which is a pain in
 the arse to work with. Since we don't always need this object the
 Android osbitmap implementation is a proxy object that instantiates
 an android.graphics.Bitmap only when it's actually needed (eg for
 use with a canvas API)
 
 
 Relationship to Texture:
 I wonder if Bitmaps and Textures are so tightly interwoven that they shouldn't be
 the same class?
 
 Sometimes we have bitmaps without textures (e.g. font glyphs and anything loaded
 or created dynamically for insertion into an atlas) and sometimes we have Textures
 without any Bitmap (e.g. camera preview frame textures).
 
 Bitmap
  - Size : ALWAYS PRESENT
  - Format : ALWAYS PRESENT
  - OSBitmap : ALWAYS PRESENT
  - GLuint textureTarget : OPTIONAL
  - GLuint textureId : OPTIONAL
 

 */

/**
 Supported bitmap formats. Bitmaps in other formats must be converted by platform code
 into one of these (see Bitmap::createFromData_()).
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

class Bitmap : public Object, ISerializableKeyed {
public:
    int _width;
    int _height;
    int _format;
    GLuint _texSampleMethod; // default GL_LINEAR
    GLuint _texTarget;       // default GL_TEXTURE_2D
    GLuint _textureId;
    bool _allocdTexData;
    bool _paramsValid;
    bool _needsUpload;

    // Constructor is protected, use oakBitmapCreate...() APIs to instantiate
protected:
    Bitmap();
    Bitmap(int width, int height, int format);
    ~Bitmap();
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
    
    // ISerializableKeyed
    virtual void readSelfFromKeyValueMap(const KeyValueMap* map);
    virtual void writeSelfToKeyValueMap(KeyValueMap* map);

};

// Platform-specific
Bitmap* oakBitmapCreate(int awidth, int aheight, int format);
void oakBitmapCreateFromData(const void* data, int cb, std::function<void(Bitmap*)> callback);

