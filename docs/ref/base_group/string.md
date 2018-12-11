---
layout: default
---

# string

```
class string

An alternative to `stdstring`.     

This is not quite a drop-in replacement. It is the same old byte buffer under the hood but this one treats the bytes as a UTF-8 encoded string and indexing is done by character offset rather than byte offset.Why? Because UTF-8 is by far the most popular way to encode text and I don't like the memory wasted by wide characters. In my experience the vast majority of strings are short and the runtime cost of finding character boundaries is acceptably small. The code is optimised for when byte count and char count are the same, i.e. the string contains no multibyte characters.NB: The allocated buffer is always one char larger than _cb and _cc indicate, to contain a terminating null character.Future: Support UTF-32 as well via pointer-tagging. Let the encoding be a settable property of the string which just defaults to UTF-8.     
` string()`<br>

` string(const char * p)`<br>

` string(const char * p, int32_t cb)`<br>

` string(const `[`string`](/oaknut/ref/base_group/string)` & str)`<br>

` string(`[`string`](/oaknut/ref/base_group/string)` && other)`<br>

` ~string()`<br>

`const char * data()`<br>

`int32_t length()`<br>

`int32_t lengthInBytes()`<br>

`int compare(const `[`string`](/oaknut/ref/base_group/string)` & str)`<br>

`int compare(const char * s)`<br>

`bool operator==(const `[`string`](/oaknut/ref/base_group/string)` & rhs)`<br>

`bool operator==(const char * rhs)`<br>

`bool operator!=(const `[`string`](/oaknut/ref/base_group/string)` & rhs)`<br>

`bool operator!=(const char * rhs)`<br>

`bool operator<(const `[`string`](/oaknut/ref/base_group/string)` & rhs)`<br>

`bool operator<(const char * rhs)`<br>

`char32_t operator[](const int32_t charIndex)`<br>

`char32_t charAt(const int32_t charIndex)`<br>

[`string`](/oaknut/ref/base_group/string)` substr(int32_t charIndexStart, int32_t charIndexEnd)`<br>

`int32_t find(const `[`string`](/oaknut/ref/base_group/string)` & str)`<br>

`int32_t find(const `[`string`](/oaknut/ref/base_group/string)` & str, int32_t start)`<br>

`int32_t find(const char * s)`<br>

`int32_t find(char32_t ch)`<br>

`bool contains(const `[`string`](/oaknut/ref/base_group/string)` & str)`<br>

`bool contains(const char * s)`<br>

`bool contains(char32_t ch)`<br>

`void assign(const char * p, int32_t cb)`<br>

[`string`](/oaknut/ref/base_group/string)` & operator=(const `[`string`](/oaknut/ref/base_group/string)` & str)`<br>

[`string`](/oaknut/ref/base_group/string)` & operator=(const char * s)`<br>

`void append(const `[`string`](/oaknut/ref/base_group/string)` & str)`<br>

`void append(const char * s)`<br>

`void append(const char * s, int32_t cb)`<br>

`void append(char32_t ch)`<br>

[`string`](/oaknut/ref/base_group/string)` & operator+=(const `[`string`](/oaknut/ref/base_group/string)` & str)`<br>

[`string`](/oaknut/ref/base_group/string)` & operator+=(const char * s)`<br>

`void insert(int32_t charIndex, const `[`string`](/oaknut/ref/base_group/string)` & str)`<br>

`void insert(int32_t charIndex, const char * str)`<br>

`void insert(int32_t charIndex, const char * str, int32_t cb)`<br>

`void erase(int32_t charIndex)`<br>

`void erase(int32_t charIndexStart, int32_t charIndexEnd)`<br>

`void trim()`<br>

`bool hasPrefix(const `[`string`](/oaknut/ref/base_group/string)` & prefix)`<br>

`bool hadPrefix(const `[`string`](/oaknut/ref/base_group/string)` & prefix)`<br>

`bool hasSuffix(const `[`string`](/oaknut/ref/base_group/string)` & suffix)`<br>

`bool hadSuffix(const `[`string`](/oaknut/ref/base_group/string)` & suffix)`<br>

` string(const iterator & start, const iterator & end)`<br>

`iterator begin()`<br>

`iterator end()`<br>

[`string`](/oaknut/ref/base_group/string)` extractUpTo(const `[`string`](/oaknut/ref/base_group/string)` & sep, bool remove)`<br>

[`string`](/oaknut/ref/base_group/string)` tokenise(const `[`string`](/oaknut/ref/base_group/string)` & sep)`<br>

`size_t hash()`<br>

[`string`](/oaknut/ref/base_group/string)` lowercase()`<br>

`vector< `[`string`](/oaknut/ref/base_group/string)` > split(const `[`string`](/oaknut/ref/base_group/string)` & delimiter)`<br>

[`bytearray`](/oaknut/ref/base_group/bytearray)` unhex()`<br>

`int32_t asInt()`<br>

[`string`](/oaknut/ref/base_group/string)` urlEncode()`<br>

[`bytearray`](/oaknut/ref/base_group/bytearray)` toByteArray(bool copy)`<br>



[`string`](/oaknut/ref/base_group/string)` format(const char * fmt, ... )`<br>

[`string`](/oaknut/ref/base_group/string)` uuid()`<br>

[`string`](/oaknut/ref/base_group/string)` hex(const void * p, int32_t cb)`<br>



`int32_t charIndexToByteIndex(int32_t charIndex)`<br>

`char32_t readUtf8(int32_t & byteOffset)`<br>

`void normaliseCharRange(int32_t & charIndexStart, int32_t & charIndexEnd)`<br>

`void countChars()`<br>



## Methods

| | |
|-|-|
| *append* | `void append(const `[`string`](/oaknut/ref/base_group/string)` & str)` |  |
| *append* | `void append(const char * s)` |  |
| *append* | `void append(const char * s, int32_t cb)` |  |
| *append* | `void append(char32_t ch)` |  |
| *asInt* | `int32_t asInt()` |  |
| *assign* | `void assign(const char * p, int32_t cb)` |  |
| *begin* | `iterator begin()` |  |
| *charAt* | `char32_t charAt(const int32_t charIndex)` |  |
| *charIndexToByteIndex* | `int32_t charIndexToByteIndex(int32_t charIndex)` |  |
| *compare* | `int compare(const `[`string`](/oaknut/ref/base_group/string)` & str)` |  |
| *compare* | `int compare(const char * s)` |  |
| *contains* | `bool contains(const `[`string`](/oaknut/ref/base_group/string)` & str)` |  |
| *contains* | `bool contains(const char * s)` |  |
| *contains* | `bool contains(char32_t ch)` |  |
| *countChars* | `void countChars()` |  |
| *data* | `const char * data()` |  |
| *end* | `iterator end()` |  |
| *erase* | `void erase(int32_t charIndex)` |  |
| *erase* | `void erase(int32_t charIndexStart, int32_t charIndexEnd)` |  |
| *extractUpTo* | [`string`](/oaknut/ref/base_group/string)` extractUpTo(const `[`string`](/oaknut/ref/base_group/string)` & sep, bool remove)` |  |
| *find* | `int32_t find(const `[`string`](/oaknut/ref/base_group/string)` & str)` |  |
| *find* | `int32_t find(const `[`string`](/oaknut/ref/base_group/string)` & str, int32_t start)` |  |
| *find* | `int32_t find(const char * s)` |  |
| *find* | `int32_t find(char32_t ch)` |  |
| *format* | [`string`](/oaknut/ref/base_group/string)` format(const char * fmt, ... )` |  |
| *hadPrefix* | `bool hadPrefix(const `[`string`](/oaknut/ref/base_group/string)` & prefix)` |  |
| *hadSuffix* | `bool hadSuffix(const `[`string`](/oaknut/ref/base_group/string)` & suffix)` |  |
| *hasPrefix* | `bool hasPrefix(const `[`string`](/oaknut/ref/base_group/string)` & prefix)` |  |
| *hasSuffix* | `bool hasSuffix(const `[`string`](/oaknut/ref/base_group/string)` & suffix)` |  |
| *hash* | `size_t hash()` |  |
| *hex* | [`string`](/oaknut/ref/base_group/string)` hex(const void * p, int32_t cb)` |  |
| *insert* | `void insert(int32_t charIndex, const `[`string`](/oaknut/ref/base_group/string)` & str)` |  |
| *insert* | `void insert(int32_t charIndex, const char * str)` |  |
| *insert* | `void insert(int32_t charIndex, const char * str, int32_t cb)` |  |
| *length* | `int32_t length()` |  |
| *lengthInBytes* | `int32_t lengthInBytes()` |  |
| *lowercase* | [`string`](/oaknut/ref/base_group/string)` lowercase()` |  |
| *normaliseCharRange* | `void normaliseCharRange(int32_t & charIndexStart, int32_t & charIndexEnd)` |  |
| *operator!=* | `bool operator!=(const `[`string`](/oaknut/ref/base_group/string)` & rhs)` |  |
| *operator!=* | `bool operator!=(const char * rhs)` |  |
| *operator+=* | [`string`](/oaknut/ref/base_group/string)` & operator+=(const `[`string`](/oaknut/ref/base_group/string)` & str)` |  |
| *operator+=* | [`string`](/oaknut/ref/base_group/string)` & operator+=(const char * s)` |  |
| *operator<* | `bool operator<(const `[`string`](/oaknut/ref/base_group/string)` & rhs)` |  |
| *operator<* | `bool operator<(const char * rhs)` |  |
| *operator=* | [`string`](/oaknut/ref/base_group/string)` & operator=(const `[`string`](/oaknut/ref/base_group/string)` & str)` |  |
| *operator=* | [`string`](/oaknut/ref/base_group/string)` & operator=(const char * s)` |  |
| *operator==* | `bool operator==(const `[`string`](/oaknut/ref/base_group/string)` & rhs)` |  |
| *operator==* | `bool operator==(const char * rhs)` |  |
| *operator[]* | `char32_t operator[](const int32_t charIndex)` |  |
| *readUtf8* | `char32_t readUtf8(int32_t & byteOffset)` |  |
| *split* | `vector< `[`string`](/oaknut/ref/base_group/string)` > split(const `[`string`](/oaknut/ref/base_group/string)` & delimiter)` |  |
| *string* | ` string()` |  |
| *string* | ` string(const char * p)` |  |
| *string* | ` string(const char * p, int32_t cb)` |  |
| *string* | ` string(const `[`string`](/oaknut/ref/base_group/string)` & str)` |  |
| *string* | ` string(`[`string`](/oaknut/ref/base_group/string)` && other)` |  |
| *string* | ` string(const iterator & start, const iterator & end)` |  |
| *substr* | [`string`](/oaknut/ref/base_group/string)` substr(int32_t charIndexStart, int32_t charIndexEnd)` |  |
| *toByteArray* | [`bytearray`](/oaknut/ref/base_group/bytearray)` toByteArray(bool copy)` |  |
| *tokenise* | [`string`](/oaknut/ref/base_group/string)` tokenise(const `[`string`](/oaknut/ref/base_group/string)` & sep)` |  |
| *trim* | `void trim()` |  |
| *unhex* | [`bytearray`](/oaknut/ref/base_group/bytearray)` unhex()` |  |
| *urlEncode* | [`string`](/oaknut/ref/base_group/string)` urlEncode()` |  |
| *uuid* | [`string`](/oaknut/ref/base_group/string)` uuid()` |  |
| *~string* | ` ~string()` |  |
