//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup app_group
 * @class Worker
 * @brief A 'worker' is a background thread that processes serialized input and produces
 serialized output. It is based on the Javascript API of the same name. Every worker
 has two components, the client (in-app) side and the background thread side. The
 `Worker` class is the client side that application code uses to instantiate and send
 and receive work from the background thread.
 
 Worker exists to provide a common abstraction of background processing that will work on web
 as well as it does natively. If the W3C and/or browser makers ever restore SharedArrayBuffer
 to the WebWorker context then we can revisit this cos pthreads will be usable everywhere then.
 
 */

class Worker : public Object {
public:
    
    /** Constructor. The string parameter is registered name of the background component*/
    Worker(const string& implName);
    ~Worker();
    
    /** Start the background thread, passing it some configuration */
    virtual void start(const variant& config);
    
    /** Send some work to the background thread, with a callback once the work completes */
    virtual void process(const variant& data_in,
                         std::function<void(const variant&)> callback);
    
    /** Stop the background thread */
    virtual void stop(std::function<void()> onStop);
    

#if PLATFORM_APPLE
    dispatch_queue_t _queue;
#elif PLATFORM_ANDROID || PLATFORM_LINUX
    sp<PosixTaskQueue> _queue;
#endif
#if PLATFORM_WEB
    worker_handle _worker;
    std::function<void()> _onStop;
    list<std::function<void(const variant&)>> _pending;
#else
    class WorkerImpl* _impl;
#endif
    
    void dispatchProcessResult(const variant& data_out);
    void dispatchStopped();
    
protected:
    Worker(); // for special types of worker only
    bool _started;

};


/**
 * @ingroup app_group
 * @class WorkerImpl
 * @brief Base class for the background thread of a Worker. Application code should not
 use this class directly, it is solely for implementing Workers.
*/

class WorkerImpl : public Object {
public:
    
    /** Corresponds to Worker::start() */
    virtual void start_(const variant& config);

    /** Corresponds to Worker::process() */
    virtual variant process_(const variant& data_in)=0;

    /** Corresponds to Worker::stop() */
    virtual void stop_();

};

#if PLATFORM_WEB
# if BUILD_AS_WORKER
#  define DECLARE_WORKER_IMPL(clazz, name) clazz* g_worker=new clazz();
# else
#  define DECLARE_WORKER_IMPL(clazz, name)
# endif
#else
#define DECLARE_WORKER_IMPL(clazz) DECLARE_DYNCREATE(clazz)
#endif

