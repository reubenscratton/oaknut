---
layout: default
---

# Object

```
class Object

Base class for all reference-counted types.     

    
| | |
|-|-|
|` Object()`||
|` ~Object()`||
|`void retain()`|Increments the internal reference counter.|
|`void release()`|Decrements the internal reference counter.|
|`void * operator new(size_t sz)`||


| | |
|-|-|
|`void flushAutodeletePool()`||
|[`Object`](/ref/base_group/Object)` * createByName(const `[`string`](/ref/base_group/string)` & className)`||


## Methods

| | |
|-|-|
| *Object* | ` Object()` |  |
| *~Object* | ` ~Object()` |  |
| *retain* | `void retain()` | NB: Not threadsafe, cos we don't have threads. |
| *release* | `void release()` | If the counter reaches zero the object is moved to a queue of objects that will be free()d between frames. NB: Not threadsafe. |
| *operator new* | `void * operator new(size_t sz)` |  |
| *flushAutodeletePool* | `void flushAutodeletePool()` |  |
| *createByName* | [`Object`](/ref/base_group/Object)` * createByName(const `[`string`](/ref/base_group/string)` & className)` |  |
