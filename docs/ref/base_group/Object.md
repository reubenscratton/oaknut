---
layout: default
---

# Object

```
class Object

Base class for all reference-counted types.     

    
` Object()`<br>

` ~Object()`<br>

`void retain()`<br>Increments the internal reference counter.
NB: Not threadsafe, cos we don't have threads.

`void release()`<br>Decrements the internal reference counter.
If the counter reaches zero the object is moved to a queue of objects that will be free()d between frames. NB: Not threadsafe.

`void* operator new(size_t sz)`<br>


`void flushAutodeletePool()`<br>

[`Object`](/ref/base_group/Object)`* createByName(const `[`string`](/ref/base_group/string)` & className)`<br>


