//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#ifdef PLATFORM_ANDROID

#include <oaknut.h>

static jmethodID s_jmidConstructor;
static jmethodID s_jmidRun;
static jmethodID s_jmidCancel;



error URLRequest::ioLoadRemote(URLResponse* response) {

    JNIEnv *env = getJNIEnv();
    if (!s_jmidCancel) {
        s_jmidConstructor = env->GetMethodID(s_jclassURLRequest, "<init>", "(JLjava/lang/String;Ljava/lang/String;[B[B)V");
        s_jmidRun = env->GetMethodID(s_jclassURLRequest, "run", "(J)V");
        s_jmidCancel = env->GetMethodID(s_jclassURLRequest, "cancel", "()V");
    }
    jobject url = env->NewStringUTF(_url.c_str());
    jobject method = env->NewStringUTF(_method.c_str());

    // Concatenate the headers into a single string
    jbyteArray headersBytes = NULL;
    string headersStr;
    for (auto it : _headers) {
        if (headersStr.lengthInBytes()>0) {
            headersStr.append('\n');
        }
        headersStr.append(it.first);
        headersStr.append(':');
        headersStr.append(it.second);
    }
    int32_t cb = headersStr.lengthInBytes();
    if (cb) {
        headersBytes = env->NewByteArray(cb);
        jbyte *buf = env->GetByteArrayElements(headersBytes, NULL);
        memcpy((char*)buf, headersStr.c_str(), cb);
        env->ReleaseByteArrayElements(headersBytes, buf, 0);
    }

    // Body bytes
    jbyteArray bodyBytes = NULL;
    cb = _body.size();
    if (cb) {
        bodyBytes = env->NewByteArray(cb);
        jbyte *buf = env->GetByteArrayElements(bodyBytes, NULL);
        memcpy((char*)buf, _body.data(), cb);
        env->ReleaseByteArrayElements(bodyBytes, buf, 0);
    }

    jobject jreq = env->NewObject(s_jclassURLRequest, s_jmidConstructor, jlong(this), url, method, headersBytes, bodyBytes);
    jreq = env->NewGlobalRef(jreq);

    env->CallVoidMethod(jreq, s_jmidRun, (jlong)response);
    error err = error::none();
    return err;

//    req.cachePolicy = NSURLRequestReloadIgnoringCacheData;

}




JAVA_FN(void, URLRequest, nativeOnGotData)(JNIEnv *env, jobject jobj, jlong cobj,
               jlong cobjResponse, jint httpStatus, jbyteArray httpHeadersUtf8, jbyteArray array) {
    URLRequest *req = (URLRequest *) cobj;
    URLResponse* response = (URLResponse *) cobjResponse;
    response->httpStatus = httpStatus;
    if (array) {
        int cb = env->GetArrayLength(array);
        response->data.resize(cb);
        env->GetByteArrayRegion(array, 0, cb, reinterpret_cast<jbyte *>(response->data.data()));
        cb = env->GetArrayLength(httpHeadersUtf8);
        bytearray httpHeadersUtf8Bytes(cb);
        env->GetByteArrayRegion(httpHeadersUtf8, 0, cb,
                                reinterpret_cast<jbyte *>(httpHeadersUtf8Bytes.data()));
        string httpHeadersStr = httpHeadersUtf8Bytes.toString();
        auto httpHeadersVec = httpHeadersStr.split("\n");
        for (auto &it : httpHeadersVec) {
            auto colonPos = it.find(":");
            string name = it.substr(0, colonPos);
            string value = it.substr(colonPos+1);
            response->headers[name.lowercase()] = value;
        }
    }
}

JAVA_FN(jboolean, URLRequest, nativeCheckIfCancelled)(JNIEnv *env, jobject jobj, long cobj) {
    URLRequest *req = (URLRequest *) cobj;
    return req->isCancelled() ? 1:0;
}

#endif
