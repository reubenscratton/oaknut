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
` bytearray(const uint8_t* p, int32_t cb)`<br>
` bytearray(const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & src)`<br>
` bytearray(`[`bytearray`](/oaknut/ref/base_group/bytearray)` && other)`<br>
` ~bytearray()`<br>
`uint8_t* data()`<br>
`int32_t size()`<br>
`int32_t length()`<br>
`void assign(const uint8_t* p, int32_t cb)`<br>
`void assignNoCopy(uint8_t* p, int32_t cb)`<br>
[`bytearray`](/oaknut/ref/base_group/bytearray)` & operator=(const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & str)`<br>
`void append(const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & str)`<br>
`void append(const uint8_t* p, int32_t cb)`<br>
`void append(uint8_t byte)`<br>
[`bytearray`](/oaknut/ref/base_group/bytearray)` & operator+=(const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & str)`<br>
`void insert(int32_t offset, const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & str)`<br>
`void insert(int32_t offset, const uint8_t* p, int32_t cb)`<br>
`void erase(int32_t offset)`<br>
`void erase(int32_t offsetStart, int32_t cb)`<br>
`void resize(int32_t newSize)`<br>
`void detach()`<br>
`class `[`string`](/oaknut/ref/base_group/string)` toString()`<br>

