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
## 



## 



## 

| ` Worker(const <a href="classstring.html">string</a> & implName)` | Constructor. |
| ` ~Worker()` |  |
| `void start(const <a href="classvariant.html">variant</a> & config)` | Start the background thread, passing it some configuration. |
| `void process(const <a href="classvariant.html">variant</a> & data_in, std::function< void(const <a href="classvariant.html">variant</a> &)> callback)` | Send some work to the background thread, with a callback once the work completes. |
| `void stop(std::function< void()> onStop)` | Stop the background thread. |
| `void dispatchProcessResult(const <a href="classvariant.html">variant</a> & data_out)` |  |
| `void dispatchStopped()` |  |


## 

| ` Worker()` |  |


# Methods

| *Worker* |  ` <a href="todo">Worker</a>(const <a href="classstring.html">string</a> & implName)` | The string parameter is registered name of the background component |
| *~Worker* |  ` <a href="todo">~Worker</a>()` |  |
| *start* |  `void <a href="todo">start</a>(const <a href="classvariant.html">variant</a> & config)` |  |
| *process* |  `void <a href="todo">process</a>(const <a href="classvariant.html">variant</a> & data_in, std::function< void(const <a href="classvariant.html">variant</a> &)> callback)` |  |
| *stop* |  `void <a href="todo">stop</a>(std::function< void()> onStop)` |  |
| *dispatchProcessResult* |  `void <a href="todo">dispatchProcessResult</a>(const <a href="classvariant.html">variant</a> & data_out)` |  |
| *dispatchStopped* |  `void <a href="todo">dispatchStopped</a>()` |  |
| *Worker* |  ` <a href="todo">Worker</a>()` |  |
