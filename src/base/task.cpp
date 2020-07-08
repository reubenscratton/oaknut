//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>
#include <sched.h>


namespace oak {
    static thread_local bool s_isMainThread;

    static struct MainThreadInit {
        MainThreadInit() {
            s_isMainThread = true;
        }
    } s_mainThreadInit;
}

bool Task::isMainThread() {
    return s_isMainThread;
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
        _numThreadsWaiting = 0;
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

        for (;;) {

            // Wait at most 5 seconds for something to do
            pool->_mutex.lock();
            pool->_numThreadsWaiting++;
            pool->_mutex.unlock();
            bool gotWork = pool->_sem.wait(5000);

            // Update wait count
            pool->_mutex.lock();
            pool->_numThreadsWaiting--;
                    
            // If there's no work (i.e. wait timed out) and thread is surplus to minimum requirement, break the loop
            if (!gotWork) {
                if (pool->_numThreadsRunning > pool->_minThreads) {
                    break;
                }
                pool->_mutex.unlock();
                continue;
            }

            // Remove subtask from head of subtask queue
            Task::subtask subtask;
            assert(!pool->_subtasks.empty());
            auto tt = const_cast<Task::subtask&>(pool->_subtasks.top());
            subtask = std::move(tt);
            pool->_subtasks.pop();
            pool->_mutex.unlock();
            
            // Run the subtask
            subtask.exec();
        }
        
        
        // Remove the pthread from the pool
        pool->_numThreadsRunning--;
        pool->_threads[threadIndex] = nullptr;
        pool->_mutex.unlock();
        
        return nullptr;
    }

    void enqueueSubtask(Task::subtask& subtask) {
        _mutex.lock();
        if (_numThreadsWaiting == 0) {
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
        }
        _subtasks.emplace(subtask);
        _mutex.unlock();
        _sem.signal();
    }

    int _minThreads;
    int _maxThreads;
    int _numThreadsRunning;
    int _numThreadsWaiting;
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


Task* Task::enqueue(const vector<Task::subtask>& subtasks, Object* owner/*=nullptr*/) {
    return new Task(subtasks, owner);
}

void Task::subtask::exec() {
    if (task->_status != Task::Cancelled) {
        task->_status = Task::Executing;
        assert(_func);
        _output = _func(_input);
    }
    task->enqueueNextSubtask(_output);
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
        s_ioThreadPool = new ThreadPool(Task::IO, 2, (int)style::get("App.max-io-threads")->intVal());
    }
    
    // Detach next subtask from internal list and set its input
    postToMainThread([=] () {
        
        // If task is complete or cancelled, release the owner, release the ref on ourself, and exit.
        if (isCancelled() || !_subtasks.size()) {
            _owner = nullptr;
            release();
            return;
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


    
Task::Task(const vector<subtask>& subtasks, Object* owner/*=nullptr*/) : _owner(owner) {
    for (auto subtask : subtasks) {
        subtask.task = this;
        _subtasks.push_back(subtask);
    }
    
    // Hold a ref on the Task object as long as the Task is executing
    retain();
    
    // Enqueue it
    enqueueNextSubtask(variant());
}
Task::~Task() {
    //log_dbg("~Task(%X : %d)", this, _runContext);
}





void Task::cancel() {
    if (_status == Cancelled || _status == Complete) {
        assert(_subtasks.size()==0);
        return;
    }
    _status = Cancelled;
}

