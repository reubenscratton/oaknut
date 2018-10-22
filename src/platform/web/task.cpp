//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>


class TaskWeb : public Task {
public:
    TaskWeb(TASKFUNC func, TaskQueue* queue) : Task(func), _queue(queue) {
    }
    
    bool cancel() override;
    
    TaskQueue* _queue;
};



class PosixTaskQueue : public TaskQueue {
public:
    pthread_t _pthread;
    pthread_cond_t _cond;
    pthread_mutex_t _mutex;
    vector<sp<Task>> _tasks;

    PosixTaskQueue(const string& name) : TaskQueue(name) {
        pthread_cond_init(&_cond, NULL);
        pthread_mutex_init(&_mutex, NULL);
        pthread_create(&_pthread, NULL, s_threadFunc, this);
    }
    ~PosixTaskQueue() {
        pthread_cond_destroy(&_cond);
        pthread_mutex_destroy(&_mutex);
    }

    Task* enqueueTask(TASKFUNC func) override {
        TaskWeb* task = new TaskWeb(func, this);
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
            sp<Task> task = _tasks.at(0);
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


bool TaskWeb::cancel() {
    if (_queue) {
        ((PosixTaskQueue*)_queue)->cancelTask(this);
    }
    return true;
}

TaskQueue* TaskQueue::create(const string &name) {
    return new PosixTaskQueue(name);
}




class Foo {
public:
    Foo(std::function<void(void)> func) : _func(func) {
    }
    
    std::function<void(void)> _func;
};

static void mainThreadThunk(Foo* foo) {
    foo->_func();
    delete foo;
}



void Task::postToMainThread(TASKFUNC func, int delay) {
    void* foo = new Foo(func);
    if (emscripten_is_main_runtime_thread()) {
        EM_ASM({
            window.setTimeout(function() { Runtime.dynCall('vi', $0, [$1]); }, $2);
        }, mainThreadThunk, foo, delay);
    } else {
        emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, mainThreadThunk, foo);
    }
}
#endif

