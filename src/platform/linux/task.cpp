//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if PLATFORM_LINUX

#include <oaknut.h>


class PosixTaskQueue : public TaskQueue {
public:
    pthread_t _pthread;
    pthread_cond_t _cond;
    pthread_mutex_t _mutex;

    class PosixTask : public Task {
    public:
        PosixTask(TASKFUNC func) : Task(func) {
        }
        bool cancel() override {
            PosixTaskQueue* queue = (PosixTaskQueue*)(TaskQueue*)_queue;
            return queue->cancelTask(this);
        }
    };
    
    vector<sp<PosixTask>> _tasks;


    PosixTaskQueue(const string& name) : TaskQueue(name) {
        pthread_cond_init(&_cond, NULL);
        pthread_mutex_init(&_mutex, NULL);
        pthread_create(&_pthread, NULL, s_threadFunc, this);
    }
    ~PosixTaskQueue() {
        pthread_cond_destroy(&_cond);
        pthread_mutex_destroy(&_mutex);
    }
    
    Task* enqueueTask(TASKFUNC taskfunc) override {
        PosixTask* task = new PosixTask(taskfunc);
        pthread_mutex_lock(&_mutex);
        _tasks.push_back(task);
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_cond);
        return task;
    }
    bool cancelTask(Task* task) {
        bool found = false;
        pthread_mutex_lock(&_mutex);
        for (auto it = _tasks.begin() ; it!=_tasks.end() ; it++) {
            if (*it == task) {
                _tasks.erase(it);
                found = true;
                break;
            }
        }
        pthread_mutex_unlock(&_mutex);
        return found;
    }
    void terminate() {
        pthread_mutex_lock(&_mutex);
        _tasks.push_back(NULL);
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_cond);
    }

    static void* s_threadFunc(void* arg) {
        return ((PosixTaskQueue*)arg)->threadFunc();
    }
    
    void* threadFunc() {
        retain();
        for (;;) {
            pthread_mutex_lock(&_mutex);
            while (_tasks.size()==0) {
                pthread_cond_wait(&_cond, &_mutex);
            }
            sp<PosixTask> task = _tasks.at(0);
            _tasks.erase(_tasks.begin());
            pthread_mutex_unlock(&_mutex);
            if (!task) {
                break;
            }
            task->exec();
        }
        release();
        printf("exiting threadFunc\n");
        pthread_exit(NULL);
    }
};

TaskQueue* TaskQueue::create(const string &name) {
    return new PosixTaskQueue(name);
}


static int mainThreadThunk(void* data) {
    Task* task = (Task*)data;
    task->exec();
    task->release();
    return 0;
}

void Task::postToMainThread(std::function<void ()> func, int delay/*=0*/) {
    Task* task = new PosixTaskQueue::PosixTask(func);
    task->retain();
    g_idle_add(mainThreadThunk, task);
}

#endif

