//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


/**
 * @ingroup app_group
 * @class Worker
 * @brief A 'worker' is a dynamically-creatable consumer of background tasks.
 * It exists to provide a common abstraction of background processing that works
 * on both web and native platforms.
 *
 * Generally speaking, a Worker is an implementation detail hidden behind
 * some API.
 *
 * A worker has two sides: the client (in-app) side and the background
 * thread side. The client side must extend the Worker class and will generally provide
 * a friendly async API to application code, hiding the serialization and other
 * ugliness. The background side extends WorkerImpl and must be invisible to
 * the application. It is strictly a number cruncher.
 */

class Worker : public Object {
public:
    
    /** Constructor. The string parameter is registered name of the background component */
    Worker(const string& implName);
    ~Worker();
    
    /** Submit a piece of work for background processing */
    Task* enqueue(int32_t msg, const variant& data_in,
                         std::function<void(const variant&)> callback);
    
    
protected:
    
#if PLATFORM_WEB
    worker_handle _worker;
    std::function<void()> _onStop;
#else
    class WorkerImpl* _impl;
#endif
    
    void dispatchProcessResult(const variant& data_out);

};


/**
 * @ingroup app_group
 * @class WorkerImpl
 * @brief Base class for the background component of a Worker. Application code should not
 use this class directly, it is solely for implementing Workers.
*/

class WorkerImpl : public Object {
public:
    
    virtual variant process_(const variant& data_in)=0;

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

