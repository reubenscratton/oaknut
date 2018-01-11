#include "../../src/oaknut.h"

class WebTask {
public:
    WebTask(std::function<void(void)> func) {
        _func = func;
    }
    
    std::function<void(void)> _func;
    
    void exec() {
        _func();
    }
};


class WebQueue {
public:
    pthread_t _pthread;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    
    WebQueue() {
        pthread_cond_init(&cond, NULL);
        pthread_mutex_init(&mutex, NULL);
        pthread_create(&_pthread, NULL, s_threadFunc, this);
    }
    
    void enqueue(std::function<void(void)> func) {
        pthread_mutex_lock(&mutex);
        _tasks.push_back(new WebTask(func));
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
    }
    void terminate() {
        pthread_mutex_lock(&mutex);
        _tasks.push_back(NULL);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
    }

    static void* s_threadFunc(void* arg) {
        return ((WebQueue*)arg)->threadFunc();
    }
    
    void* threadFunc() {
        for (;;) {
            pthread_mutex_lock(&mutex);
            while (_tasks.size()==0) {
                pthread_cond_wait(&cond, &mutex);
            }
            WebTask* task = _tasks.at(0);
            _tasks.erase(_tasks.begin());
            pthread_mutex_unlock(&mutex);
            if (!task) {
                break;
            }
            task->exec();
            delete task;
        }
        printf("exiting threadFunc\n");
        pthread_exit(NULL);
        delete this;
    }
    
    
    vector<WebTask*> _tasks;
};




void* oakAsyncQueueCreate(const char* queueName) {
    return new WebQueue();
}
void oakAsyncQueueEnqueueItem(void* osobj, std::function<void(void)> func) {
    WebQueue* queue = (WebQueue*)osobj;
    queue->enqueue(func);
}
void oakAsyncQueueDestroy(void* osobj) {
    WebQueue* queue = (WebQueue*)osobj;
    queue->terminate();
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
    emscripten_async_waitable_run_in_main_runtime_thread_(EM_FUNC_SIG_VI, (void*)mainThreadThunk, pv);
}


