//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>

Task* Task::create(TASKFUNC func) {
    return new Task(func);
}


class PosixTaskQueue : public TaskQueue {
public:
    pthread_t _pthread;
    pthread_cond_t _cond;
    pthread_mutex_t _mutex;
    vector<ObjPtr<Task>> _tasks;

    PosixTaskQueue(const string& name) : TaskQueue(name) {
        pthread_cond_init(&_cond, NULL);
        pthread_mutex_init(&_mutex, NULL);
        pthread_create(&_pthread, NULL, s_threadFunc, this);
    }
    ~PosixTaskQueue() {
        pthread_cond_destroy(&_cond);
        pthread_mutex_destroy(&_mutex);
    }

    void enqueueTask(Task* task) {
        pthread_mutex_lock(&_mutex);
        _tasks.push_back(task);
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_cond);
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
            ObjPtr<Task> task = _tasks.at(0);
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

void Task::nextTick(TASKFUNC func) {
    void* foo = new Foo(func);
    EM_ASM({
        window.setTimeout(function() { Runtime.dynCall('vi', $0, [$1]); }, 0);
    }, mainThreadThunk, foo);
}



#endif

