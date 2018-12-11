---
layout: default
---

# ByteBuffer

```
class ByteBuffer
    : public Object, 
public ISerializable
```


Simple wrapper for a refcounted contiguous array of bytes, analogous to NSData on iOS or byte[] in Java.     

    
## 



## 

| ` ByteBuffer()` |  |
| ` ByteBuffer(size_t cb)` |  |
| ` ByteBuffer(uint8_t * data, size_t cb, bool copy)` |  |
| ` ByteBuffer(const <a href="class_byte_buffer.html">ByteBuffer</a> & data)` |  |
| ` ByteBuffer(const <a href="classstring.html">string</a> & str)` |  |
| ` ~ByteBuffer()` |  |
| `void set(const void * rawData, size_t len)` |  |
| `void attach(uint8_t * data, size_t cb)` |  |
| `void detach()` |  |
| `void saveToFile(const <a href="classstring.html">string</a> & path)` |  |
| `<a href="classstring.html">string</a> toString(bool copy)` |  |
| `bool readSelfFromStream(<a href="class_stream.html">Stream</a> * stream)` |  |
| `bool writeSelfToStream(<a href="class_stream.html">Stream</a> * stream)` |  |


## 

| `<a href="class_byte_buffer.html">ByteBuffer</a> * createFromFile(const <a href="classstring.html">string</a> & path)` |  |


# Methods

| *ByteBuffer* |  ` <a href="todo">ByteBuffer</a>()` |  |
| *ByteBuffer* |  ` <a href="todo">ByteBuffer</a>(size_t cb)` |  |
| *ByteBuffer* |  ` <a href="todo">ByteBuffer</a>(uint8_t * data, size_t cb, bool copy)` |  |
| *ByteBuffer* |  ` <a href="todo">ByteBuffer</a>(const <a href="class_byte_buffer.html">ByteBuffer</a> & data)` |  |
| *ByteBuffer* |  ` <a href="todo">ByteBuffer</a>(const <a href="classstring.html">string</a> & str)` |  |
| *~ByteBuffer* |  ` <a href="todo">~ByteBuffer</a>()` |  |
| *set* |  `void <a href="todo">set</a>(const void * rawData, size_t len)` |  |
| *attach* |  `void <a href="todo">attach</a>(uint8_t * data, size_t cb)` |  |
| *detach* |  `void <a href="todo">detach</a>()` |  |
| *saveToFile* |  `void <a href="todo">saveToFile</a>(const <a href="classstring.html">string</a> & path)` |  |
| *toString* |  `<a href="classstring.html">string</a> <a href="todo">toString</a>(bool copy)` |  |
| *readSelfFromStream* |  `bool <a href="todo">readSelfFromStream</a>(<a href="class_stream.html">Stream</a> * stream)` |  |
| *writeSelfToStream* |  `bool <a href="todo">writeSelfToStream</a>(<a href="class_stream.html">Stream</a> * stream)` |  |
| *createFromFile* |  `<a href="class_byte_buffer.html">ByteBuffer</a> * <a href="todo">createFromFile</a>(const <a href="classstring.html">string</a> & path)` |  |
