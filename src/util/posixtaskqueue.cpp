//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_ANDROID || PLATFORM_LINUX

#include <oaknut.h>


static void* s_threadFunc(void* arg) {
    ((PosixTaskQueue*)arg)->threadFunc();
    return NULL;
}



PosixTaskQueue::PosixTaskQueue(const string& name) {
    pthread_cond_init(&_cond, NULL);
    pthread_mutex_init(&_mutex, NULL);
    retain(); // +1 ref as long as the thread is alive
    pthread_create(&_pthread, NULL, s_threadFunc, this);
}

PosixTaskQueue::~PosixTaskQueue() {
    pthread_cond_destroy(&_cond);
    pthread_mutex_destroy(&_mutex);
}

int PosixTaskQueue::enqueueTask(std::function<void(void)> func) {
    pthread_mutex_lock(&_mutex);
    int counter = ++_counter;
    _tasks.push_back(std::make_pair(counter,func));
    pthread_mutex_unlock(&_mutex);
    pthread_cond_signal(&_cond);
    return counter;
}

void PosixTaskQueue::threadFunc() {
    for (;;) {
        pthread_mutex_lock(&_mutex);
        while (_tasks.size()==0) {
            pthread_cond_wait(&_cond, &_mutex);
        }
        auto entry = _tasks.at(0);
        _tasks.erase(_tasks.begin());
        pthread_mutex_unlock(&_mutex);
        if (entry.first<0) {
            break;
        }
        entry.second();
    }
    release();
    printf("exiting threadFunc\n");
    pthread_exit(NULL);
}
bool PosixTaskQueue::cancelTask(int taskId) {
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

void PosixTaskQueue::terminate() {
    pthread_mutex_lock(&_mutex);
    _tasks.push_back(std::make_pair(-1, [](){}));
    pthread_mutex_unlock(&_mutex);
    pthread_cond_signal(&_cond);
}
    
/*


class Foo {
public:
    Foo(std::function<void(void)> func) : _func(func) {
    }
    
    std::function<void(void)> _func;
};

static void mainThreadThunk(Foo* foo) {
    foo->_func();
    delete foo;
}*/

#endif
