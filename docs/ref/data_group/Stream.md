---
layout: default
---

# Stream

```
class Stream
    : public Object
```


Abstract base class for a thing that can read and write elementary data.     

    
| | |
|-|-|
|` Stream()`||
|`bool hasMoreToRead()`||
|`bool writeBytes(size_t cb, const void * bytes)`||
|`bool readBytes(size_t cb, void * bytes)`||
|`void setWriteOffset(size_t offset)`||
|`bool readInt8(int8_t * val)`||
|`bool writeInt8(int8_t val)`||
|`bool readInt16(int16_t * val)`||
|`bool writeInt16(int16_t val)`||
|`bool readInt32(int32_t * val)`||
|`bool writeInt32(int32_t val)`||
|`bool readUint32(uint32_t * val)`||
|`bool writeUint32(uint32_t val)`||
|`bool readByteArray(`[`bytearray`](/ref/base_group/bytearray)` * ba)`||
|`bool writeByteArray(const `[`bytearray`](/ref/base_group/bytearray)` & ba)`||
|`bool readString(`[`string`](/ref/base_group/string)` * str)`||
|`bool writeString(const `[`string`](/ref/base_group/string)` & str)`||
|`bool readVariant(class variant * val)`||
|`bool writeVariant(const variant & val)`||


## Methods

| | |
|-|-|
| *Stream* | ` Stream()` |  |
| *hasMoreToRead* | `bool hasMoreToRead()` |  |
| *writeBytes* | `bool writeBytes(size_t cb, const void * bytes)` |  |
| *readBytes* | `bool readBytes(size_t cb, void * bytes)` |  |
| *setWriteOffset* | `void setWriteOffset(size_t offset)` |  |
| *readInt8* | `bool readInt8(int8_t * val)` |  |
| *writeInt8* | `bool writeInt8(int8_t val)` |  |
| *readInt16* | `bool readInt16(int16_t * val)` |  |
| *writeInt16* | `bool writeInt16(int16_t val)` |  |
| *readInt32* | `bool readInt32(int32_t * val)` |  |
| *writeInt32* | `bool writeInt32(int32_t val)` |  |
| *readUint32* | `bool readUint32(uint32_t * val)` |  |
| *writeUint32* | `bool writeUint32(uint32_t val)` |  |
| *readByteArray* | `bool readByteArray(`[`bytearray`](/ref/base_group/bytearray)` * ba)` |  |
| *writeByteArray* | `bool writeByteArray(const `[`bytearray`](/ref/base_group/bytearray)` & ba)` |  |
| *readString* | `bool readString(`[`string`](/ref/base_group/string)` * str)` |  |
| *writeString* | `bool writeString(const `[`string`](/ref/base_group/string)` & str)` |  |
| *readVariant* | `bool readVariant(class variant * val)` |  |
| *writeVariant* | `bool writeVariant(const variant & val)` |  |
