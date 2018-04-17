//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include "bitmap.h"


static int bytesPerPixelForFormat(int format) {
    switch (format) {
        case BITMAPFORMAT_RGBA32: return 4;
        case BITMAPFORMAT_BGRA32: return 4;
        case BITMAPFORMAT_RGB565: return 2;
        case BITMAPFORMAT_A8: return 1;
        default: assert(0);
    }
    return 0;
}

static int glformatForFormat(int format) {
    switch (format) {
        case BITMAPFORMAT_RGBA32: return GL_RGBA;
        case BITMAPFORMAT_BGRA32: return GL_RGBA;
        case BITMAPFORMAT_RGB565: return GL_RGB;
        case BITMAPFORMAT_A8: return GL_ALPHA;
        default: assert(0);
    }
    return 0;
}

static int gltypeForFormat(int format) {
    switch (format) {
        case BITMAPFORMAT_RGBA32: return GL_UNSIGNED_BYTE;
        case BITMAPFORMAT_BGRA32: return GL_UNSIGNED_BYTE;
        case BITMAPFORMAT_RGB565: return GL_UNSIGNED_SHORT;
        case BITMAPFORMAT_A8: return GL_UNSIGNED_BYTE;
        default: assert(0);
    }
    return 0;
}




Bitmap::Bitmap() : _img(val::null()), _buff(val::null()) {
}

// Writeable bitmap constructor, creates a 2D pixel array accessible to both javascript and C++
Bitmap::Bitmap(int width, int height, int format) : BitmapBase(width, height, format), _img(val::null()), _buff(val::null()) {
    _pixelData.stride = width*bytesPerPixelForFormat(format);
    _pixelData.cb = _pixelData.stride*height;
    _pixelData.data = malloc(_pixelData.cb);
    _buff = val(typed_memory_view((size_t)_pixelData.cb, (unsigned char*)_pixelData.data));
}

// Read-only bitmap constructor, wraps the Image created by an ImageRequest
Bitmap::Bitmap(val img, bool isPng)  : _img(img), _isPng(isPng), _buff(val::null()) {
    _width = img["width"].as<int>();
    _height = img["height"].as<int>();
    _format = BITMAPFORMAT_RGBA32;
}
Bitmap::~Bitmap() {
    if (_pixelData.data) {
        free(_pixelData.data);
    }
}

void Bitmap::lock(PIXELDATA* pixelData, bool forWriting) {
    // If we have an Image then we have to create a Canvas, draw the Image to it, and extract the ImageData.
    if (!_img.isNull()) {
        val canvas = val::global("document").call<val>("createElement", val("canvas"));
        canvas.set("width", val(_width));
        canvas.set("height", val(_height));
        val ctxt = canvas.call<val>("getContext", val("2d"));
        ctxt.call<void>("drawImage", _img, val(0), val(0), val(_width), val(_height));
        val imgdata = ctxt.call<val>("getImageData", val(0), val(0), val(_width), val(_height));
        assert(_format == BITMAPFORMAT_RGBA32);
        val imgdatabuff = imgdata["data"];
        _pixelData.cb = imgdatabuff["length"].as<int>();
        _pixelData.stride = _pixelData.cb / _height;
        _pixelData.data = malloc(_pixelData.cb);
        _buff = val(typed_memory_view((size_t)_pixelData.cb, (unsigned char*)_pixelData.data));
        _buff.call<void>("set", imgdatabuff);
        _img = val::null();
    }
    
    if (!_pixelData.data) {
        app.log("Warning: lock() called on bitmap with no data or image");
        return;
    }

    pixelData->data = _pixelData.data;
    pixelData->stride = _pixelData.stride;
    pixelData->cb = _pixelData.cb;
}

void Bitmap::unlock(PIXELDATA* pixelData, bool pixelsChanged) {
    // NB: Nothing happens here. Once an image has been lock()d into heap memory
    // there's no real point copying back to Javascript-only memory and then copying all
    // over again on the next lock() call. Better (I think) to just keep the
    // buffer hanging around until the object is freed.
}

void Bitmap::bind() {
    BitmapBase::bind();
    val gl = val::global("gl");
    int format = _isPng ? GL_RGBA : GL_RGB;
    format = glformatForFormat(_format);
    int type = gltypeForFormat(_format);
    if (!_img.isNull()) {
        gl.call<void>("texImage2D", GL_TEXTURE_2D, 0, format, format, type, _img);
    } else if (!_buff.isNull()) {
        gl.call<void>("texImage2D", GL_TEXTURE_2D, 0, format, _width, _height, 0, format, type, _buff);
    } else {
        app.log("bind() called on data-less bitmap");
    }
}




static const char basis_64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


string base64_encode(const char* input, size_t len) {
    int i;
    
    //size_t input_length = data.size();
    size_t output_length = ((len + 2) / 3 * 4) + 1;
    
    char* pp = (char*)malloc(output_length+1);
    char* p = pp;
    
    for (i = 0; i < len - 2; i += 3) {
        *p++ = basis_64[(input[i] >> 2) & 0x3F];
        *p++ = basis_64[((input[i] & 0x3) << 4) | ((int) (input[i + 1] & 0xF0) >> 4)];
        *p++ = basis_64[((input[i + 1] & 0xF) << 2) | ((int) (input[i + 2] & 0xC0) >> 6)];
        *p++ = basis_64[input[i + 2] & 0x3F];
    }
    if (i < len) {
        *p++ = basis_64[(input[i] >> 2) & 0x3F];
        if (i == (len - 1)) {
            *p++ = basis_64[((input[i] & 0x3) << 4)];
            *p++ = '=';
        }
        else {
            *p++ = basis_64[((input[i] & 0x3) << 4) | ((int) (input[i + 1] & 0xF0) >> 4)];
            *p++ = basis_64[((input[i + 1] & 0xF) << 2)];
        }
        *p++ = '=';
    }
    
    *p++ = '\0';
    
    return  string(pp);
}

// ISerializable
Bitmap::Bitmap(const VariantMap* map) : BitmapBase(map), _img(val::null()), _buff(val::null())  {
    app.log("TODO! finish Bitmap serialization");
}
void Bitmap::writeSelf(VariantMap* map) {
    BitmapBase::writeSelf(map);
    
    PIXELDATA pixelData;
    lock(&pixelData, false);
    app.log("TODO! finish Bitmap serialization");
}

// Platform-specific
static void onImageLoadedFromData(Bitmap* bitmap) {
    bitmap->_width = bitmap->_img["width"].as<int>();
    bitmap->_height = bitmap->_img["height"].as<int>();
    bitmap->_format = BITMAPFORMAT_RGBA32;
    if (bitmap->_width<=0) { // image failed to decode
        app.log("Warning: bitmap failed to decode");
        bitmap->_img = val::null();
    }
    bitmap->_tmp(bitmap);
}
void BitmapBase::createFromData(const void* data, int cb, std::function<void(Bitmap*)> callback) {
    string str = base64_encode((const char*)data, cb);
    string sstr = "data:image/png;base64,";
    sstr.append(str);
    //app.log("bitmap is %s", sstr.data());

    Bitmap* bitmap = new Bitmap();
    bitmap->_tmp = callback;
    bitmap->_img = val::global("Image").new_();
    val got = val::global("GlobalObjectTracker");
    got.set(4, bitmap->_img);
    EM_ASM_({
        var bitmap=$0;
        var OnImageLoadedFromData=$2;
        var img = GlobalObjectTracker[4];
        img.onload = function() {
            Runtime.dynCall('vi', OnImageLoadedFromData, [bitmap]);
        };
        img.src = Pointer_stringify($1);
    }, bitmap, sstr.data(), onImageLoadedFromData);
}

#endif

