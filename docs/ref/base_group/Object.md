---
layout: default
---

# Object

```
class Object

Base class for all reference-counted types.     

    
## 



## 

| ` Object()` |  |
| ` ~Object()` |  |
| `void retain()` | Increments the internal reference counter. |
| `void release()` | Decrements the internal reference counter. |
| `void * operator new(size_t sz)` |  |


## 

| `void flushAutodeletePool()` |  |
| `<a href="class_object.html">Object</a> * createByName(const <a href="classstring.html">string</a> & className)` |  |


# Methods

| *Object* |  ` <a href="todo">Object</a>()` |  |
| *~Object* |  ` <a href="todo">~Object</a>()` |  |
| *retain* |  `void <a href="todo">retain</a>()` | NB: Not threadsafe, cos we don't have threads. |
| *release* |  `void <a href="todo">release</a>()` | If the counter reaches zero the object is moved to a queue of objects that will be free()d between frames. NB: Not threadsafe. |
| *operator new* |  `void * <a href="todo">operator new</a>(size_t sz)` |  |
| *flushAutodeletePool* |  `void <a href="todo">flushAutodeletePool</a>()` |  |
| *createByName* |  `<a href="class_object.html">Object</a> * <a href="todo">createByName</a>(const <a href="classstring.html">string</a> & className)` |  |
