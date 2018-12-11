---
layout: default
---

# Task

```
class Task
    : public Object
```


Any cancelable operation that performs a callback on the main thread when finished, unless the operation was cancelled.     

NB: Task is *not* an API for scheduling background app work, see `Worker` for that.`Task` is not intended for instantiation in app code, Tasks are typically created and returned by platform APIs. For example `Bitmap::createFromData()` returns a Task representing a background operation that decodes a bitmap from PNG or JPG data.     
` Task(std::function< void(void)> oncomplete)`<br>
`bool isCancelled()`<br>
`void complete()`<br>
`void cancel()`<br>

