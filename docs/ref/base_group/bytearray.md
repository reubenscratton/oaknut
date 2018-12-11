---
layout: default
---

# bytearray

```
class bytearray

An alternative to std::vector<uint8_t>     

A simple array of bytes as distinct from a string. Exists because I felt it ought to.     
` bytearray()`<br>

` bytearray(int32_t cb)`<br>

` bytearray(const uint8_t * p, int32_t cb)`<br>

` bytearray(const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & src)`<br>

` bytearray(`[`bytearray`](/oaknut/ref/base_group/bytearray)` && other)`<br>

` ~bytearray()`<br>

`uint8_t * data()`<br>

`int32_t size()`<br>

`int32_t length()`<br>

`void assign(const uint8_t * p, int32_t cb)`<br>

`void assignNoCopy(uint8_t * p, int32_t cb)`<br>

[`bytearray`](/oaknut/ref/base_group/bytearray)` & operator=(const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & str)`<br>

`void append(const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & str)`<br>

`void append(const uint8_t * p, int32_t cb)`<br>

`void append(uint8_t byte)`<br>

[`bytearray`](/oaknut/ref/base_group/bytearray)` & operator+=(const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & str)`<br>

`void insert(int32_t offset, const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & str)`<br>

`void insert(int32_t offset, const uint8_t * p, int32_t cb)`<br>

`void erase(int32_t offset)`<br>

`void erase(int32_t offsetStart, int32_t cb)`<br>

`void resize(int32_t newSize)`<br>

`void detach()`<br>

`class `[`string`](/oaknut/ref/base_group/string)` toString()`<br>



## Methods

| | |
|-|-|
| *append* | `void append(const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & str)` |  |
| *append* | `void append(const uint8_t * p, int32_t cb)` |  |
| *append* | `void append(uint8_t byte)` |  |
| *assign* | `void assign(const uint8_t * p, int32_t cb)` |  |
| *assignNoCopy* | `void assignNoCopy(uint8_t * p, int32_t cb)` |  |
| *bytearray* | ` bytearray()` |  |
| *bytearray* | ` bytearray(int32_t cb)` |  |
| *bytearray* | ` bytearray(const uint8_t * p, int32_t cb)` |  |
| *bytearray* | ` bytearray(const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & src)` |  |
| *bytearray* | ` bytearray(`[`bytearray`](/oaknut/ref/base_group/bytearray)` && other)` |  |
| *data* | `uint8_t * data()` |  |
| *detach* | `void detach()` |  |
| *erase* | `void erase(int32_t offset)` |  |
| *erase* | `void erase(int32_t offsetStart, int32_t cb)` |  |
| *insert* | `void insert(int32_t offset, const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & str)` |  |
| *insert* | `void insert(int32_t offset, const uint8_t * p, int32_t cb)` |  |
| *length* | `int32_t length()` |  |
| *operator+=* | [`bytearray`](/oaknut/ref/base_group/bytearray)` & operator+=(const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & str)` |  |
| *operator=* | [`bytearray`](/oaknut/ref/base_group/bytearray)` & operator=(const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & str)` |  |
| *resize* | `void resize(int32_t newSize)` |  |
| *size* | `int32_t size()` |  |
| *toString* | `class `[`string`](/oaknut/ref/base_group/string)` toString()` |  |
| *~bytearray* | ` ~bytearray()` |  |
