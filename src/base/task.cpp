//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>
#include <sched.h>



/**
 * @ingroup base_group
 * @class Thread
 * @brief Oaknut Threads are tightly controlled - you cannot spawn your own Threads, so if you need to do that
 * please use `std::thread` and be sure you know what you're doing. Oaknut maintains two pools of background
 * threads, one for CPU-intensive work, and one for I/O. These background threads execute `Tasks`, which
 * are cancelable, have priority ordering, and other useful attributes.
 *
 * The background thread pool is limited to the number of physical cores present.
 * The I/O thread pool grows dynamically to a maximum of 50 threads or so. Obviously it is expected that IO threads
 * will spend the vast majority of their time blocked and waiting for IO to complete.
 *
 */

namespace oak {
class ThreadLocalData {
public:
    
    std::list<Object*> _deadObjects;
    vector<std::function<void()>> _callbacks;
    bool _isMainThread;
    std::mutex _mutex;
    
    void flush() {
        while (_deadObjects.size()) {
            auto it = _deadObjects.begin();
            Object* obj = *it;
            assert(obj->_refs == 0);
            _deadObjects.erase(it);
            delete obj;
        }
        
        _mutex.lock();
        if (_callbacks.size()) {
            for (auto& cb : _callbacks) {
                cb();
            }
            _callbacks.clear();
        }
        _mutex.unlock();
    }
    
    void postCallback(std::function<void()> callback) {
        _mutex.lock();
        _callbacks.push_back(callback);
        _mutex.unlock();
    }

};
}

static thread_local ThreadLocalData s_threadLocalData;
static ThreadLocalData* s_mainThreadLocalData;

static struct MainThreadInit {
    MainThreadInit() {
        s_threadLocalData._isMainThread = true;
        s_mainThreadLocalData = &s_threadLocalData;
    }
} s_mainThreadInit;

bool Task::isMainThread() {
    return s_threadLocalData._isMainThread;
}

void Task::addObjectToCurrentThreadDeletePool(Object* obj) {
    s_threadLocalData._deadObjects.push_back(obj);
}

void Task::flushCurrentThread() {
    s_threadLocalData.flush();
}

namespace oak {

class ThreadPool {
public:
    struct thread_init {
        ThreadPool* pool;
        int thread_index;
    };
    
    ThreadPool(Task::exec_context ctx, int threads) : _ctx(ctx), _stop(false) {
        for(int i=0 ; i<threads ; ++i) {
            pthread_t thread;
            auto init = new thread_init {this, i+1};
            int status = pthread_create(&thread, NULL, thread_func, init);
            assert(status == 0);
            _threads.push_back(thread);
        }
    }

    ~ThreadPool();
    
    static void* thread_func(void* arg) {
        auto init = (thread_init*)arg;
        ThreadPool* pool = init->pool;
        char name[64];
        const char* prefix;
        switch (pool->_ctx) {
            case Task::MainThread: prefix="Main"; break;
            case Task::Background: prefix="Background"; break;
            case Task::IO: prefix="IO"; break;
            default: prefix="?"; break;
        }
        sprintf(name, "%s %d", prefix, init->thread_index);
        pthread_setname_np(name);
        delete init;
        
        for(;;) {
            Task::subtask subtask;
            if (pool->waitForSubtask(subtask)) {
                subtask.exec();
            }
            s_threadLocalData.flush();
        }
        return nullptr;

    }
    bool waitForSubtask(Task::subtask& subtask) {
        std::unique_lock<std::mutex> lock(_mutex);
        _condition.wait(lock,
            [this]{ return _stop || !_subtasks.empty(); });
        if(_stop || _subtasks.empty())
            return false;
        auto tt = const_cast<Task::subtask&>(_subtasks.top());
        subtask = std::move(tt);
        _subtasks.pop();
        return true;
    }

    void enqueueSubtask(Task::subtask& subtask) {
        if (_stop) {
            app->log("enqueue on stopped ThreadPool");
            return;
        }
        std::unique_lock<std::mutex> lock(_mutex);
        _subtasks.emplace(subtask);
        _condition.notify_one();
    }

    vector<pthread_t> _threads;
    std::priority_queue<Task::subtask> _subtasks;
    std::mutex _mutex;
    std::condition_variable _condition;
    std::atomic<bool> _stop;
    Task::exec_context _ctx;
    
};
}

// Tasks and threading
static ThreadPool* s_backgroundThreadPool;
static ThreadPool* s_ioThreadPool;


Task* Task::enqueue(const vector<Task::subtask>& subtasks) {
    Task* task = new Task(subtasks);
    task->enqueueNextSubtask(variant());
    return task;
}

void Task::subtask::exec() {
    auto& status = task->_status;
    if (status != Task::Cancelled) {
        status = Task::Executing;
        assert(_func);
        _output = _func(_input);
        if (task->_subtasks.size()) {
            task->enqueueNextSubtask(_output);
        } else {
            status = Task::Complete;
        }
    }
}

void Task::enqueueNextSubtask(const variant& input) {
    if (!s_backgroundThreadPool) {
        unsigned int ncores=0,nthreads=0,ht=0;
        asm volatile("cpuid": "=a" (ncores), "=b" (nthreads) : "a" (0xb), "c" (0x1) : );
        ht=(ncores!=nthreads);
        //printf("Cores: %d\nThreads: %d\nHyperThreading: %s\n",ncores,nthreads,ht?"Yes":"No");
        s_backgroundThreadPool = new ThreadPool(Task::Background, MAX(1,ncores-1));
        s_ioThreadPool = new ThreadPool(Task::IO, 2); // TODO: work out some kind of heuristic
    }
    
    // Detach next subtask from internal list and set its input
    Task::subtask subtask = _subtasks[0];
    _subtasks.erase(_subtasks.begin());
    subtask._input = input;
    
    //assert(_status == Created || _status == Executing);
    
    _status = Queued;
    switch(subtask._threadType) {
        case Task::Background:
            s_backgroundThreadPool->enqueueSubtask(subtask);
            break;
        case Task::IO:
            s_ioThreadPool->enqueueSubtask(subtask);
            break;
        case Task::MainThread:
            postToMainThread([=]() mutable {
                subtask.exec();
            });
            break;
    }
    
}

/*            // Thread affinity test
            thread_port_t mach_thread = pthread_mach_thread_np(_threads.back().native_handle());
            thread_affinity_policy_data_t policy = { 1<<(i+1) };
            kern_return_t k = thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY,
                              (thread_policy_t)&policy, 1);
            assert(k==0);*/



ThreadPool::~ThreadPool() {
    _stop = true;
    _condition.notify_all();
    /*for (auto& thread : _threads) {
        thread->join();
    }*/
}
 

/*
void Thread::post(std::function<void ()> callback) {
    _callbacks.push_back(callback);
}*/

#if PLATFORM_WEB
    worker_handle _worker;
    std::function<void()> _onStop;
#endif


    
Task::Task(const vector<subtask>& subtasks) {
    for (auto subtask : subtasks) {
        subtask.task = this;
        _subtasks.push_back(subtask);
    }
}
Task::~Task() {
    //app->log("~Task(%X : %d)", this, _runContext);
}





void Task::cancel() {
    if (_status == Cancelled || _status == Complete) {
        assert(_subtasks.size()==0);
        return;
    }
    _subtasks.clear();
    _status = Cancelled;
}



variant Task::fileLoad(int fd) {
    assert(!isMainThread());
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
variant Task::fileLoad(const string& path) {
    assert(!isMainThread());
    int fd = ::open(path.c_str(), O_RDONLY, 0);
    if (-1 == fd) {
        return variant(error::fromErrno());
    }
    return fileLoad(fd);
}

static void mkdir_rec(char* path) {
    char* p = path;
    while ((p = strchr(p+1, '/'))) {
        *p = 0;
        mkdir(path, S_IRWXU);
        *p = '/';
    }
}
error Task::fileSave(const string& path, const bytearray& data) {
    assert(!isMainThread());
    int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR);
    if (-1 == fd && 2==errno) {
        mkdir_rec((char*)path.c_str());
        fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR);
    }
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

