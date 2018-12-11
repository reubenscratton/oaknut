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

` ByteBufferStream(`[`ByteBuffer`](/ref/data_group/ByteBuffer)`* data)`<br>

[`bytearray`](/ref/base_group/bytearray)` getWrittenBytes()`<br>

`bool hasMoreToRead()`<br>

`bool writeBytes(size_t cb, const void* bytes)`<br>

`bool readBytes(size_t cb, void* bytes)`<br>

`void setWriteOffset(size_t offset)`<br>


