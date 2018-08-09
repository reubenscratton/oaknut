//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_ANDROID

#if OAKNUT_WANT_AUDIOINPUT

#include <oaknut.h>

static jclass jclassAudioInput;
static jmethodID jmidConstructor;
static jmethodID jmidStart;
static jmethodID jmidStop;

class AudioInput {
public:
    jobject javaobj;
    AudioInputDelegate delegate;

    AudioInput(int sampleRate) {
        JNIEnv* env = getJNIEnv();
        jclassAudioInput = env->FindClass(PACKAGE "/AudioInput");
        jclassAudioInput = (jclass)env->NewGlobalRef(jclassAudioInput);
        jmidConstructor = env->GetMethodID(jclassAudioInput, "<init>", "(JI)V");
        jmidStart = env->GetMethodID(jclassAudioInput, "start", "()V");
        jmidStop = env->GetMethodID(jclassAudioInput, "stop", "()V");
        javaobj = env->NewObject(jclassAudioInput, jmidConstructor, (jlong)this, (jint)sampleRate);
    }
    ~AudioInput() {
        getJNIEnv()->DeleteGlobalRef(javaobj);
    }

    void start(AudioInputDelegate delegate) {
        this->delegate = delegate;
        getJNIEnv()->CallVoidMethod(javaobj, jmidStart);
    }

    void stop() {
        getJNIEnv()->CallVoidMethod(javaobj, jmidStop);
    }
};


const void* oakAudioInputOpen(int sampleRate) { // samples are always int16_t, always record in mono.
    return new AudioInput(sampleRate);
}

void oakAudioInputStart(const void* osobj, AudioInputDelegate delegate) {
    AudioInput* audioInput = (AudioInput*)osobj;
    audioInput->start(delegate);
}
void oakAudioInputStop(const void* osobj) {
    AudioInput* audioInput = (AudioInput*)osobj;
    audioInput->stop();
}
void oakAudioInputClose(const void* osobj) {
    delete (AudioInput*)osobj;
}

JAVA_FN(void, AudioInput, nativeOnGotData)(JNIEnv *env, jobject javaobj, jlong nativeObj, jshortArray buffer, jint offset, jint len) {
    //app.log("Got audio data!");
    AudioInput* audioInput = (AudioInput*)nativeObj;
    jshort* vals = env->GetShortArrayElements(buffer, NULL);
    audioInput->delegate(len, vals+offset);
    env->ReleaseShortArrayElements(buffer, vals, 0);
}

#endif
#endif
