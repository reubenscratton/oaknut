//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#ifdef PLATFORM_ANDROID

#include "bitmap.h"

static jclass s_jclass;
static jmethodID s_jmidConstructor;
static jmethodID s_jmidCancel;


void URLRequest::nativeStart() {
    retain(); // ensure request is alive until it completes, can't count on anything else to keep us alive
    JNIEnv *env = getJNIEnv();
    if (!s_jclass) {
        s_jclass = env->FindClass(PACKAGE "/URLRequest");
        s_jclass = (jclass) env->NewGlobalRef(s_jclass);
        s_jmidConstructor = env->GetMethodID(s_jclass, "<init>", "(JLjava/lang/String;)V");
        s_jmidCancel = env->GetMethodID(s_jclass, "cancel", "()V");
    }
    jobject url = env->NewStringUTF(_url.data());
    jobject object = env->NewObject(s_jclass, s_jmidConstructor, jlong(this), url);
    _osobj = env->NewGlobalRef(object);
}

void URLRequest::nativeStop() {
    JNIEnv* env = getJNIEnv();
    env->CallVoidMethod((jobject)_osobj, s_jmidCancel);
    env->DeleteGlobalRef((jobject)_osobj);
    _osobj = NULL;
}

JAVA_FN(void, URLRequest, nativeOnGotBitmap)(JNIEnv *env, jobject jobj, jlong cobj, jobject jbitmap) {
    URLRequest* req = (URLRequest*)cobj;
    OSBitmap* bitmap = new OSBitmap(jbitmap);
    bitmap->retain();
    req->dispatchOnLoad(new URLData(bitmap));
    req->release(); // match retain() in nativeStart();
    bitmap->release();
}

JAVA_FN(void, URLRequest, nativeOnGotGenericData)(JNIEnv *env, jobject jobj, jlong cobj, jbyteArray array) {
    URLRequest* req = (URLRequest*)cobj;
    Data* data = new Data();
    data->cb = env->GetArrayLength(array);
    data->data = (uint8_t*)malloc(data->cb);
    env->GetByteArrayRegion(array, 0, data->cb, reinterpret_cast<jbyte*>(data->data));
    data->retain();
    req->dispatchOnLoad(new URLData(data));
    req->release(); // match retain() in nativeStart();
    data->release();
}

#endif
