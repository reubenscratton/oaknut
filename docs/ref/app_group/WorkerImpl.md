---
layout: default
---

# WorkerImpl

```
class WorkerImpl
    : public Object
```


Base class for the background thread of a Worker.     

Application code should not use this class directly, it is solely for implementing Workers.     
## 

| `void start_(const <a href="classvariant.html">variant</a> & config)` | Corresponds to Worker::start() |
| `<a href="classvariant.html">variant</a> process_(const <a href="classvariant.html">variant</a> & data_in)` | Corresponds to Worker::process() |
| `void stop_()` | Corresponds to Worker::stop() |


# Methods

| *start_* |  `void <a href="todo">start_</a>(const <a href="classvariant.html">variant</a> & config)` |  |
| *process_* |  `<a href="classvariant.html">variant</a> <a href="todo">process_</a>(const <a href="classvariant.html">variant</a> & data_in)` |  |
| *stop_* |  `void <a href="todo">stop_</a>()` |  |
