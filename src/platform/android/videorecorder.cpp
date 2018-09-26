//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#ifdef PLATFORM_ANDROID

#include <oaknut.h>
#include "platform.h"


class VideoRecorderAndroid : public VideoRecorder {
public:

    jclass _jclass;
    jobject _jobject;

    VideoRecorderAndroid() {
        JNIEnv* env = getJNIEnv();
        _jclass = env->FindClass(PACKAGE "/VideoRecorder");
        _jclass = (jclass)env->NewGlobalRef(_jclass);
        jmethodID jmidConstructor = env->GetMethodID(_jclass, "<init>", "(J)V");
        _jobject = env->NewObject(_jclass, jmidConstructor, (jlong)this);
        _jobject = env->NewGlobalRef(_jobject);
    }
    ~VideoRecorderAndroid() {
        JNIEnv* env = getJNIEnv();
        env->DeleteGlobalRef(_jobject);
        env->DeleteGlobalRef(_jclass);
    }


    void start(SIZE size, int frameRate, int keyframeRate, int audioSampleRate) override {
        JNIEnv* env = getJNIEnv();
        jmethodID jmid = env->GetMethodID(_jclass, "start", "(IIIII)V");
        env->CallVoidMethod(_jobject, jmid, (jint)size.width, (jint)size.height, frameRate, keyframeRate, audioSampleRate);
    }

    void handleNewCameraFrame(int textureId, long timestamp, float* transform) override {
        JNIEnv* env = getJNIEnv();
        jmethodID jmid = env->GetMethodID(_jclass, "handleNewCameraFrame", "(IJ[F)V");
        jfloatArray jtransform = env->NewFloatArray(16);
        env->SetFloatArrayRegion(jtransform, 0, 16, transform);
        env->CallVoidMethod(_jobject, jmid, textureId, timestamp, jtransform);
    }

    void handleNewAudioSamples(void* samples, int numBytes) override {
        JNIEnv* env = getJNIEnv();
        jmethodID jmid = env->GetMethodID(_jclass, "handleNewAudioSamples", "([B)V");
        jbyteArray jsamples = env->NewByteArray(numBytes);
        env->SetByteArrayRegion(jsamples, 0, numBytes, (jbyte*)samples);
        env->CallVoidMethod(_jobject, jmid, jsamples);
    }


    void stop() override {
        JNIEnv* env = getJNIEnv();
        jmethodID jmid = env->GetMethodID(_jclass, "stop", "()V");
        env->CallVoidMethod(_jobject, jmid);
    }

};

VideoRecorder* VideoRecorder::create() {
    return new VideoRecorderAndroid();
}

#endif

