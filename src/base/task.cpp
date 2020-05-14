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

    ThreadPool(Task::exec_context ctx, int minThreads, int maxThreads) : _ctx(ctx), _minThreads(minThreads), _maxThreads(maxThreads) {
        _numThreadsRunning = 0;
        _threads = (pthread_t*)calloc(maxThreads, sizeof(pthread_t));
    }
    ~ThreadPool();
    
    
    static void* thread_func(void* arg) {
        auto init = (thread_init*)arg;
        int threadIndex = init->thread_index;
        ThreadPool* pool = init->pool;
        char name[64];
        const char* prefix;
        switch (pool->_ctx) {
            case Task::Background: prefix="Background"; break;
            case Task::IO: prefix="IO"; break;
            default: prefix="?"; break;
        }
        sprintf(name, "%s %d", prefix, threadIndex+1);
#if PLATFORM_APPLE
        pthread_setname_np(name);
#else
        pthread_setname_np(pool->_threads[threadIndex], name);
#endif
        delete init;

dontjudgeme:
        // Wait at most 5 seconds for something to do
        while (pool->_sem.wait(5000)) {

            // Remove subtask from head of subtask queue
            Task::subtask subtask;
            pool->_mutex.lock();
            assert(!pool->_subtasks.empty());
            auto tt = const_cast<Task::subtask&>(pool->_subtasks.top());
            subtask = std::move(tt);
            pool->_subtasks.pop();
            pool->_mutex.unlock();
            
            // Run the subtask
            subtask.exec();
            
            // Clean up any dead objects on this thread
            s_threadLocalData.flush();
        }
        
        // Wait timed out, exit the thread unless pool is already at minimum thread count
        pool->_mutex.lock();
        if (pool->_numThreadsRunning <= pool->_minThreads) {
            pool->_mutex.unlock();
            goto dontjudgeme;
        }
        
        // Remove the pthread from the pool
        pool->_numThreadsRunning--;
        pool->_threads[threadIndex] = nullptr;
        pool->_mutex.unlock();
        
        s_threadLocalData.flush();
        return nullptr;
    }

    void enqueueSubtask(Task::subtask& subtask) {
        _mutex.lock();
        if (_numThreadsRunning < _maxThreads) {
            for (int i=0 ; i<_maxThreads ; i++) {
                pthread_t& thread = _threads[i];
                if (thread == nullptr) {
                    auto init = new thread_init {this, i};
                    int status = pthread_create(&thread, NULL, thread_func, init);
                    assert(status == 0);
                    _numThreadsRunning++;
                    break;
                }
            }
        }
        _subtasks.emplace(subtask);
        _mutex.unlock();
        _sem.signal();
    }

    int _minThreads;
    int _maxThreads;
    int _numThreadsRunning;
    pthread_t* _threads;
    std::priority_queue<Task::subtask> _subtasks;
    std::mutex _mutex;
    semaphore _sem;
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
        unsigned int ncores=0;
#if defined(__x86_64__) || defined(__i386__) // For Intel we want to know core count, not thread count. Hyperthreading is a bit of a con.
        unsigned int nthreads=0,hyperthreaded=0;
        asm volatile("cpuid": "=a" (ncores), "=b" (nthreads) : "a" (0xb), "c" (0x1) : );
        hyperthreaded = (ncores!=nthreads);
#else
        ncores = std::thread::hardware_concurrency();
#endif
        s_backgroundThreadPool = new ThreadPool(Task::Background, 1, MAX(1,ncores-1));
        s_ioThreadPool = new ThreadPool(Task::IO, 2, (int)app->getStyleFloat("app.max-io-threads"));
    }
    
    // Detach next subtask from internal list and set its input
    postToMainThread([=] () {
        if (isCancelled()) {
            return;
        }
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
                subtask.exec();
                break;
        }
    });
    
}

/*            // Thread affinity test
            thread_port_t mach_thread = pthread_mach_thread_np(_threads.back().native_handle());
            thread_affinity_policy_data_t policy = { 1<<(i+1) };
            kern_return_t k = thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY,
                              (thread_policy_t)&policy, 1);
            assert(k==0);*/



ThreadPool::~ThreadPool() {
}
 

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
    //log("~Task(%X : %d)", this, _runContext);
}





void Task::cancel() {
    if (_status == Cancelled || _status == Complete) {
        assert(_subtasks.size()==0);
        return;
    }
    _status = Cancelled;
}

