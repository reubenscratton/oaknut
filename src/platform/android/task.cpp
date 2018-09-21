//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_ANDROID

#include <oaknut.h>

static jclass jclassTaskQueue, jclassTask;
static jmethodID jmidConstructor;
static jmethodID jmidEnqueue;
static jmethodID jmidTaskCancel;
static jmethodID jmidRunOnMainThread;


static JNIEnv* getTaskEnv() {
  JNIEnv* env = getJNIEnv();
  if (!jclassTaskQueue) {
      jclassTaskQueue = env->FindClass(PACKAGE "/TaskQueue");
      jclassTaskQueue = (jclass)env->NewGlobalRef(jclassTaskQueue);
      jmidConstructor = env->GetMethodID(jclassTaskQueue, "<init>", "()V");
      jmidEnqueue = env->GetMethodID(jclassTaskQueue, "enqueue", "(J)L" PACKAGE "/Task;");
      jmidRunOnMainThread = env->GetStaticMethodID(jclassTaskQueue, "runOnMainThread", "(IJ)V");
      jclassTask = env->FindClass(PACKAGE "/Task");
      jclassTask = (jclass)env->NewGlobalRef(jclassTask);
      jmidTaskCancel = env->GetMethodID(jclassTask, "cancel", "()V");
  }
  return env;
}

class TaskAndroid : public Task {
public:
    TaskAndroid(TASKFUNC func) : Task(func) {
    }

    bool cancel() override {
        getTaskEnv()->CallVoidMethod(_obj, jmidTaskCancel);
        return true;
    }

    jobject _obj;
};

class TaskQueueAndroid : public TaskQueue {
public:
    
    jobject _obj;
    
    TaskQueueAndroid(const string& name) : TaskQueue(name) {
        JNIEnv* env = getTaskEnv();
        jobject queue = env->NewObject(jclassTaskQueue, jmidConstructor);
        _obj = env->NewGlobalRef(queue);
    }
    ~TaskQueueAndroid() {
        JNIEnv* env = getJNIEnv();
        env->DeleteGlobalRef(_obj);
    }
    
    Task* enqueueTask(TASKFUNC func) override {
        JNIEnv* env = getTaskEnv();
        TaskAndroid* task = new TaskAndroid(func);
        task->retain();
        task->_obj = env->NewGlobalRef(env->CallObjectMethod(_obj, jmidEnqueue, (jlong)task));
        return task;
    }

};

TaskQueue* TaskQueue::create(const string& name) {
    return new TaskQueueAndroid(name);
}



void Task::nextTick(std::function<void ()> func) {
    Task* task = new TaskAndroid(func);
    task->retain();
    getTaskEnv()->CallStaticVoidMethod(jclassTaskQueue, jmidRunOnMainThread, 0, (jlong)task);
}

void Task::after(int delay, std::function<void ()> func) {
    Task* task = new TaskAndroid(func);
    task->retain();
    getTaskEnv()->CallStaticVoidMethod(jclassTaskQueue, jmidRunOnMainThread, (jint)delay, (jlong)task);
}

JAVA_FN(void, Task, nativeRun)(JNIEnv *env, jobject obj, jlong nativeObj) {
    TaskAndroid* task = (TaskAndroid*)nativeObj;
    task->exec();
    task->release();
}


#endif
