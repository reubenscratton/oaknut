---
layout: default
---

# bytearray

```
class bytearray

An alternative to std::vector<uint8_t>     

A simple array of bytes as distinct from a string. Exists because I felt it ought to.     
| | |
|-|-|
|` bytearray()`||
|` bytearray(int32_t cb)`||
|` bytearray(const uint8_t * p, int32_t cb)`||
|` bytearray(const `[`bytearray`](/ref/base_group/bytearray)` & src)`||
|` bytearray(`[`bytearray`](/ref/base_group/bytearray)` && other)`||
|` ~bytearray()`||
|`uint8_t * data()`||
|`int32_t size()`||
|`int32_t length()`||
|`void assign(const uint8_t * p, int32_t cb)`||
|`void assignNoCopy(uint8_t * p, int32_t cb)`||
|[`bytearray`](/ref/base_group/bytearray)` & operator=(const `[`bytearray`](/ref/base_group/bytearray)` & str)`||
|`void append(const `[`bytearray`](/ref/base_group/bytearray)` & str)`||
|`void append(const uint8_t * p, int32_t cb)`||
|`void append(uint8_t byte)`||
|[`bytearray`](/ref/base_group/bytearray)` & operator+=(const `[`bytearray`](/ref/base_group/bytearray)` & str)`||
|`void insert(int32_t offset, const `[`bytearray`](/ref/base_group/bytearray)` & str)`||
|`void insert(int32_t offset, const uint8_t * p, int32_t cb)`||
|`void erase(int32_t offset)`||
|`void erase(int32_t offsetStart, int32_t cb)`||
|`void resize(int32_t newSize)`||
|`void detach()`||
|`class `[`string`](/ref/base_group/string)` toString()`||


## Methods

| | |
|-|-|
| *bytearray* | ` bytearray()` |  |
| *bytearray* | ` bytearray(int32_t cb)` |  |
| *bytearray* | ` bytearray(const uint8_t * p, int32_t cb)` |  |
| *bytearray* | ` bytearray(const `[`bytearray`](/ref/base_group/bytearray)` & src)` |  |
| *bytearray* | ` bytearray(`[`bytearray`](/ref/base_group/bytearray)` && other)` |  |
| *~bytearray* | ` ~bytearray()` |  |
| *data* | `uint8_t * data()` |  |
| *size* | `int32_t size()` |  |
| *length* | `int32_t length()` |  |
| *assign* | `void assign(const uint8_t * p, int32_t cb)` |  |
| *assignNoCopy* | `void assignNoCopy(uint8_t * p, int32_t cb)` |  |
| *operator=* | [`bytearray`](/ref/base_group/bytearray)` & operator=(const `[`bytearray`](/ref/base_group/bytearray)` & str)` |  |
| *append* | `void append(const `[`bytearray`](/ref/base_group/bytearray)` & str)` |  |
| *append* | `void append(const uint8_t * p, int32_t cb)` |  |
| *append* | `void append(uint8_t byte)` |  |
| *operator+=* | [`bytearray`](/ref/base_group/bytearray)` & operator+=(const `[`bytearray`](/ref/base_group/bytearray)` & str)` |  |
| *insert* | `void insert(int32_t offset, const `[`bytearray`](/ref/base_group/bytearray)` & str)` |  |
| *insert* | `void insert(int32_t offset, const uint8_t * p, int32_t cb)` |  |
| *erase* | `void erase(int32_t offset)` |  |
| *erase* | `void erase(int32_t offsetStart, int32_t cb)` |  |
| *resize* | `void resize(int32_t newSize)` |  |
| *detach* | `void detach()` |  |
| *toString* | `class `[`string`](/ref/base_group/string)` toString()` |  |
