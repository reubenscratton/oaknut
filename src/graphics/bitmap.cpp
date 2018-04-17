//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


#include <oaknut.h>

BitmapBase::BitmapBase() {
    _texSampleMethod = GL_LINEAR;
    _texTarget = GL_TEXTURE_2D;
}

BitmapBase::BitmapBase(int awidth, int aheight, int format) : BitmapBase() {
    _width = awidth;
    _height = aheight;
    _format = format;
}

BitmapBase::BitmapBase(const VariantMap* map) {
    _width = map->getInt("w");
    _height = map->getInt("h");
    _format = map->getInt("f");
    /*PIXELDATA pixeldata;
    lock(&pixeldata, true);
    pixeldata.stride = map->getInt("s");
    ObjPtr<ByteBuffer> bb = map->getByteBuffer("d");
    pixeldata.data = bb->data;
    pixeldata.cb = bb->cb;
    app.log("here!");
    unlock(&pixeldata, true);*/
}


BitmapBase::~BitmapBase() {
	//app.log("~Bitmap %d x %d", _width, _height);
    if (_textureId) {
        check_gl(glDeleteTextures, 1, &_textureId);
    }
}

// ISerializable
void BitmapBase::writeSelf(VariantMap* map) {
    map->setInt("w", _width);
    map->setInt("h", _height);
    map->setInt("f", _format);
   /* PIXELDATA pixeldata;
    lock(&pixeldata, false);
    map->setInt("s", pixeldata.stride);
    ByteBuffer bb;
    bb.data = (uint8_t*)pixeldata.data;
    bb.cb = pixeldata.cb;
    map->setByteBuffer("d", &bb);
    bb.data = NULL;
    unlock(&pixeldata, false);*/
}


GLenum BitmapBase::getGlFormat() {
    switch (_format) {
        case BITMAPFORMAT_RGBA32: return GL_RGBA;
        case BITMAPFORMAT_BGRA32: return GL_BGRA;
        case BITMAPFORMAT_RGB565: return GL_RGB;
        case BITMAPFORMAT_A8:
#ifdef PLATFORM_LINUX   // TODO: 'red' vs 'alpha' is probably not a Linux thing, it's more likely to be GL vs GL ES
            return GL_RED;
#else
            return GL_ALPHA;
#endif

    }
    assert(0);
}
GLenum BitmapBase::getGlInternalFormat() {
    switch (_format) {
        case BITMAPFORMAT_RGBA32: return GL_RGBA;
        case BITMAPFORMAT_BGRA32: return GL_RGBA;
        case BITMAPFORMAT_RGB565: return GL_RGB;
        case BITMAPFORMAT_A8:
#ifdef PLATFORM_LINUX
            return GL_RED;
#else
            return GL_ALPHA;
#endif
    }
    assert(0);
}

int BitmapBase::getGlPixelType() {
    switch (_format) {
        case BITMAPFORMAT_RGBA32: return GL_UNSIGNED_BYTE;
        case BITMAPFORMAT_BGRA32: return GL_UNSIGNED_BYTE;
        case BITMAPFORMAT_RGB565: return GL_UNSIGNED_SHORT_5_6_5;
        case BITMAPFORMAT_A8: return GL_UNSIGNED_BYTE;
    }
    assert(0);
}

bool BitmapBase::hasAlpha() {
    return _format!=BITMAPFORMAT_RGB565;
}

int BitmapBase::getBytesPerPixel() {
    switch (_format) {
        case BITMAPFORMAT_RGBA32: return 4;
        case BITMAPFORMAT_BGRA32: return 4;
        case BITMAPFORMAT_RGB565: return 2;
        case BITMAPFORMAT_A8: return 1;
    }
    assert(0);
}

int BitmapBase::sizeInBytes() {
    return _width*_height*getBytesPerPixel();
}



uint8_t* BitmapBase::pixelAddress(PIXELDATA* pixelData, int x, int y) {
	return ((uint8_t*)pixelData->data) + y*pixelData->stride + x*getBytesPerPixel();
}



void BitmapBase::bind() {
    if (!_textureId) {
        check_gl(glGenTextures, 1, &_textureId);
    }
    check_gl(glBindTexture, _texTarget, _textureId);
    if (!_paramsValid) {
        check_gl(glTexParameterf, _texTarget, GL_TEXTURE_MIN_FILTER, _texSampleMethod);
        check_gl(glTexParameterf, _texTarget, GL_TEXTURE_MAG_FILTER, _texSampleMethod);
        check_gl(glTexParameteri, _texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        check_gl(glTexParameteri, _texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        _paramsValid = true;
    }
}



Bitmap* BitmapBase::convertToFormat(int newFormat) {
    PIXELDATA pixeldataSrc;
    lock(&pixeldataSrc, false);
    Bitmap* bitmap = new Bitmap(_width, _height, newFormat);
    PIXELDATA pixeldataDst;
    bitmap->lock(&pixeldataDst, true);
    if (newFormat == BITMAPFORMAT_A8) {
        if (_format == BITMAPFORMAT_RGBA32) {
            uint8_t* rowDst = (uint8_t*)pixeldataDst.data;
            uint8_t* rowSrc = (uint8_t*)pixeldataSrc.data;
            for (int y=0 ; y<_height ; y++) {
                for (int x=0 ; x<_width ; x++) {
                    rowDst[x] = rowSrc[x*4+3];
                }
                rowDst += pixeldataDst.stride;
                rowSrc += pixeldataSrc.stride;
            }
        } else {
            assert(0); // need to write the converter
        }
    } else {
        assert(0); // need to write the converter
    }
    bitmap->unlock(&pixeldataDst, true);
    unlock(&pixeldataSrc, false);
    return bitmap;
}



