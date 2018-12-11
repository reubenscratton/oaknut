---
layout: default
---

# bytearray

```
class bytearray

An alternative to std::vector<uint8_t>     

A simple array of bytes as distinct from a string. Exists because I felt it ought to.     
## 



## 

| ` bytearray()` |  |
| ` bytearray(int32_t cb)` |  |
| ` bytearray(const uint8_t * p, int32_t cb)` |  |
| ` bytearray(const <a href="classbytearray.html">bytearray</a> & src)` |  |
| ` bytearray(<a href="classbytearray.html">bytearray</a> && other)` |  |
| ` ~bytearray()` |  |
| `uint8_t * data()` |  |
| `int32_t size()` |  |
| `int32_t length()` |  |
| `void assign(const uint8_t * p, int32_t cb)` |  |
| `void assignNoCopy(uint8_t * p, int32_t cb)` |  |
| `<a href="classbytearray.html">bytearray</a> & operator=(const <a href="classbytearray.html">bytearray</a> & str)` |  |
| `void append(const <a href="classbytearray.html">bytearray</a> & str)` |  |
| `void append(const uint8_t * p, int32_t cb)` |  |
| `void append(uint8_t byte)` |  |
| `<a href="classbytearray.html">bytearray</a> & operator+=(const <a href="classbytearray.html">bytearray</a> & str)` |  |
| `void insert(int32_t offset, const <a href="classbytearray.html">bytearray</a> & str)` |  |
| `void insert(int32_t offset, const uint8_t * p, int32_t cb)` |  |
| `void erase(int32_t offset)` |  |
| `void erase(int32_t offsetStart, int32_t cb)` |  |
| `void resize(int32_t newSize)` |  |
| `void detach()` |  |
| `class <a href="classstring.html">string</a> toString()` |  |


# Methods

| *bytearray* |  ` <a href="todo">bytearray</a>()` |  |
| *bytearray* |  ` <a href="todo">bytearray</a>(int32_t cb)` |  |
| *bytearray* |  ` <a href="todo">bytearray</a>(const uint8_t * p, int32_t cb)` |  |
| *bytearray* |  ` <a href="todo">bytearray</a>(const <a href="classbytearray.html">bytearray</a> & src)` |  |
| *bytearray* |  ` <a href="todo">bytearray</a>(<a href="classbytearray.html">bytearray</a> && other)` |  |
| *~bytearray* |  ` <a href="todo">~bytearray</a>()` |  |
| *data* |  `uint8_t * <a href="todo">data</a>()` |  |
| *size* |  `int32_t <a href="todo">size</a>()` |  |
| *length* |  `int32_t <a href="todo">length</a>()` |  |
| *assign* |  `void <a href="todo">assign</a>(const uint8_t * p, int32_t cb)` |  |
| *assignNoCopy* |  `void <a href="todo">assignNoCopy</a>(uint8_t * p, int32_t cb)` |  |
| *operator=* |  `<a href="classbytearray.html">bytearray</a> & <a href="todo">operator=</a>(const <a href="classbytearray.html">bytearray</a> & str)` |  |
| *append* |  `void <a href="todo">append</a>(const <a href="classbytearray.html">bytearray</a> & str)` |  |
| *append* |  `void <a href="todo">append</a>(const uint8_t * p, int32_t cb)` |  |
| *append* |  `void <a href="todo">append</a>(uint8_t byte)` |  |
| *operator+=* |  `<a href="classbytearray.html">bytearray</a> & <a href="todo">operator+=</a>(const <a href="classbytearray.html">bytearray</a> & str)` |  |
| *insert* |  `void <a href="todo">insert</a>(int32_t offset, const <a href="classbytearray.html">bytearray</a> & str)` |  |
| *insert* |  `void <a href="todo">insert</a>(int32_t offset, const uint8_t * p, int32_t cb)` |  |
| *erase* |  `void <a href="todo">erase</a>(int32_t offset)` |  |
| *erase* |  `void <a href="todo">erase</a>(int32_t offsetStart, int32_t cb)` |  |
| *resize* |  `void <a href="todo">resize</a>(int32_t newSize)` |  |
| *detach* |  `void <a href="todo">detach</a>()` |  |
| *toString* |  `class <a href="classstring.html">string</a> <a href="todo">toString</a>()` |  |
