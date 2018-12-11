---
layout: default
---

# ByteBufferStream

```
class ByteBufferStream
    : public Stream
```


A `Stream` that acts on a `ByteBuffer`.     

    
| | |
|-|-|
|` ByteBufferStream()`||
|` ByteBufferStream(int cb)`||
|` ByteBufferStream(`[`ByteBuffer`](/ref/data_group/ByteBuffer)` * data)`||
|[`bytearray`](/ref/base_group/bytearray)` getWrittenBytes()`||
|`bool hasMoreToRead()`||
|`bool writeBytes(size_t cb, const void * bytes)`||
|`bool readBytes(size_t cb, void * bytes)`||
|`void setWriteOffset(size_t offset)`||


## Methods

| | |
|-|-|
| *ByteBufferStream* | ` ByteBufferStream()` |  |
| *ByteBufferStream* | ` ByteBufferStream(int cb)` |  |
| *ByteBufferStream* | ` ByteBufferStream(`[`ByteBuffer`](/ref/data_group/ByteBuffer)` * data)` |  |
| *getWrittenBytes* | [`bytearray`](/ref/base_group/bytearray)` getWrittenBytes()` |  |
| *hasMoreToRead* | `bool hasMoreToRead()` |  |
| *writeBytes* | `bool writeBytes(size_t cb, const void * bytes)` |  |
| *readBytes* | `bool readBytes(size_t cb, void * bytes)` |  |
| *setWriteOffset* | `void setWriteOffset(size_t offset)` |  |
