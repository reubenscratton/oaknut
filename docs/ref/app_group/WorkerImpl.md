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
| | |
|-|-|
|`void start_(const variant & config)`|Corresponds to Worker::start()|
|`variant process_(const variant & data_in)`|Corresponds to Worker::process()|
|`void stop_()`|Corresponds to Worker::stop()|


## Methods

| | |
|-|-|
| *start_* | `void start_(const variant & config)` |  |
| *process_* | `variant process_(const variant & data_in)` |  |
| *stop_* | `void stop_()` |  |
