//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if USE_WORKER_FaceDetector

#if PLATFORM_ANDROID && 0 // code not ready yet

#include <oaknut.h>

static jclass jclassFaceDetector;
static jmethodID jmidConstructor;
static jmethodID jmidUpdate;

static JNIEnv* s_env;

class FaceDetectorWorker : public WorkerImpl {
public:

    jobject _faceDetector;
    
    FaceDetectorWorker() {
        int getEnvStat = g_jvm->GetEnv((void **)&s_env, JNI_VERSION_1_6);
        if (getEnvStat == JNI_EDETACHED) {
            if (g_jvm->AttachCurrentThread(&s_env, NULL) != 0) {
                app->log("Failed to attach to jvm");
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

    variant process_(const variant& data_in) override {
        int width = data_in.intVal("width");
        int height = data_in.intVal("height");
        const bytearray& bytes = data_in.bytearrayVal("data");
        PIXELDATA pixeldata;
        bitmap->lock(&pixeldata, false);
        jobject directBuffer = s_env->NewDirectByteBuffer(pixeldata.data, pixeldata.cb);
        int numFaces = s_env->CallIntMethod(_faceDetector, jmidUpdate, width, height, pixeldata.stride, directBuffer);
        bitmap->unlock(&pixeldata, false);
        variant result;
        result.setType(variant::ARRAY);
        for (int i=0 ; i<numFaces; i++) {
            result.appendVal(i); // todo: should be the face rect
        }
        return result;
    }

};

DECLARE_WORKER_IMPL(FaceDetectorWorker, "FaceDetector");

#endif
#endif
