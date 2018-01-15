//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_ANDROID

#include "android.h"

static jclass jclassTimer;
static jmethodID jmidSchedule;
static jmethodID jmidUnschedule;



class OSTimer : public Timer {
public:
    OSTimer(const TimerDelegate& del, int intervalMillis, bool repeats) : Timer(del, intervalMillis, repeats) {
        JNIEnv *env = getJNIEnv();
        if (!jclassTimer) {
            jclassTimer = env->FindClass(PACKAGE "/Timer");
            jclassTimer = (jclass) env->NewGlobalRef(jclassTimer);
            jmidSchedule = env->GetStaticMethodID(jclassTimer, "schedule",
                                                           "(JJZ)L" PACKAGE "/Timer;");
            jmidUnschedule = env->GetMethodID(jclassTimer, "unschedule", "()V");
        }
        jobject obj = env->CallStaticObjectMethod(jclassTimer, jmidSchedule, (jlong)this, (jlong)intervalMillis, (jboolean)repeats);
        _jobject = env->NewGlobalRef(obj);
    }
    
    void stop() {
        if (_jobject) {
            JNIEnv* env = getJNIEnv();
            env->CallVoidMethod(_jobject, jmidUnschedule);
            env->DeleteGlobalRef(_jobject);
            _jobject = NULL;
        }
        Timer::stop();
    }
    
    void dispatch() {
        _del();
    }
    
protected:
    jobject _jobject;
};


Timer* Timer::start(const TimerDelegate& del, int intervalMillis, bool repeats) {
    return new OSTimer(del, intervalMillis, repeats);
}

JAVA_FN(void, Timer, nativeDispatch)(JNIEnv *env, jclass clazz, jlong cobj) {
    OSTimer* timer = (OSTimer*)cobj;
    timer->dispatch();
}

#endif
