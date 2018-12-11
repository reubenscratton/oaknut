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
`void start_(const variant & config)`<br>Corresponds to Worker::start()

`variant process_(const variant & data_in)`<br>Corresponds to Worker::process()

`void stop_()`<br>Corresponds to Worker::stop()


