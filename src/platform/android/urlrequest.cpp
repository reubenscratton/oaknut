//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#ifdef PLATFORM_ANDROID

#include <oaknut.h>

static jclass s_jclass;
static jmethodID s_jmidConstructor;
static jmethodID s_jmidCancel;


class URLRequestAndroid : public URLRequest {
public:
    URLRequestAndroid(const string& url, const string& method, const string& body, int flags)
        : URLRequest(url, method, body, flags) {
    }
    ~URLRequestAndroid() {
    }

    void run() override {
      JNIEnv *env = getJNIEnv();
      if (!s_jclass) {
          s_jclass = env->FindClass(PACKAGE "/URLRequest");
          s_jclass = (jclass) env->NewGlobalRef(s_jclass);
          s_jmidConstructor = env->GetMethodID(s_jclass, "<init>", "(JLjava/lang/String;Ljava/lang/String;)V");
          s_jmidCancel = env->GetMethodID(s_jclass, "cancel", "()V");
      }
      jobject url = env->NewStringUTF(_url.data());
      jobject method = env->NewStringUTF(_method.data());
      jobject object = env->NewObject(s_jclass, s_jmidConstructor, jlong(this), url, method);
      _osobj = env->NewGlobalRef(object);
    }

    void cancel() override {
        _cancelled = true;
        JNIEnv* env = getJNIEnv();
        env->CallVoidMethod((jobject)_osobj, s_jmidCancel);
        env->DeleteGlobalRef((jobject)_osobj);
        _osobj = NULL;
    }

    void dispatchResult(int httpStatus, const map<string,string>& responseHeaders, const bytearray& data) {
      _responseData = data;
      URLRequest::dispatchResult(httpStatus, responseHeaders);
    }

protected:
  jobject _osobj;
};

URLRequest* URLRequest::create(const string& url, const string& method, const string& body, int flags) {
    return new URLRequestAndroid(url, method, body, flags);
}

JAVA_FN(void, URLRequest, nativeOnGotData)(JNIEnv *env, jobject jobj, jlong cobj, jint httpStatus, jbyteArray httpHeadersUtf8, jbyteArray array) {
    URLRequestAndroid* req = (URLRequestAndroid*)cobj;
    int cb = env->GetArrayLength(array);
    bytearray data(cb);
    env->GetByteArrayRegion(array, 0, cb, reinterpret_cast<jbyte*>(data.data()));
    cb = env->GetArrayLength(httpHeadersUtf8);
    bytearray httpHeadersUtf8Bytes(cb);
    env->GetByteArrayRegion(httpHeadersUtf8, 0, cb, reinterpret_cast<jbyte*>(httpHeadersUtf8Bytes.data()));
    string httpHeadersStr((char*)httpHeadersUtf8Bytes.data());
    auto httpHeadersVec = httpHeadersStr.split("\n");
    map<string,string> responseHeaders;
    for (auto& it : httpHeadersVec) {
      string header = it;
      string name = header.extractUpTo(":", true);
      responseHeaders[name.lowercase()] = header;
    }
    req->dispatchResult((int)httpStatus, responseHeaders, data);
}

#endif
