//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_LINUX

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

OSBitmap::OSBitmap(GdkPixbuf* pixbuf) : Bitmap() {
    //_pixbuf = pixbuf;
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
OSBitmap::OSBitmap(int width, int height, int format) : Bitmap(width, height, format) {
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

OSBitmap::~OSBitmap() {
    if (_cairo_surface) {
        cairo_surface_destroy(_cairo_surface);
        _cairo_surface = NULL;
    }
    if (_textureId) {
         check_gl(glDeleteTextures, 1, &_textureId);
        _textureId = 0;
    }
}

void OSBitmap::lock(PIXELDATA* pixelData, bool forWriting) {
    pixelData->data = cairo_image_surface_get_data(_cairo_surface);
    pixelData->stride = cairo_image_surface_get_stride(_cairo_surface);
    pixelData->cb = _height * pixelData->stride;
    assert(pixelData->data && pixelData->cb && pixelData->stride);
}



void OSBitmap::unlock(PIXELDATA* pixelData, bool pixelDataChanged) {
    if (pixelDataChanged) {
        _needsUpload = true;
    }
}

void OSBitmap::bind() {


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

cairo_t* OSBitmap::getCairo() {
    if (!_cairo) {
        _cairo = cairo_create(_cairo_surface);
    }
    return _cairo;
}

Bitmap* oakBitmapCreate(int width, int height, int format) {
    return new OSBitmap(width, height, format);
}

static void on_area_prepared(GdkPixbufLoader *loader, gpointer user_data) {
    GdkPixbuf* pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
    OSBitmap* bitmap = new OSBitmap(pixbuf);
    GError* error = NULL;
    std::function<void(Bitmap*)>* callback = (std::function<void(Bitmap*)>*)user_data;
    (*callback)(bitmap);
    //g_object_unref(loader);
}
void oakBitmapCreateFromData(const void* data, int cb, std::function<void(Bitmap*)> callback) {
    GError* error = NULL;
    GdkPixbufLoader* loader = gdk_pixbuf_loader_new();
    g_signal_connect(loader, "area_prepared", G_CALLBACK(on_area_prepared), &callback);
    gdk_pixbuf_loader_write(loader, (const guint8*)data, cb, &error);
    gdk_pixbuf_loader_close(loader, &error);

}


#endif
