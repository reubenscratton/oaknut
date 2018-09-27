//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_ANDROID

#include <oaknut.h>

static jclass jclassAudioInput;
static jmethodID jmidConstructor;
static jmethodID jmidStart;
static jmethodID jmidStop;

class AudioInputAndroid : public AudioInput {
public:
    jobject javaobj;

    AudioInputAndroid() {
        JNIEnv* env = getJNIEnv();
        jclassAudioInput = env->FindClass(PACKAGE "/AudioInput");
        jclassAudioInput = (jclass)env->NewGlobalRef(jclassAudioInput);
        jmidConstructor = env->GetMethodID(jclassAudioInput, "<init>", "(JI)V");
        jmidStart = env->GetMethodID(jclassAudioInput, "start", "()V");
        jmidStop = env->GetMethodID(jclassAudioInput, "stop", "()V");
    }
    ~AudioInputAndroid() {
    }

    void open(int sampleRate) override {
        JNIEnv* env = getJNIEnv();
        javaobj = env->NewObject(jclassAudioInput, jmidConstructor, (jlong)this, (jint)sampleRate);
        javaobj = env->NewGlobalRef(javaobj);
    }
    void start() override {
        getJNIEnv()->CallVoidMethod(javaobj, jmidStart);
    }

    void stop() override {
        getJNIEnv()->CallVoidMethod(javaobj, jmidStop);
    }

    void close() override {
        getJNIEnv()->DeleteGlobalRef(javaobj);
    }
};

AudioInput* AudioInput::create() {
    return new AudioInputAndroid();
}


JAVA_FN(void, AudioInput, nativeOnGotData)(JNIEnv *env, jobject javaobj, jlong nativeObj, jobject byteBuffer, jint cb) {
    AudioInputAndroid* audioInput = (AudioInputAndroid*)nativeObj;
    void* bytes = env->GetDirectBufferAddress(byteBuffer);
    ObjPtr<AudioInputSamples> samples = new AudioInputSamples();
    samples->_data = bytearray((uint8_t*)bytes, cb);
    audioInput->onNewAudioSamples(samples);
}

#endif
