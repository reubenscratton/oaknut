#include "android.h"

static jclass jclassAsync;
static jmethodID jmidConstructor;
static jmethodID jmidEnqueue;
static jmethodID jmidRunOnMainThread;


void* oakAsyncQueueCreate(const char* queueName) {
    JNIEnv* env = getJNIEnv();
    jclassAsync = env->FindClass(PACKAGE "/Async");
    jclassAsync = (jclass)env->NewGlobalRef(jclassAsync);
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

void oakAsyncRunOnMainThread(function<void(void)> func) {
    JNIEnv* env = getJNIEnv();
    if (!jmidRunOnMainThread) {
        jmidRunOnMainThread = env->GetStaticMethodID(jclassAsync, "runOnMainThread", "(J)V");
    }
    function<void(void)>* cpy = new function<void(void)>();
    *cpy = func;
    env->CallStaticVoidMethod(jclassAsync, jmidRunOnMainThread, (jlong)cpy);
}

JAVA_FN(void, Async, nativeRun)(JNIEnv *env, jobject obj, jlong nativeObj) {
    function<void(void)>* func = (function<void(void)>*)nativeObj;
    (*func)();
    delete func;
}