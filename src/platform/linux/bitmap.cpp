//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_LINUX

#include <oaknut.h>


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

BitmapLinux::BitmapLinux(GdkPixbuf* pixbuf) : Bitmap() {
    _pixbuf = pixbuf;
    g_object_ref(_pixbuf);
    int nc = gdk_pixbuf_get_n_channels(pixbuf);
    int bpc = gdk_pixbuf_get_bits_per_sample(pixbuf);
    if (nc==4 && bpc==8) _format = BITMAPFORMAT_RGBA32;
    else assert(false); // todo: add format here
    _width = gdk_pixbuf_get_width(pixbuf);
    _height = gdk_pixbuf_get_height(pixbuf);
    int stride = gdk_pixbuf_get_rowstride(pixbuf);
    _cairo_surface = cairo_image_surface_create_for_data(gdk_pixbuf_get_pixels(pixbuf), CAIRO_FORMAT_ARGB32, _width, _height, stride);
    _needsUpload = true;

}
BitmapLinux::BitmapLinux(int width, int height, int format) : Bitmap(width, height, format) {
    cairo_format_t cairo_format;
    switch (format) {
        case BITMAPFORMAT_RGBA32: cairo_format = CAIRO_FORMAT_ARGB32; break;
        case BITMAPFORMAT_RGB565: cairo_format = CAIRO_FORMAT_RGB16_565; break;
        case BITMAPFORMAT_A8: cairo_format = CAIRO_FORMAT_A8; break;
        default: assert(false);
    }
    _cairo_surface = cairo_image_surface_create(cairo_format, width, height);
    _needsUpload = true;
}

BitmapLinux::~BitmapLinux() {
    if (_cairo_surface) {
        cairo_surface_destroy(_cairo_surface);
        _cairo_surface = NULL;
    }
    if (_textureId) {
         check_gl(glDeleteTextures, 1, &_textureId);
        _textureId = 0;
    }
    g_object_unref(_pixbuf);
}

void BitmapLinux::lock(PIXELDATA* pixelData, bool forWriting) {
    pixelData->data = cairo_image_surface_get_data(_cairo_surface);
    pixelData->stride = cairo_image_surface_get_stride(_cairo_surface);
    pixelData->cb = _height * pixelData->stride;
    assert(pixelData->data && pixelData->cb && pixelData->stride);
}



void BitmapLinux::unlock(PIXELDATA* pixelData, bool pixelDataChanged) {
    if (pixelDataChanged) {
        _needsUpload = true;
    }
}

void BitmapLinux::bind() {
    
    Bitmap::bind();

    // If bitmap data changed we may need to update texture data
    if (!_needsUpload) {
        return;
    }
    if (!_cairo_surface){return;}
    _needsUpload = false;
    
    
    // Slow path
    void* data = cairo_image_surface_get_data(_cairo_surface);
    if (!_allocdTexData) {
        _allocdTexData = true;
        check_gl(glTexImage2D, _texTarget, 0, getGlInternalFormat(),
                _width, _height, 0, getGlFormat(), getGlPixelType(), data);
    } else {
        check_gl(glTexSubImage2D, _texTarget, 0, 0, 0, _width, _height, getGlFormat(), getGlPixelType(),data);
    }

}

cairo_t* BitmapLinux::getCairo() {
    if (!_cairo) {
        _cairo = cairo_create(_cairo_surface);
    }
    return _cairo;
}


Bitmap* Bitmap::create(int width, int height, int format) {
    return new BitmapLinux(width, height, format);
}

class BitmapDecodeTask : public Task {
public:
    GTask* _gtask;
    std::function<void(Bitmap*)> _callback;
    Bitmap* _bitmap;
    void* _data;
    int _cb;

    BitmapDecodeTask(const void* data, int cb, std::function<void(Bitmap*)> callback) : Task([=]() {
        _callback(_bitmap);
    }) {
        _callback = callback;
        _gtask = g_task_new(NULL, NULL, asyncReadyCallback, this);
        _data = malloc(cb);
        memcpy(_data, data, cb);
        _cb = cb;
        g_task_set_task_data (_gtask, this, NULL);
        g_task_run_in_thread(_gtask, do_work);
    }

    static void do_work(GTask* gtask,
                   gpointer source_obj,
                   gpointer task_data,
                   GCancellable *cancellable)
    {
        BitmapDecodeTask* task = (BitmapDecodeTask*)task_data;
        GError* error = NULL;
        GdkPixbufLoader* loader = gdk_pixbuf_loader_new();
        //g_signal_connect(loader, "area_prepared", G_CALLBACK(on_area_prepared), task);
        gdk_pixbuf_loader_write(loader, (const guint8*)task->_data, task->_cb, &error);
        gdk_pixbuf_loader_close(loader, &error);
        GdkPixbuf* pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
        task->_bitmap = new BitmapLinux(pixbuf);
        //g_task_return_pointer(task->_gtask, NULL, NULL);
        g_object_unref(loader);
        free(task->_data);
    }
    static void asyncReadyCallback(GObject *source_object, GAsyncResult *res,  gpointer user_data) {
        BitmapDecodeTask* task = (BitmapDecodeTask*)user_data;
        task->complete();
    }
    /*static void on_area_prepared(GdkPixbufLoader *loader, gpointer user_data) {
        BitmapDecodeTask* task = (BitmapDecodeTask*)user_data;
        GdkPixbuf* pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
        task->_bitmap = new BitmapLinux(pixbuf);
        g_task_return_pointer(task->_gtask, NULL, NULL);
    }*/

};

Task* Bitmap::createFromData(const void* data, int cb, std::function<void(Bitmap*)> callback) {
    return new BitmapDecodeTask(data, cb, callback);
}


#endif
