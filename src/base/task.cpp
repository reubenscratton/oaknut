//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


class ThreadPool {
public:
    ThreadPool(size_t threads) : _stop(false) {
        for(size_t i=0 ; i<threads ; ++i) {
            _threads.emplace_back([this] {
                    for(;;) {
                        sp<Task> task;
                        {
                            std::unique_lock<std::mutex> lock(_mutex);
                            _condition.wait(lock,
                                [this]{ return _stop || !_tasks.empty(); });
                            if(_stop && _tasks.empty())
                                return;
                            task = _tasks.top();
                            assert(task);
                            _tasks.pop();
                        }
                        task->dispatch();
                    }
                }
            );
        }
    }

    void enqueue(Task* task) {
        if (_stop) {
            app->log("enqueue on stopped ThreadPool");
            return;
        }
        std::unique_lock<std::mutex> lock(_mutex);
        _tasks.emplace(task);
        _condition.notify_one();
    }

    ~ThreadPool() {
        _stop = true;
        _condition.notify_all();
        for (auto& thread : _threads) {
            thread.join();
        }
    }
private:
    vector<std::thread> _threads;
    std::priority_queue<sp<Task>> _tasks;
    std::mutex _mutex;
    std::condition_variable _condition;
    std::atomic<bool> _stop;
};
 


static ThreadPool* s_mainThreadPool;
static ThreadPool* s_ioThreadPool;

    
#if PLATFORM_WEB
    worker_handle _worker;
    std::function<void()> _onStop;
#endif


    
class BackgroundInit {
public:
    BackgroundInit() {
        
        unsigned int ncores=0,nthreads=0,ht=0;
        asm volatile("cpuid": "=a" (ncores), "=b" (nthreads) : "a" (0xb), "c" (0x1) : );
        ht=(ncores!=nthreads);
        //printf("Cores: %d\nThreads: %d\nHyperThreading: %s\n",ncores,nthreads,ht?"Yes":"No");
        s_mainThreadPool = new ThreadPool(MAX(1,ncores-1));
        s_ioThreadPool = new ThreadPool(2); // TODO: work out some kind of heuristic
    }
};
static BackgroundInit s_init;

Task::Task(RunContext runContext, std::function<variant(variant&)> func) :
    _runContext(runContext),
    _func(func)
{
    
}

void Task::dispatch() {
    if (_status != Cancelled) {
        _status = Executing;
        _output = _func(_input);
        _status = Complete;
        if (_nextTask) {
            _nextTask->_input = _output;
            enqueue(_nextTask);
        }
    }
}

void Task::enqueue(Task* task) {
    assert(task->_status == Created);
    switch(task->_runContext) {
        case Background:
            s_mainThreadPool->enqueue(task);
            break;
        case IO:
            s_ioThreadPool->enqueue(task);
            break;
        case MainThread:
            task->_status = Queued;
            app->postToMainThread([=]() {
                task->dispatch();
            });
            break;
    }
/*    pthread_mutex_lock(&s_qmutex);
    s_q.push(task);
    pthread_mutex_unlock(&s_qmutex);
    semaphore_signal(s_qsem);*/

}
Task* Task::enqueue(const vector<spec>& specs) {
    Task* prevTask = nullptr;
    Task* rootTask = nullptr;
    for (auto& spec : specs) {
        Task* task = new Task(spec.first, spec.second);
        if (prevTask) {
            prevTask->_nextTask = task;
        } else {
            rootTask = task;
        }
        prevTask = task;
    }
    enqueue(rootTask);
    return rootTask;
}


void Task::cancel() {
}


variant Task::fileLoad(const string& path) {
    assert(!isMainThread());
    int fd = ::open(path.c_str(), O_RDONLY, 0);
    if (-1 == fd) {
        return variant(error::fromErrno());
    }
    struct stat st;
    if (-1 == fstat(fd, &st)) {
        ::close(fd);
        return variant(error::fromErrno());
    }
    bytearray bytes((int32_t)st.st_size);
    ssize_t cbRead = ::read(fd, bytes.data(), st.st_size);
    if (cbRead == -1) {
        ::close(fd);
        return variant(error::fromErrno());
    }
    ::close(fd);
    assert(st.st_size == cbRead);
    return variant(bytes);
}

error Task::fileSave(const string& path, const bytearray& data) {
    assert(!isMainThread());
    int fd = ::open(path.c_str(), O_WRONLY, 0);
    if (-1 == fd) {
        return error::fromErrno();
    }
    ssize_t cbWritten = ::write(fd, data.data(), data.size());
    ::close(fd);
    if (cbWritten != data.size()) {
        return error::fromErrno();
    }
    return error::none();
}

