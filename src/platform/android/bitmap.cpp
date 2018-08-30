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
  if (format == BITMAPFORMAT_RGBA32) jformat = configARGB8888;
  else if (format == BITMAPFORMAT_RGB565) jformat = configRGB565;
  else if (format == BITMAPFORMAT_A8) jformat = configAlpha8;
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

Bitmap::Bitmap() : BitmapBase(), _androidBitmap(NULL) {
}

Bitmap::Bitmap(int width, int height, int format) : BitmapBase(width,height,format) {
    _texTarget = GL_TEXTURE_2D;
    _needsUpload = true;
    _androidBitmap = createAndroidBitmap(width, height, format, NULL);
}

Bitmap::Bitmap(jobject jbitmap) {
    auto env = getBitmapEnv();
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
Bitmap::Bitmap(GLuint textureId) {
    _texTarget = GL_TEXTURE_2D;
    _textureId = textureId;
}

Bitmap::~Bitmap() {
    if (_androidBitmap) {
        getBitmapEnv()->DeleteGlobalRef(_androidBitmap);
        _androidBitmap = NULL;
    }
}

void BitmapBase::createFromData(const void* data, int cb, std::function<void(Bitmap*)> callback) {
    auto env = getBitmapEnv();
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
    Bitmap* bitmap = new Bitmap(jbitmap);
    bitmap->retain();
    callback(bitmap);
    bitmap->release();
}


void Bitmap::lock(PIXELDATA* pixelData, bool forWriting) {
    auto env = getBitmapEnv();
    assert(_androidBitmap);
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, _androidBitmap, &info);
    pixelData->stride = info.stride;
    pixelData->cb = info.stride * info.height;
    AndroidBitmap_lockPixels(env, _androidBitmap, &pixelData->data);
}
void Bitmap::unlock(PIXELDATA* pixelData, bool pixelDataChanged) {
    auto env = getBitmapEnv();
    AndroidBitmap_unlockPixels(env, _androidBitmap);
    _needsUpload |= pixelDataChanged;
}

void Bitmap::bind() {
    BitmapBase::bind();

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

void Bitmap::fromVariant(const variant& v) {
    BitmapBase::fromVariant(v);
    auto bb = v.bytearrayVal("bb");
    _androidBitmap = createAndroidBitmap(_width, _height, _format, &bb);
    _needsUpload = true;
}
void Bitmap::toVariant(variant& v) {
    BitmapBase::toVariant(v);
    auto env = getBitmapEnv();
    int stride = env->CallIntMethod(_androidBitmap, jmidGetRowBytes);
    int cb = stride * _height;
    jobject byteBuffer = env->CallStaticObjectMethod(jclassByteBuffer, jmidAllocateDirect, cb);
    env->CallVoidMethod(_androidBitmap, jmidCopyPixelsToBuffer, byteBuffer);
    v["bb"] = bytearray((const uint8_t*)env->GetDirectBufferAddress(byteBuffer), cb);
}
#endif
