---
layout: default
---

# FileStream

```
class FileStream
    : public Stream
```


A `Stream` backed by a `FILE*`.     

    
` FileStream(`[`string`](/oaknut/ref/base_group/string)` path)`<br>

` ~FileStream()`<br>

`bool openForRead()`<br>

`bool openForWrite()`<br>

`void close()`<br>

`bool hasMoreToRead()`<br>

`bool writeBytes(size_t cb, const void * bytes)`<br>

`bool readBytes(size_t cb, void * bytes)`<br>

`void setWriteOffset(size_t offset)`<br>



## Methods

| | |
|-|-|
| *FileStream* | ` FileStream(`[`string`](/oaknut/ref/base_group/string)` path)` |  |
| *close* | `void close()` |  |
| *hasMoreToRead* | `bool hasMoreToRead()` |  |
| *openForRead* | `bool openForRead()` |  |
| *openForWrite* | `bool openForWrite()` |  |
| *readBytes* | `bool readBytes(size_t cb, void * bytes)` |  |
| *setWriteOffset* | `void setWriteOffset(size_t offset)` |  |
| *writeBytes* | `bool writeBytes(size_t cb, const void * bytes)` |  |
| *~FileStream* | ` ~FileStream()` |  |
