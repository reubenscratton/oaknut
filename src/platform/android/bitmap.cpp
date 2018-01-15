//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_ANDROID

#include "bitmap.h"


static jclass jclassBitmap;
static jmethodID jmidCreateBitmap;
static jclass jclassBitmapFactory;
static jmethodID jmidDecodeByteArray;
static jobject configAlpha8;
static jobject configARGB8888;
static jobject configRGB565;


Bitmap* oakBitmapCreate(int width, int height, int format) {
    return new OSBitmap(width, height, format);
}

OSBitmap::OSBitmap(int width, int height, int format) : Bitmap(width,height,format) {
    _texTarget = GL_TEXTURE_2D;
    JNIEnv* env = getJNIEnv();
    if (!jclassBitmap) {
        jclassBitmap = env->FindClass("android/graphics/Bitmap");
        jclassBitmap = (jclass) env->NewGlobalRef(jclassBitmap);
        jmidCreateBitmap = env->GetStaticMethodID(jclassBitmap, "createBitmap",
                                                     "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
        jclass jclassBitmapConfig = env->FindClass("android/graphics/Bitmap$Config");
        jfieldID fidAlpha8 = env->GetStaticFieldID(jclassBitmapConfig , "ALPHA_8", "Landroid/graphics/Bitmap$Config;");
        jfieldID fidARGB8888 = env->GetStaticFieldID(jclassBitmapConfig , "ARGB_8888", "Landroid/graphics/Bitmap$Config;");
        jfieldID fidRGB565 = env->GetStaticFieldID(jclassBitmapConfig , "RGB_565", "Landroid/graphics/Bitmap$Config;");
        configAlpha8 = env->NewGlobalRef(env->GetStaticObjectField(jclassBitmapConfig, fidAlpha8));
        configARGB8888 = env->NewGlobalRef(env->GetStaticObjectField(jclassBitmapConfig, fidARGB8888));
        configRGB565 = env->NewGlobalRef(env->GetStaticObjectField(jclassBitmapConfig, fidRGB565));
    }
    jobject jformat;
    if (format == BITMAPFORMAT_RGBA32) jformat = configARGB8888;
    else if (format == BITMAPFORMAT_RGB565) jformat = configRGB565;
    else if (format == BITMAPFORMAT_A8) jformat = configAlpha8;
    else assert(0);
    _androidBitmap = env->CallStaticObjectMethod(jclassBitmap, jmidCreateBitmap, width, height, jformat);
    assert(_androidBitmap);
    _androidBitmap = env->NewGlobalRef(_androidBitmap);
    _needsUpload = true;
}

OSBitmap::OSBitmap(jobject jbitmap) {
    JNIEnv* env = getJNIEnv();
    assert(jbitmap);
    _androidBitmap = env->NewGlobalRef(jbitmap);
    _texTarget = GL_TEXTURE_2D;
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, _androidBitmap, &info);
    _width = info.width;
    _height = info.height;
    switch (info.format) {
        case ANDROID_BITMAP_FORMAT_NONE: _format=BITMAPFORMAT_UNKNOWN; break;
        case ANDROID_BITMAP_FORMAT_RGBA_8888: _format=BITMAPFORMAT_RGBA32; break;
        case ANDROID_BITMAP_FORMAT_RGB_565: _format=BITMAPFORMAT_RGB565; break;
        case ANDROID_BITMAP_FORMAT_A_8: _format=BITMAPFORMAT_A8; break;
        default: assert(0); break;
    }
    _needsUpload = true;
}
OSBitmap::OSBitmap(GLuint textureId) {
    _texTarget = GL_TEXTURE_2D;
    _textureId = textureId;
}

OSBitmap::~OSBitmap() {
    if (_androidBitmap) {
        getJNIEnv()->DeleteGlobalRef(_androidBitmap);
        _androidBitmap = NULL;
    }
}

void oakBitmapCreateFromData(const void* data, int cb, std::function<void(Bitmap*)> callback) {
    JNIEnv* env = getJNIEnv();
    if (!jclassBitmapFactory) {
        jclassBitmapFactory = env->FindClass("android/graphics/BitmapFactory");
        jclassBitmapFactory = (jclass) env->NewGlobalRef(jclassBitmapFactory);
        jmidDecodeByteArray = env->GetStaticMethodID(jclassBitmapFactory, "decodeByteArray",
                                                     "([BII)Landroid/graphics/Bitmap;");
    }
    jbyteArray jbuff = env->NewByteArray(cb);
    env->SetByteArrayRegion(jbuff, 0, cb, (jbyte*)data);
    jobject jbitmap = env->CallStaticObjectMethod(jclassBitmapFactory, jmidDecodeByteArray, jbuff, 0, cb);
    env->DeleteLocalRef(jbuff);
    OSBitmap* bitmap = new OSBitmap(jbitmap);
    bitmap->retain();
    //dispatch_async(dispatch_get_main_queue(), ^() ;
    callback(bitmap);
    bitmap->release();
}


void OSBitmap::lock(PIXELDATA* pixelData, bool forWriting) {
    JNIEnv* env = getJNIEnv();
    assert(_androidBitmap);
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, _androidBitmap, &info);
    pixelData->stride = info.stride;
    pixelData->cb = info.stride * info.height;
    AndroidBitmap_lockPixels(env, _androidBitmap, &pixelData->data);
}
void OSBitmap::unlock(PIXELDATA* pixelData, bool pixelDataChanged) {
    JNIEnv* env = getJNIEnv();
    AndroidBitmap_unlockPixels(env, _androidBitmap);
    _needsUpload |= pixelDataChanged;
}

void OSBitmap::bind() {

    Bitmap::bind();


    // If bitmap data changed we may need to update texture data
    if (!_needsUpload) {
        return;
    }
    _needsUpload = false;

    // Slow path
    PIXELDATA pixeldata;
    lock(&pixeldata, false);
    if (!_allocdTexData) {
        _allocdTexData = true;
        check_gl(glTexImage2D, _texTarget, 0, getGlInternalFormat(),
                 _width, _height, 0, getGlFormat(), getGlPixelType(), pixeldata.data);
    } else {
        check_gl(glTexSubImage2D, _texTarget, 0, 0, 0, _width, _height, getGlFormat(), getGlPixelType(), pixeldata.data);
    }
    unlock(&pixeldata, false);
}

#endif

