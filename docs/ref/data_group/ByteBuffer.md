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

    
| | |
|-|-|
|` ByteBuffer()`||
|` ByteBuffer(size_t cb)`||
|` ByteBuffer(uint8_t * data, size_t cb, bool copy)`||
|` ByteBuffer(const `[`ByteBuffer`](/ref/data_group/ByteBuffer)` & data)`||
|` ByteBuffer(const `[`string`](/ref/base_group/string)` & str)`||
|` ~ByteBuffer()`||
|`void set(const void * rawData, size_t len)`||
|`void attach(uint8_t * data, size_t cb)`||
|`void detach()`||
|`void saveToFile(const `[`string`](/ref/base_group/string)` & path)`||
|[`string`](/ref/base_group/string)` toString(bool copy)`||
|`bool readSelfFromStream(`[`Stream`](/ref/data_group/Stream)` * stream)`||
|`bool writeSelfToStream(`[`Stream`](/ref/data_group/Stream)` * stream)`||


| | |
|-|-|
|[`ByteBuffer`](/ref/data_group/ByteBuffer)` * createFromFile(const `[`string`](/ref/base_group/string)` & path)`||


## Methods

| | |
|-|-|
| *ByteBuffer* | ` ByteBuffer()` |  |
| *ByteBuffer* | ` ByteBuffer(size_t cb)` |  |
| *ByteBuffer* | ` ByteBuffer(uint8_t * data, size_t cb, bool copy)` |  |
| *ByteBuffer* | ` ByteBuffer(const `[`ByteBuffer`](/ref/data_group/ByteBuffer)` & data)` |  |
| *ByteBuffer* | ` ByteBuffer(const `[`string`](/ref/base_group/string)` & str)` |  |
| *~ByteBuffer* | ` ~ByteBuffer()` |  |
| *set* | `void set(const void * rawData, size_t len)` |  |
| *attach* | `void attach(uint8_t * data, size_t cb)` |  |
| *detach* | `void detach()` |  |
| *saveToFile* | `void saveToFile(const `[`string`](/ref/base_group/string)` & path)` |  |
| *toString* | [`string`](/ref/base_group/string)` toString(bool copy)` |  |
| *readSelfFromStream* | `bool readSelfFromStream(`[`Stream`](/ref/data_group/Stream)` * stream)` |  |
| *writeSelfToStream* | `bool writeSelfToStream(`[`Stream`](/ref/data_group/Stream)` * stream)` |  |
| *createFromFile* | [`ByteBuffer`](/ref/data_group/ByteBuffer)` * createFromFile(const `[`string`](/ref/base_group/string)` & path)` |  |
