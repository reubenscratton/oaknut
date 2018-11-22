//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 A 'worker' can be regarded as a distinct process that consumes and produces binary data.
 Like a thread but without shared memory, the inputs and outputs are serialized.
 
 Worker exists to provide a common abstraction of background processing that will work on web
 as well as it does natively. If the W3C and/or browser makers ever restore SharedArrayBuffer
 to the WebWorker context then we can revisit this cos pthreads will be usable everywhere then.
 
 */

class Worker : public Object {
public:
    
    // The client (app) API
    Worker(const string& implName);
    ~Worker();
    virtual void start(const variant& config);
    virtual void process(const variant& data_in,
                         std::function<void(const variant&)> callback);
    virtual void stop(std::function<void()> onStop);
    

#if PLATFORM_APPLE
    dispatch_queue_t _queue;
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
};


// The worker API. App code must not try to call these.

class WorkerImpl : public Object {
public:
    
    virtual void start_(const variant& config);
    virtual variant process_(const variant& data_in)=0;
    virtual void stop_();

};

#if PLATFORM_WEB
# if BUILD_AS_WORKER
#  define DECLARE_WORKER_IMPL(x) x* g_worker=new x();
# else
#  define DECLARE_WORKER_IMPL(x)
# endif
#else
#define DECLARE_WORKER_IMPL(x) DECLARE_DYNCREATE(x)
#endif

