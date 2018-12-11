---
layout: default
---

# FileStream

```
class FileStream
    : public Stream
```


A `Stream` backed by a `FILE*`.     

    
` FileStream(`[`string`](/ref/base_group/string)` path)`<br>

` ~FileStream()`<br>

`bool openForRead()`<br>

`bool openForWrite()`<br>

`void close()`<br>

`bool hasMoreToRead()`<br>

`bool writeBytes(size_t cb, const void* bytes)`<br>

`bool readBytes(size_t cb, void* bytes)`<br>

`void setWriteOffset(size_t offset)`<br>


