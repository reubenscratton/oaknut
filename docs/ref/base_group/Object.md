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

`void release()`<br>Decrements the internal reference counter.

`void * operator new(size_t sz)`<br>



`void flushAutodeletePool()`<br>

[`Object`](/oaknut/ref/base_group/Object)` * createByName(const `[`string`](/oaknut/ref/base_group/string)` & className)`<br>



## Methods

| | |
|-|-|
| *Object* | ` Object()` |  |
| *createByName* | [`Object`](/oaknut/ref/base_group/Object)` * createByName(const `[`string`](/oaknut/ref/base_group/string)` & className)` |  |
| *flushAutodeletePool* | `void flushAutodeletePool()` |  |
| *operator new* | `void * operator new(size_t sz)` |  |
| *release* | `void release()` | If the counter reaches zero the object is moved to a queue of objects that will be free()d between frames. NB: Not threadsafe. |
| *retain* | `void retain()` | NB: Not threadsafe, cos we don't have threads. |
| *~Object* | ` ~Object()` |  |
