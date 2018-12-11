---
layout: default
---

# string

```
class string

An alternative to `stdstring`.     

This is not quite a drop-in replacement. It is the same old byte buffer under the hood but this one treats the bytes as a UTF-8 encoded string and indexing is done by character offset rather than byte offset.Why? Because UTF-8 is by far the most popular way to encode text and I don't like the memory wasted by wide characters. In my experience the vast majority of strings are short and the runtime cost of finding character boundaries is acceptably small. The code is optimised for when byte count and char count are the same, i.e. the string contains no multibyte characters.NB: The allocated buffer is always one char larger than _cb and _cc indicate, to contain a terminating null character.Future: Support UTF-32 as well via pointer-tagging. Let the encoding be a settable property of the string which just defaults to UTF-8.     
| | |
|-|-|
|` string()`||
|` string(const char * p)`||
|` string(const char * p, int32_t cb)`||
|` string(const `[`string`](/ref/base_group/string)` & str)`||
|` string(`[`string`](/ref/base_group/string)` && other)`||
|` ~string()`||
|`const char * data()`||
|`int32_t length()`||
|`int32_t lengthInBytes()`||
|`int compare(const `[`string`](/ref/base_group/string)` & str)`||
|`int compare(const char * s)`||
|`bool operator==(const `[`string`](/ref/base_group/string)` & rhs)`||
|`bool operator==(const char * rhs)`||
|`bool operator!=(const `[`string`](/ref/base_group/string)` & rhs)`||
|`bool operator!=(const char * rhs)`||
|`bool operator<(const `[`string`](/ref/base_group/string)` & rhs)`||
|`bool operator<(const char * rhs)`||
|`char32_t operator[](const int32_t charIndex)`||
|`char32_t charAt(const int32_t charIndex)`||
|[`string`](/ref/base_group/string)` substr(int32_t charIndexStart, int32_t charIndexEnd)`||
|`int32_t find(const `[`string`](/ref/base_group/string)` & str)`||
|`int32_t find(const `[`string`](/ref/base_group/string)` & str, int32_t start)`||
|`int32_t find(const char * s)`||
|`int32_t find(char32_t ch)`||
|`bool contains(const `[`string`](/ref/base_group/string)` & str)`||
|`bool contains(const char * s)`||
|`bool contains(char32_t ch)`||
|`void assign(const char * p, int32_t cb)`||
|[`string`](/ref/base_group/string)` & operator=(const `[`string`](/ref/base_group/string)` & str)`||
|[`string`](/ref/base_group/string)` & operator=(const char * s)`||
|`void append(const `[`string`](/ref/base_group/string)` & str)`||
|`void append(const char * s)`||
|`void append(const char * s, int32_t cb)`||
|`void append(char32_t ch)`||
|[`string`](/ref/base_group/string)` & operator+=(const `[`string`](/ref/base_group/string)` & str)`||
|[`string`](/ref/base_group/string)` & operator+=(const char * s)`||
|`void insert(int32_t charIndex, const `[`string`](/ref/base_group/string)` & str)`||
|`void insert(int32_t charIndex, const char * str)`||
|`void insert(int32_t charIndex, const char * str, int32_t cb)`||
|`void erase(int32_t charIndex)`||
|`void erase(int32_t charIndexStart, int32_t charIndexEnd)`||
|`void trim()`||
|`bool hasPrefix(const `[`string`](/ref/base_group/string)` & prefix)`||
|`bool hadPrefix(const `[`string`](/ref/base_group/string)` & prefix)`||
|`bool hasSuffix(const `[`string`](/ref/base_group/string)` & suffix)`||
|`bool hadSuffix(const `[`string`](/ref/base_group/string)` & suffix)`||
|` string(const iterator & start, const iterator & end)`||
|`iterator begin()`||
|`iterator end()`||
|[`string`](/ref/base_group/string)` extractUpTo(const `[`string`](/ref/base_group/string)` & sep, bool remove)`||
|[`string`](/ref/base_group/string)` tokenise(const `[`string`](/ref/base_group/string)` & sep)`||
|`size_t hash()`||
|[`string`](/ref/base_group/string)` lowercase()`||
|`vector< `[`string`](/ref/base_group/string)` > split(const `[`string`](/ref/base_group/string)` & delimiter)`||
|[`bytearray`](/ref/base_group/bytearray)` unhex()`||
|`int32_t asInt()`||
|[`string`](/ref/base_group/string)` urlEncode()`||
|[`bytearray`](/ref/base_group/bytearray)` toByteArray(bool copy)`||


| | |
|-|-|
|[`string`](/ref/base_group/string)` format(const char * fmt, ... )`||
|[`string`](/ref/base_group/string)` uuid()`||
|[`string`](/ref/base_group/string)` hex(const void * p, int32_t cb)`||


| | |
|-|-|
|`int32_t charIndexToByteIndex(int32_t charIndex)`||
|`char32_t readUtf8(int32_t & byteOffset)`||
|`void normaliseCharRange(int32_t & charIndexStart, int32_t & charIndexEnd)`||
|`void countChars()`||


## Methods

| | |
|-|-|
| *string* | ` string()` |  |
| *string* | ` string(const char * p)` |  |
| *string* | ` string(const char * p, int32_t cb)` |  |
| *string* | ` string(const `[`string`](/ref/base_group/string)` & str)` |  |
| *string* | ` string(`[`string`](/ref/base_group/string)` && other)` |  |
| *~string* | ` ~string()` |  |
| *data* | `const char * data()` |  |
| *length* | `int32_t length()` |  |
| *lengthInBytes* | `int32_t lengthInBytes()` |  |
| *compare* | `int compare(const `[`string`](/ref/base_group/string)` & str)` |  |
| *compare* | `int compare(const char * s)` |  |
| *operator==* | `bool operator==(const `[`string`](/ref/base_group/string)` & rhs)` |  |
| *operator==* | `bool operator==(const char * rhs)` |  |
| *operator!=* | `bool operator!=(const `[`string`](/ref/base_group/string)` & rhs)` |  |
| *operator!=* | `bool operator!=(const char * rhs)` |  |
| *operator<* | `bool operator<(const `[`string`](/ref/base_group/string)` & rhs)` |  |
| *operator<* | `bool operator<(const char * rhs)` |  |
| *operator[]* | `char32_t operator[](const int32_t charIndex)` |  |
| *charAt* | `char32_t charAt(const int32_t charIndex)` |  |
| *substr* | [`string`](/ref/base_group/string)` substr(int32_t charIndexStart, int32_t charIndexEnd)` |  |
| *find* | `int32_t find(const `[`string`](/ref/base_group/string)` & str)` |  |
| *find* | `int32_t find(const `[`string`](/ref/base_group/string)` & str, int32_t start)` |  |
| *find* | `int32_t find(const char * s)` |  |
| *find* | `int32_t find(char32_t ch)` |  |
| *contains* | `bool contains(const `[`string`](/ref/base_group/string)` & str)` |  |
| *contains* | `bool contains(const char * s)` |  |
| *contains* | `bool contains(char32_t ch)` |  |
| *assign* | `void assign(const char * p, int32_t cb)` |  |
| *operator=* | [`string`](/ref/base_group/string)` & operator=(const `[`string`](/ref/base_group/string)` & str)` |  |
| *operator=* | [`string`](/ref/base_group/string)` & operator=(const char * s)` |  |
| *append* | `void append(const `[`string`](/ref/base_group/string)` & str)` |  |
| *append* | `void append(const char * s)` |  |
| *append* | `void append(const char * s, int32_t cb)` |  |
| *append* | `void append(char32_t ch)` |  |
| *operator+=* | [`string`](/ref/base_group/string)` & operator+=(const `[`string`](/ref/base_group/string)` & str)` |  |
| *operator+=* | [`string`](/ref/base_group/string)` & operator+=(const char * s)` |  |
| *insert* | `void insert(int32_t charIndex, const `[`string`](/ref/base_group/string)` & str)` |  |
| *insert* | `void insert(int32_t charIndex, const char * str)` |  |
| *insert* | `void insert(int32_t charIndex, const char * str, int32_t cb)` |  |
| *erase* | `void erase(int32_t charIndex)` |  |
| *erase* | `void erase(int32_t charIndexStart, int32_t charIndexEnd)` |  |
| *trim* | `void trim()` |  |
| *hasPrefix* | `bool hasPrefix(const `[`string`](/ref/base_group/string)` & prefix)` |  |
| *hadPrefix* | `bool hadPrefix(const `[`string`](/ref/base_group/string)` & prefix)` |  |
| *hasSuffix* | `bool hasSuffix(const `[`string`](/ref/base_group/string)` & suffix)` |  |
| *hadSuffix* | `bool hadSuffix(const `[`string`](/ref/base_group/string)` & suffix)` |  |
| *string* | ` string(const iterator & start, const iterator & end)` |  |
| *begin* | `iterator begin()` |  |
| *end* | `iterator end()` |  |
| *extractUpTo* | [`string`](/ref/base_group/string)` extractUpTo(const `[`string`](/ref/base_group/string)` & sep, bool remove)` |  |
| *tokenise* | [`string`](/ref/base_group/string)` tokenise(const `[`string`](/ref/base_group/string)` & sep)` |  |
| *hash* | `size_t hash()` |  |
| *lowercase* | [`string`](/ref/base_group/string)` lowercase()` |  |
| *split* | `vector< `[`string`](/ref/base_group/string)` > split(const `[`string`](/ref/base_group/string)` & delimiter)` |  |
| *unhex* | [`bytearray`](/ref/base_group/bytearray)` unhex()` |  |
| *asInt* | `int32_t asInt()` |  |
| *urlEncode* | [`string`](/ref/base_group/string)` urlEncode()` |  |
| *toByteArray* | [`bytearray`](/ref/base_group/bytearray)` toByteArray(bool copy)` |  |
| *format* | [`string`](/ref/base_group/string)` format(const char * fmt, ... )` |  |
| *uuid* | [`string`](/ref/base_group/string)` uuid()` |  |
| *hex* | [`string`](/ref/base_group/string)` hex(const void * p, int32_t cb)` |  |
| *charIndexToByteIndex* | `int32_t charIndexToByteIndex(int32_t charIndex)` |  |
| *readUtf8* | `char32_t readUtf8(int32_t & byteOffset)` |  |
| *normaliseCharRange* | `void normaliseCharRange(int32_t & charIndexStart, int32_t & charIndexEnd)` |  |
| *countChars* | `void countChars()` |  |
