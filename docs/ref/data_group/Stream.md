---
layout: default
---

# Stream

```
class Stream
    : public Object
```


Abstract base class for a thing that can read and write elementary data.     

    
` Stream()`<br>
`bool hasMoreToRead()`<br>
`bool writeBytes(size_t cb, const void* bytes)`<br>
`bool readBytes(size_t cb, void* bytes)`<br>
`void setWriteOffset(size_t offset)`<br>
`bool readInt8(int8_t* val)`<br>
`bool writeInt8(int8_t val)`<br>
`bool readInt16(int16_t* val)`<br>
`bool writeInt16(int16_t val)`<br>
`bool readInt32(int32_t* val)`<br>
`bool writeInt32(int32_t val)`<br>
`bool readUint32(uint32_t* val)`<br>
`bool writeUint32(uint32_t val)`<br>
`bool readByteArray(`[`bytearray`](/oaknut/ref/base_group/bytearray)`* ba)`<br>
`bool writeByteArray(const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & ba)`<br>
`bool readString(`[`string`](/oaknut/ref/base_group/string)`* str)`<br>
`bool writeString(const `[`string`](/oaknut/ref/base_group/string)` & str)`<br>
`bool readVariant(class variant* val)`<br>
`bool writeVariant(const variant & val)`<br>

