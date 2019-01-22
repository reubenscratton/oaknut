//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


#include <oaknut.h>



Bitmap::Bitmap() {
}

Bitmap::Bitmap(int awidth, int aheight, int format) : Bitmap() {
    _width = awidth;
    _height = aheight;
    _format = format;
}



void Bitmap::texInvalidate() {
    if (_texture) {
        _texture->_needsUpload = true;
    }
}

// ISerializableToVariant
void Bitmap::fromVariant(const variant& v) {
    _width = v.intVal("w");
    _height = v.intVal("h");
    _format = v.intVal("f");
}

void Bitmap::toVariant(variant& v) {
    v.set("w", _width);
    v.set("h", _height);
    v.set("f", _format);
}

bool Bitmap::hasAlpha() {
    return _format!=BITMAPFORMAT_RGB565;
}
bool Bitmap::hasPremultipliedAlpha() {
    return _hasPremultipliedAlpha;
}

int Bitmap::getBytesPerPixel() {
    switch (_format) {
        case BITMAPFORMAT_RGBA32: return 4;
        case BITMAPFORMAT_BGRA32: return 4;
        case BITMAPFORMAT_RGB24: return 3;
        case BITMAPFORMAT_RGB565: return 2;
        case BITMAPFORMAT_A8: return 1;
    }
    assert(0);
}

int Bitmap::sizeInBytes() {
    return _width*_height*getBytesPerPixel();
}



uint8_t* Bitmap::pixelAddress(PIXELDATA* pixelData, int x, int y) {
	return ((uint8_t*)pixelData->data) + y*pixelData->stride + x*getBytesPerPixel();
}




Bitmap* Bitmap::convertToFormat(int newFormat) {
    PIXELDATA pixeldataSrc;
    lock(&pixeldataSrc, false);
    Bitmap* bitmap = Bitmap::create(_width, _height, newFormat);
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



