---
layout: default
---

# string

```
class string

An alternative to `stdstring`.     

This is not quite a drop-in replacement. It is the same old byte buffer under the hood but this one treats the bytes as a UTF-8 encoded string and indexing is done by character offset rather than byte offset.Why? Because UTF-8 is by far the most popular way to encode text and I don't like the memory wasted by wide characters. In my experience the vast majority of strings are short and the runtime cost of finding character boundaries is acceptably small. The code is optimised for when byte count and char count are the same, i.e. the string contains no multibyte characters.NB: The allocated buffer is always one char larger than _cb and _cc indicate, to contain a terminating null character.Future: Support UTF-32 as well via pointer-tagging. Let the encoding be a settable property of the string which just defaults to UTF-8.     
` string()`<br>

` string(const char* p)`<br>

` string(const char* p, int32_t cb)`<br>

` string(const `[`string`](/ref/base_group/string)` & str)`<br>

` string(`[`string`](/ref/base_group/string)` && other)`<br>

` ~string()`<br>

`const char* data()`<br>

`int32_t length()`<br>

`int32_t lengthInBytes()`<br>

`int compare(const `[`string`](/ref/base_group/string)` & str)`<br>

`int compare(const char* s)`<br>

`bool operator==(const `[`string`](/ref/base_group/string)` & rhs)`<br>

`bool operator==(const char* rhs)`<br>

`bool operator!=(const `[`string`](/ref/base_group/string)` & rhs)`<br>

`bool operator!=(const char* rhs)`<br>

`bool operator<(const `[`string`](/ref/base_group/string)` & rhs)`<br>

`bool operator<(const char* rhs)`<br>

`char32_t operator[](const int32_t charIndex)`<br>

`char32_t charAt(const int32_t charIndex)`<br>

[`string`](/ref/base_group/string)` substr(int32_t charIndexStart, int32_t charIndexEnd)`<br>

`int32_t find(const `[`string`](/ref/base_group/string)` & str)`<br>

`int32_t find(const `[`string`](/ref/base_group/string)` & str, int32_t start)`<br>

`int32_t find(const char* s)`<br>

`int32_t find(char32_t ch)`<br>

`bool contains(const `[`string`](/ref/base_group/string)` & str)`<br>

`bool contains(const char* s)`<br>

`bool contains(char32_t ch)`<br>

`void assign(const char* p, int32_t cb)`<br>

[`string`](/ref/base_group/string)` & operator=(const `[`string`](/ref/base_group/string)` & str)`<br>

[`string`](/ref/base_group/string)` & operator=(const char* s)`<br>

`void append(const `[`string`](/ref/base_group/string)` & str)`<br>

`void append(const char* s)`<br>

`void append(const char* s, int32_t cb)`<br>

`void append(char32_t ch)`<br>

[`string`](/ref/base_group/string)` & operator+=(const `[`string`](/ref/base_group/string)` & str)`<br>

[`string`](/ref/base_group/string)` & operator+=(const char* s)`<br>

`void insert(int32_t charIndex, const `[`string`](/ref/base_group/string)` & str)`<br>

`void insert(int32_t charIndex, const char* str)`<br>

`void insert(int32_t charIndex, const char* str, int32_t cb)`<br>

`void erase(int32_t charIndex)`<br>

`void erase(int32_t charIndexStart, int32_t charIndexEnd)`<br>

`void trim()`<br>

`bool hasPrefix(const `[`string`](/ref/base_group/string)` & prefix)`<br>

`bool hadPrefix(const `[`string`](/ref/base_group/string)` & prefix)`<br>

`bool hasSuffix(const `[`string`](/ref/base_group/string)` & suffix)`<br>

`bool hadSuffix(const `[`string`](/ref/base_group/string)` & suffix)`<br>

` string(const iterator & start, const iterator & end)`<br>

`iterator begin()`<br>

`iterator end()`<br>

[`string`](/ref/base_group/string)` extractUpTo(const `[`string`](/ref/base_group/string)` & sep, bool remove)`<br>

[`string`](/ref/base_group/string)` tokenise(const `[`string`](/ref/base_group/string)` & sep)`<br>

`size_t hash()`<br>

[`string`](/ref/base_group/string)` lowercase()`<br>

`vector< `[`string`](/ref/base_group/string)` > split(const `[`string`](/ref/base_group/string)` & delimiter)`<br>

[`bytearray`](/ref/base_group/bytearray)` unhex()`<br>

`int32_t asInt()`<br>

[`string`](/ref/base_group/string)` urlEncode()`<br>

[`bytearray`](/ref/base_group/bytearray)` toByteArray(bool copy)`<br>


[`string`](/ref/base_group/string)` format(const char* fmt, ... )`<br>

[`string`](/ref/base_group/string)` uuid()`<br>

[`string`](/ref/base_group/string)` hex(const void* p, int32_t cb)`<br>


`int32_t charIndexToByteIndex(int32_t charIndex)`<br>

`char32_t readUtf8(int32_t & byteOffset)`<br>

`void normaliseCharRange(int32_t & charIndexStart, int32_t & charIndexEnd)`<br>

`void countChars()`<br>


