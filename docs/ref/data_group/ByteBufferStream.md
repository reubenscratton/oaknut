---
layout: default
---

# ByteBufferStream

```
class ByteBufferStream
    : public Stream
```


A `Stream` that acts on a `ByteBuffer`.     

    
` ByteBufferStream()`<br>

` ByteBufferStream(int cb)`<br>

` ByteBufferStream(`[`ByteBuffer`](/oaknut/ref/data_group/ByteBuffer)` * data)`<br>

[`bytearray`](/oaknut/ref/base_group/bytearray)` getWrittenBytes()`<br>

`bool hasMoreToRead()`<br>

`bool writeBytes(size_t cb, const void * bytes)`<br>

`bool readBytes(size_t cb, void * bytes)`<br>

`void setWriteOffset(size_t offset)`<br>



## Methods

| | |
|-|-|
| *ByteBufferStream* | ` ByteBufferStream()` |  |
| *ByteBufferStream* | ` ByteBufferStream(int cb)` |  |
| *ByteBufferStream* | ` ByteBufferStream(`[`ByteBuffer`](/oaknut/ref/data_group/ByteBuffer)` * data)` |  |
| *getWrittenBytes* | [`bytearray`](/oaknut/ref/base_group/bytearray)` getWrittenBytes()` |  |
| *hasMoreToRead* | `bool hasMoreToRead()` |  |
| *readBytes* | `bool readBytes(size_t cb, void * bytes)` |  |
| *setWriteOffset* | `void setWriteOffset(size_t offset)` |  |
| *writeBytes* | `bool writeBytes(size_t cb, const void * bytes)` |  |
