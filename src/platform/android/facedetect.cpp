//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_ANDROID

#include <oaknut.h>

static jclass jclassFaceDetector;
static jmethodID jmidConstructor;
static jmethodID jmidUpdate;

static JNIEnv* s_env;

class FaceDetectorAndroid : public FaceDetector {
public:
    
    jobject _faceDetector;
    
    FaceDetectorAndroid() {
        int getEnvStat = g_jvm->GetEnv((void **)&s_env, JNI_VERSION_1_6);
        if (getEnvStat == JNI_EDETACHED) {
            if (g_jvm->AttachCurrentThread(&s_env, NULL) != 0) {
                app.log("Failed to attach to jvm");
                assert(0);
            }
        }
        jclassFaceDetector = s_env->FindClass(PACKAGE "/FaceDetector");
        jclassFaceDetector = (jclass)s_env->NewGlobalRef(jclassFaceDetector);
        jmidConstructor = s_env->GetMethodID(jclassFaceDetector, "<init>", "()V");
        jmidUpdate = s_env->GetMethodID(jclassFaceDetector, "update", "(IIILjava/nio/Buffer;)I");
        jobject faceDetector = s_env->NewObject(jclassFaceDetector, jmidConstructor);
        _faceDetector = s_env->NewGlobalRef(faceDetector);
    }
    ~FaceDetectorAndroid() {
        s_env->DeleteGlobalRef(_faceDetector);
    }

    int detectFaces(Bitmap* bitmap) {
        PIXELDATA pixeldata;
        bitmap->lock(&pixeldata, false);
        jobject directBuffer = s_env->NewDirectByteBuffer(pixeldata.data, pixeldata.cb);
        int numFaces = s_env->CallIntMethod(_faceDetector, jmidUpdate, bitmap->_width, bitmap->_height, pixeldata.stride, directBuffer);
        bitmap->unlock(&pixeldata, false);
        return numFaces;
    }

};

FaceDetector* FaceDetector::create() {
    return new FaceDetectorAndroid();
}

#endif

