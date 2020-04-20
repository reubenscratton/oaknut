//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_ANDROID

#include <oaknut.h>


static jclass jclassBitmap;
static jmethodID jmidCreateBitmap;
static jmethodID jmidCopyPixelsFromBuffer;
static jmethodID jmidCopyPixelsToBuffer;
static jmethodID jmidGetRowBytes;
static jclass jclassBitmapFactory;
static jmethodID jmidDecodeByteArray;
static jobject configAlpha8;
static jobject configARGB8888;
static jobject configRGB565;
static jclass jclassByteBuffer;
static jmethodID jmidAllocateDirect;

static JNIEnv* getBitmapEnv() {
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
      jmidCopyPixelsFromBuffer = env->GetMethodID(jclassBitmap, "copyPixelsFromBuffer",
                                                   "(Ljava/nio/Buffer;)V");
      jmidCopyPixelsToBuffer = env->GetMethodID(jclassBitmap, "copyPixelsFromBuffer",
                                                   "(Ljava/nio/Buffer;)V");
      jmidGetRowBytes = env->GetMethodID(jclassBitmap, "getRowBytes", "()I");
      jclassByteBuffer = env->FindClass("java/nio/ByteBuffer");
      jclassByteBuffer = (jclass) env->NewGlobalRef(jclassByteBuffer);
      jmidAllocateDirect = env->GetStaticMethodID(jclassByteBuffer, "allocateDirect",
                    "(I)Ljava/nio/ByteBuffer;");

  }
  return env;
}
static jobject createAndroidBitmap(int width, int height, int format, bytearray* pixels) {
  auto env = getBitmapEnv();
  jobject jformat;
  if (format == PIXELFORMAT_RGBA32) jformat = configARGB8888;
  else if (format == PIXELFORMAT_RGB565) jformat = configRGB565;
  else if (format == PIXELFORMAT_A8) jformat = configAlpha8;
  else assert(0);
  jobject androidBitmap = env->CallStaticObjectMethod(jclassBitmap, jmidCreateBitmap, width, height, jformat);
  assert(androidBitmap);
  if (pixels) {
    jobject byteBuffer = env->CallStaticObjectMethod(jclassByteBuffer, jmidAllocateDirect, pixels->size());
    void* byteBufferBytes = env->GetDirectBufferAddress(byteBuffer);
    memcpy(byteBufferBytes, pixels->data(), pixels->size());
    env->CallVoidMethod(androidBitmap, jmidCopyPixelsFromBuffer, byteBuffer);
  }

  return env->NewGlobalRef(androidBitmap);
}

BitmapAndroid::BitmapAndroid() : Bitmap(), _androidBitmap(NULL) {
}

BitmapAndroid::BitmapAndroid(int width, int height, int format) : Bitmap(width,height,format) {
    _androidBitmap = createAndroidBitmap(width, height, format, NULL);
}

BitmapAndroid::BitmapAndroid(jobject jbitmap) {
    auto env = getBitmapEnv();
    assert(jbitmap);
    _androidBitmap = env->NewGlobalRef(jbitmap);
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, _androidBitmap, &info);
    _width = info.width;
    _height = info.height;
    switch (info.format) {
        case ANDROID_BITMAP_FORMAT_NONE: _format=PIXELFORMAT_UNKNOWN; break;
        case ANDROID_BITMAP_FORMAT_RGBA_8888: _format=PIXELFORMAT_RGBA32; break;
        case ANDROID_BITMAP_FORMAT_RGB_565: _format=PIXELFORMAT_RGB565; break;
        case ANDROID_BITMAP_FORMAT_A_8: _format=PIXELFORMAT_A8; break;
        default: assert(0); break;
    }
    //_needsUpload = true;
}
BitmapAndroid::BitmapAndroid(GLuint textureId) {
    //_texTarget = GL_TEXTURE_2D;
    //_textureId = textureId;
    assert(0);
}

BitmapAndroid::~BitmapAndroid() {
    if (_androidBitmap) {
        getBitmapEnv()->DeleteGlobalRef(_androidBitmap);
        _androidBitmap = NULL;
    }
}



Bitmap* Bitmap::createFromData(const bytearray& data) {
    auto env = getBitmapEnv();
    if (!jclassBitmapFactory) {
        jclassBitmapFactory = env->FindClass("android/graphics/BitmapFactory");
        jclassBitmapFactory = (jclass) env->NewGlobalRef(jclassBitmapFactory);
        jmidDecodeByteArray = env->GetStaticMethodID(jclassBitmapFactory, "decodeByteArray", "([BII)Landroid/graphics/Bitmap;");
    }
    jbyteArray jbuff = env->NewByteArray(data.length());
    env->SetByteArrayRegion(jbuff, 0, data.length(), (jbyte*)data.data());
    jobject jbitmap = env->CallStaticObjectMethod(jclassBitmapFactory, jmidDecodeByteArray, jbuff, 0, data.length());
    //env->DeleteLocalRef(jbuff);
    return new BitmapAndroid(jbitmap);
}

Bitmap* Bitmap::create(int width, int height, int format) {
    return new BitmapAndroid(width, height, format);
}



void BitmapAndroid::lock(PIXELDATA* pixelData, bool forWriting) {

    assert(_androidBitmap);
    /*if (!_androidBitmap) {
        assert(_textureId);
        GLint oldTex;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTex);
        glBindTexture(GL_TEXTURE_2D, _textureId);
        pixelData->stride = _width * 4;
        pixelData->cb = pixelData->stride * _height;
        pixelData->data = malloc(pixelData->cb);
        check_gl(glReadPixels, 0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, pixelData->data);
        glBindTexture(GL_TEXTURE_2D, oldTex);
    } else {*/
        auto env = getBitmapEnv();
        AndroidBitmapInfo info;
        AndroidBitmap_getInfo(env, _androidBitmap, &info);
        pixelData->stride = info.stride;
        pixelData->cb = info.stride * info.height;
        AndroidBitmap_lockPixels(env, _androidBitmap, &pixelData->data);
    //}

}
void BitmapAndroid::unlock(PIXELDATA* pixelData, bool pixelDataChanged) {
    //if (!_androidBitmap) {
    //    assert(!pixelDataChanged); // todo: implement texture upload
    //    free(pixelData->data);
    //} else {
    auto env = getBitmapEnv();
    AndroidBitmap_unlockPixels(env, _androidBitmap);
    if (pixelDataChanged) {
        texInvalidate();
    }
    //}
}


void BitmapAndroid::fromVariant(const variant& v) {
    Bitmap::fromVariant(v);
    auto bb = v.bytearrayRef("bb");
    _androidBitmap = createAndroidBitmap(_width, _height, _format, &bb);
    texInvalidate();
}
void BitmapAndroid::toVariant(variant& v) {
    Bitmap::toVariant(v);
    auto env = getBitmapEnv();
    int stride = env->CallIntMethod(_androidBitmap, jmidGetRowBytes);
    int cb = stride * _height;
    jobject byteBuffer = env->CallStaticObjectMethod(jclassByteBuffer, jmidAllocateDirect, cb);
    env->CallVoidMethod(_androidBitmap, jmidCopyPixelsToBuffer, byteBuffer);
    v["bb"] = bytearray((const uint8_t*)env->GetDirectBufferAddress(byteBuffer), cb);
}
#endif
