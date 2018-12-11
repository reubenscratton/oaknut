---
layout: default
---

# FileStream

```
class FileStream
    : public Stream
```


A `Stream` backed by a `FILE*`.     

    
| | |
|-|-|
|` FileStream(`[`string`](/ref/base_group/string)` path)`||
|` ~FileStream()`||
|`bool openForRead()`||
|`bool openForWrite()`||
|`void close()`||
|`bool hasMoreToRead()`||
|`bool writeBytes(size_t cb, const void * bytes)`||
|`bool readBytes(size_t cb, void * bytes)`||
|`void setWriteOffset(size_t offset)`||


## Methods

| | |
|-|-|
| *FileStream* | ` FileStream(`[`string`](/ref/base_group/string)` path)` |  |
| *~FileStream* | ` ~FileStream()` |  |
| *openForRead* | `bool openForRead()` |  |
| *openForWrite* | `bool openForWrite()` |  |
| *close* | `void close()` |  |
| *hasMoreToRead* | `bool hasMoreToRead()` |  |
| *writeBytes* | `bool writeBytes(size_t cb, const void * bytes)` |  |
| *readBytes* | `bool readBytes(size_t cb, void * bytes)` |  |
| *setWriteOffset* | `void setWriteOffset(size_t offset)` |  |
