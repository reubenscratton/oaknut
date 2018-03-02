//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_LINUX

#include <oaknut.h>



class TaskQueue_POSIX : public TaskQueue {
public:
    pthread_t _pthread;
    pthread_cond_t _cond;
    pthread_mutex_t _mutex;
    int _counter;
    typedef pair<int, std::function<void(void)>> QUEUED_TASK;
    vector<QUEUED_TASK> _tasks;

    TaskQueue_POSIX(const string& name) : TaskQueue(name) {
        pthread_cond_init(&_cond, NULL);
        pthread_mutex_init(&_mutex, NULL);
        pthread_create(&_pthread, NULL, s_threadFunc, this);
    }
    ~TaskQueue_POSIX() {
        pthread_cond_destroy(&_cond);
        pthread_mutex_destroy(&_mutex);
    }
    
    int enqueueTask(std::function<void(void)> func) {
        pthread_mutex_lock(&_mutex);
        int taskId = ++_counter;
        _tasks.push_back(QUEUED_TASK(taskId,func));
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_cond);
        return taskId;
    }
    bool cancelTask(int taskId) {
        bool found = false;
        pthread_mutex_lock(&_mutex);
        for (auto it = _tasks.begin() ; it!=_tasks.end() ; it++) {
            if (it->first == taskId) {
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
        _tasks.push_back(QUEUED_TASK(-1,NULL));
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_cond);
    }

    static void* s_threadFunc(void* arg) {
        return ((TaskQueue_POSIX*)arg)->threadFunc();
    }
    
    void* threadFunc() {
        retain();
        for (;;) {
            pthread_mutex_lock(&_mutex);
            while (_tasks.size()==0) {
                pthread_cond_wait(&_cond, &_mutex);
            }
            QUEUED_TASK task = _tasks.at(0);
            _tasks.erase(_tasks.begin());
            pthread_mutex_unlock(&_mutex);
            if (task.first < 0) {
                break;
            }
            task.second();
        }
        release();
        printf("exiting threadFunc\n");
        pthread_exit(NULL);
    }
    
    
};

TaskQueue* TaskQueue::create(const string &name) {
    return new TaskQueue_POSIX(name);
}



class Foo {
public:
    Foo(std::function<void(void)> func) : _func(func) {
    }
    
    std::function<void(void)> _func;
};

void mainThreadThunk(Foo* foo) {
    foo->_func();
    delete foo;
}

void oakAsyncRunOnMainThread(std::function<void(void)> func) {
    void* pv = new Foo(func);
    oakLog("TODO! oakAsyncRunOnMainThread");
    //emscripten_async_waitable_run_in_main_runtime_thread_(EM_FUNC_SIG_VI, (void*)mainThreadThunk, pv);
}

#endif

