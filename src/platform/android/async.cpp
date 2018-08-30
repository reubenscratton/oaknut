//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_ANDROID

#include <oaknut.h>

static jclass jclassAsync;
static jmethodID jmidConstructor;
static jmethodID jmidEnqueue;
static jmethodID jmidRunOnMainThread;


static JNIEnv* getTaskEnv() {
  JNIEnv* env = getJNIEnv();
  if (!jclassAsync) {
     jclassAsync = env->FindClass(PACKAGE "/Async");
     jclassAsync = (jclass)env->NewGlobalRef(jclassAsync);
     jmidRunOnMainThread = env->GetStaticMethodID(jclassAsync, "runOnMainThread", "(J)V");
  }
  return env;
}

void* oakAsyncQueueCreate(const char* queueName) {
    JNIEnv* env = getJNIEnv();
    jmidConstructor = env->GetMethodID(jclassAsync, "<init>", "()V");
    jmidEnqueue = env->GetMethodID(jclassAsync, "enqueue", "(J)V");
    jobject queue = env->NewObject(jclassAsync, jmidConstructor);
    return (void*)env->NewGlobalRef(queue);
}
void oakAsyncQueueDestroy(void* osobj) {
    JNIEnv* env = getJNIEnv();
    env->DeleteGlobalRef((jobject)osobj);
}
void oakAsyncQueueEnqueueItem(void* osobj, function<void(void)> func) {
    JNIEnv* env = getJNIEnv();
    function<void(void)>* cpy = new function<void(void)>();
    *cpy = func;
    env->CallVoidMethod((jobject)osobj, jmidEnqueue, (jlong)cpy);
}


void Task::nextTick(std::function<void ()> func) {
  function<void(void)>* cpy = new function<void(void)>();
  *cpy = func;
  getTaskEnv()->CallStaticVoidMethod(jclassAsync, jmidRunOnMainThread, (jlong)cpy);
}

JAVA_FN(void, Async, nativeRun)(JNIEnv *env, jobject obj, jlong nativeObj) {
    function<void(void)>* func = (function<void(void)>*)nativeObj;
    (*func)();
    delete func;
}

#endif
