//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include "bitmap.h"
#include "canvas.h"


static int bytesPerPixelForFormat(int format) {
    switch (format) {
        case PIXELFORMAT_RGBA32: return 4;
        case PIXELFORMAT_BGRA32: return 4;
        case PIXELFORMAT_RGB24: return 3;
        case PIXELFORMAT_RGB565: return 2;
        case PIXELFORMAT_A8: return 1;
        default: assert(0);
    }
    return 0;
}




BitmapWeb::BitmapWeb() : Bitmap(), _img(val::null()), _buff(val::null()) {
}

BitmapWeb::BitmapWeb(CanvasWeb* canvas) : Bitmap(), _img(val::null()), _buff(val::null()), _canvas(canvas) {
    _format = PIXELFORMAT_RGBA32;
}


// Writeable bitmap constructor, creates a 2D pixel array accessible to both javascript and C++
BitmapWeb::BitmapWeb(int width, int height, int format) : Bitmap(width, height, format), _img(val::null()), _buff(val::null()) {
    _pixelData.stride = width*bytesPerPixelForFormat(format);
    _pixelData.cb = _pixelData.stride*height;
    _pixelData.data = malloc(_pixelData.cb);
    _buff = val(typed_memory_view((size_t)_pixelData.cb, (unsigned char*)_pixelData.data));
}

// Read-only bitmap constructor, wraps the Image created by an ImageRequest
BitmapWeb::BitmapWeb(val img)  : _img(img), _buff(val::null()) {
    _width = img["width"].as<int>();
    _height = img["height"].as<int>();
    _format = PIXELFORMAT_RGBA32;
}

BitmapWeb::~BitmapWeb() {
    if (_pixelData.data) {
        free(_pixelData.data);
    }
}

void BitmapWeb::lock(PIXELDATA* pixelData, bool forWriting) {
    // If bitmap wraps an HTMLImageElement (i.e. image has been downloaded) then
    // we have to convert it to a standard bitmap by creating an HTMLImageCanvas
    // and drawing the HTMLImageElement to it, then extracting the ImageData.
    if (!_img.isNull()) {
        val canvas = val::global("document").call<val>("createElement", val("canvas"));
        canvas.set("width", val(_width));
        canvas.set("height", val(_height));
        val ctxt = canvas.call<val>("getContext", val("2d"));
        ctxt.call<void>("drawImage", _img, val(0), val(0), val(_width), val(_height));
        val imgdata = ctxt.call<val>("getImageData", val(0), val(0), val(_width), val(_height));
        assert(_format == PIXELFORMAT_RGBA32);
        val imgdatabuff = imgdata["data"];
        _pixelData.cb = imgdatabuff["length"].as<int>();
        _pixelData.stride = _pixelData.cb / _height;
        _pixelData.data = malloc(_pixelData.cb);
        _buff = val(typed_memory_view((size_t)_pixelData.cb, (unsigned char*)_pixelData.data));
        _buff.call<void>("set", imgdatabuff);
        _img = val::null();
    }

    // If bitmap belongs to a Canvas (i.e. is really the HTMLCanvasElement's ImageData)
    if (_canvas) {
        val imgdata = _canvas->_ctxt.call<val>("getImageData", val(0), val(0), val(_width), val(_height));
        val imgdatabuff = imgdata["data"];
        _pixelData.cb = imgdatabuff["length"].as<int>();
        _pixelData.stride = _pixelData.cb / _height;
        _pixelData.data = malloc(_pixelData.cb);
        _buff = val(typed_memory_view((size_t)_pixelData.cb, (unsigned char*)_pixelData.data));
        _buff.call<void>("set", imgdatabuff);
    }

    if (!_pixelData.data) {
        app.warn("lock() called on bitmap with no data or image");
        return;
    }

    pixelData->data = _pixelData.data;
    pixelData->stride = _pixelData.stride;
    pixelData->cb = _pixelData.cb;
}

void BitmapWeb::unlock(PIXELDATA* pixelData, bool pixelsChanged) {
    if (_canvas) {
        if (pixelsChanged) {
            app.warn("TODO: implement canvas bitmap writeback");
            assert(0); // code needed here
        }
        free(_pixelData.data);
        _pixelData.data = NULL;
    }
    if (pixelsChanged && _texture) {
        texInvalidate();
    }
    // NB: Nothing happens here. Once an image has been lock()d into heap memory
    // there's no real point copying back to Javascript-only memory and then copying all
    // over again on the next lock() call. Better (I think) to just keep the
    // buffer hanging around until the object is freed.
}


void BitmapWeb::glTexImage2D(int width, int height) {
    _width = width;
    _height = height;
    // TODO: this really belongs in Renderer. Need an 'uploadTextureFromBitmap' method...
    val gl = val::global("gl");
    int format = (_format==PIXELFORMAT_A8) ? GL_ALPHA : GL_RGBA;
    int type = GL_UNSIGNED_BYTE;
    if (!_img.isNull()) {
        gl.call<void>("texImage2D", GL_TEXTURE_2D, 0, format, format, type, _img);
    } else if (!_buff.isNull()) {
        gl.call<void>("texImage2D", GL_TEXTURE_2D, 0, format, _width, _height, 0, format, type, _buff);
    } else if (_canvas) {
        if (_canvas->_hasChanged) {
            gl.call<void>("texImage2D", GL_TEXTURE_2D, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, _canvas->_canvas);
            _canvas->_hasChanged = false;
        }
    } else {
        app.log("glTexImage2D() called on data-less bitmap");
    }
}

// ISerializableToVariant
void BitmapWeb::fromVariant(const variant& v) {
    Bitmap::fromVariant(v);
    _pixelData.stride = _width*bytesPerPixelForFormat(_format);
    _pixelData.cb = _pixelData.stride*_height;
    _pixelData.data = malloc(_pixelData.cb);
    _buff = val(typed_memory_view((size_t)_pixelData.cb, (unsigned char*)_pixelData.data));
    auto& bb = v.bytearrayVal("bb");
    memcpy(_pixelData.data, bb.data(), _pixelData.cb);
}
void BitmapWeb::toVariant(variant& v) {
    Bitmap::toVariant(v);
    PIXELDATA pixelData;
    lock(&pixelData, false);
    v.set("bb", bytearray((uint8_t*)pixelData.data, pixelData.cb));
    unlock(&pixelData, false);
}

class BitmapDecodeTask : public Task {
public:
    BitmapDecodeTask(const void* data, int cb, std::function<void(Bitmap*)> callback) : Task([=]() {
        callback(_bitmap);
    }) {
        string str = base64_encode((const uint8_t*)data, cb);
        string sstr = "data:image/png;base64,"; // TODO: this API needs a MIMEtype
        sstr.append(str);
        
        // TODO: If Safari ever supports it, switch to createImageBitmap().
        
        EM_ASM_({
            var task=$0;
            var img = new Image();
            img.onload = function() {
                Runtime.dynCall('vii', $1, [task, gotSet(img)]);
            };
            img.src = Pointer_stringify($2);
        }, this, onImageLoad, sstr.data());
    }

    static void onImageLoad(BitmapDecodeTask* task, int imgGotIndex) {
        val img = val::global("gotGet")(imgGotIndex);
        if (!task->isCancelled()) {
            task->_bitmap = new BitmapWeb();
            task->_bitmap->retain(); // release() is after Task::complete().
            task->_bitmap->_width = img["width"].as<int>();
            task->_bitmap->_height = img["height"].as<int>();
            task->_bitmap->_format = PIXELFORMAT_RGBA32;
            if (task->_bitmap->_width<=0) { // image failed to decode
                app.warn("Bitmap failed to decode");
                task->_bitmap->_img = val::null();
            } else {
                task->_bitmap->_img = img;
            }
            task->complete();
            task->_bitmap->release();
            task->_bitmap = NULL;
        } else {
            task->complete();
        }
    }

    BitmapWeb* _bitmap;
};


Task* Bitmap::createFromData(const void* data, int cb, std::function<void(Bitmap*)> callback) {
    return new BitmapDecodeTask(data, cb, callback);
}

Bitmap* Bitmap::create(int width, int height, int format) {
    return new BitmapWeb(width, height, format);
}


#endif

