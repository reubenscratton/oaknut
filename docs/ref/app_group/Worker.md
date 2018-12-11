---
layout: default
---

# Worker

```
class Worker
    : public Object
```


A 'worker' is a background thread that processes serialized input and produces serialized output.     

It is based on the Javascript API of the same name. Every worker has two components, the client (in-app) side and the background thread side. The `Worker` class is the client side that application code uses to instantiate and send and receive work from the background thread.Worker exists to provide a common abstraction of background processing that will work on web as well as it does natively. If the W3C and/or browser makers ever restore SharedArrayBuffer to the WebWorker context then we can revisit this cos pthreads will be usable everywhere then.     
` Worker(const `[`string`](/oaknut/ref/base_group/string)` & implName)`<br>Constructor.

` ~Worker()`<br>

`void start(const variant & config)`<br>Start the background thread, passing it some configuration.

`void process(const variant & data_in, std::function< void(const variant &)> callback)`<br>Send some work to the background thread, with a callback once the work completes.

`void stop(std::function< void()> onStop)`<br>Stop the background thread.

`void dispatchProcessResult(const variant & data_out)`<br>

`void dispatchStopped()`<br>



` Worker()`<br>



## Methods

| | |
|-|-|
| *Worker* | ` Worker(const `[`string`](/oaknut/ref/base_group/string)` & implName)` | The string parameter is registered name of the background component |
| *Worker* | ` Worker()` |  |
| *dispatchProcessResult* | `void dispatchProcessResult(const variant & data_out)` |  |
| *dispatchStopped* | `void dispatchStopped()` |  |
| *process* | `void process(const variant & data_in, std::function< void(const variant &)> callback)` |  |
| *start* | `void start(const variant & config)` |  |
| *stop* | `void stop(std::function< void()> onStop)` |  |
| *~Worker* | ` ~Worker()` |  |
