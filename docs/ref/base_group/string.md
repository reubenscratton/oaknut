---
layout: default
---

# string

```
class string

An alternative to `stdstring`.     

This is not quite a drop-in replacement. It is the same old byte buffer under the hood but this one treats the bytes as a UTF-8 encoded string and indexing is done by character offset rather than byte offset.Why? Because UTF-8 is by far the most popular way to encode text and I don't like the memory wasted by wide characters. In my experience the vast majority of strings are short and the runtime cost of finding character boundaries is acceptably small. The code is optimised for when byte count and char count are the same, i.e. the string contains no multibyte characters.NB: The allocated buffer is always one char larger than _cb and _cc indicate, to contain a terminating null character.Future: Support UTF-32 as well via pointer-tagging. Let the encoding be a settable property of the string which just defaults to UTF-8.     
## 



## 



## 

| ` string()` |  |
| ` string(const char * p)` |  |
| ` string(const char * p, int32_t cb)` |  |
| ` string(const <a href="classstring.html">string</a> & str)` |  |
| ` string(<a href="classstring.html">string</a> && other)` |  |
| ` ~string()` |  |
| `const char * data()` |  |
| `int32_t length()` |  |
| `int32_t lengthInBytes()` |  |
| `int compare(const <a href="classstring.html">string</a> & str)` |  |
| `int compare(const char * s)` |  |
| `bool operator==(const <a href="classstring.html">string</a> & rhs)` |  |
| `bool operator==(const char * rhs)` |  |
| `bool operator!=(const <a href="classstring.html">string</a> & rhs)` |  |
| `bool operator!=(const char * rhs)` |  |
| `bool operator<(const <a href="classstring.html">string</a> & rhs)` |  |
| `bool operator<(const char * rhs)` |  |
| `char32_t operator[](const int32_t charIndex)` |  |
| `char32_t charAt(const int32_t charIndex)` |  |
| `<a href="classstring.html">string</a> substr(int32_t charIndexStart, int32_t charIndexEnd)` |  |
| `int32_t find(const <a href="classstring.html">string</a> & str)` |  |
| `int32_t find(const <a href="classstring.html">string</a> & str, int32_t start)` |  |
| `int32_t find(const char * s)` |  |
| `int32_t find(char32_t ch)` |  |
| `bool contains(const <a href="classstring.html">string</a> & str)` |  |
| `bool contains(const char * s)` |  |
| `bool contains(char32_t ch)` |  |
| `void assign(const char * p, int32_t cb)` |  |
| `<a href="classstring.html">string</a> & operator=(const <a href="classstring.html">string</a> & str)` |  |
| `<a href="classstring.html">string</a> & operator=(const char * s)` |  |
| `void append(const <a href="classstring.html">string</a> & str)` |  |
| `void append(const char * s)` |  |
| `void append(const char * s, int32_t cb)` |  |
| `void append(char32_t ch)` |  |
| `<a href="classstring.html">string</a> & operator+=(const <a href="classstring.html">string</a> & str)` |  |
| `<a href="classstring.html">string</a> & operator+=(const char * s)` |  |
| `void insert(int32_t charIndex, const <a href="classstring.html">string</a> & str)` |  |
| `void insert(int32_t charIndex, const char * str)` |  |
| `void insert(int32_t charIndex, const char * str, int32_t cb)` |  |
| `void erase(int32_t charIndex)` |  |
| `void erase(int32_t charIndexStart, int32_t charIndexEnd)` |  |
| `void trim()` |  |
| `bool hasPrefix(const <a href="classstring.html">string</a> & prefix)` |  |
| `bool hadPrefix(const <a href="classstring.html">string</a> & prefix)` |  |
| `bool hasSuffix(const <a href="classstring.html">string</a> & suffix)` |  |
| `bool hadSuffix(const <a href="classstring.html">string</a> & suffix)` |  |
| ` string(const iterator & start, const iterator & end)` |  |
| `iterator begin()` |  |
| `iterator end()` |  |
| `<a href="classstring.html">string</a> extractUpTo(const <a href="classstring.html">string</a> & sep, bool remove)` |  |
| `<a href="classstring.html">string</a> tokenise(const <a href="classstring.html">string</a> & sep)` |  |
| `size_t hash()` |  |
| `<a href="classstring.html">string</a> lowercase()` |  |
| `vector< <a href="classstring.html">string</a> > split(const <a href="classstring.html">string</a> & delimiter)` |  |
| `<a href="classbytearray.html">bytearray</a> unhex()` |  |
| `int32_t asInt()` |  |
| `<a href="classstring.html">string</a> urlEncode()` |  |
| `<a href="classbytearray.html">bytearray</a> toByteArray(bool copy)` |  |


## 

| `<a href="classstring.html">string</a> format(const char * fmt, ... )` |  |
| `<a href="classstring.html">string</a> uuid()` |  |
| `<a href="classstring.html">string</a> hex(const void * p, int32_t cb)` |  |


## 

| `int32_t charIndexToByteIndex(int32_t charIndex)` |  |
| `char32_t readUtf8(int32_t & byteOffset)` |  |
| `void normaliseCharRange(int32_t & charIndexStart, int32_t & charIndexEnd)` |  |
| `void countChars()` |  |


# Methods

| *string* |  ` <a href="todo">string</a>()` |  |
| *string* |  ` <a href="todo">string</a>(const char * p)` |  |
| *string* |  ` <a href="todo">string</a>(const char * p, int32_t cb)` |  |
| *string* |  ` <a href="todo">string</a>(const <a href="classstring.html">string</a> & str)` |  |
| *string* |  ` <a href="todo">string</a>(<a href="classstring.html">string</a> && other)` |  |
| *~string* |  ` <a href="todo">~string</a>()` |  |
| *data* |  `const char * <a href="todo">data</a>()` |  |
| *length* |  `int32_t <a href="todo">length</a>()` |  |
| *lengthInBytes* |  `int32_t <a href="todo">lengthInBytes</a>()` |  |
| *compare* |  `int <a href="todo">compare</a>(const <a href="classstring.html">string</a> & str)` |  |
| *compare* |  `int <a href="todo">compare</a>(const char * s)` |  |
| *operator==* |  `bool <a href="todo">operator==</a>(const <a href="classstring.html">string</a> & rhs)` |  |
| *operator==* |  `bool <a href="todo">operator==</a>(const char * rhs)` |  |
| *operator!=* |  `bool <a href="todo">operator!=</a>(const <a href="classstring.html">string</a> & rhs)` |  |
| *operator!=* |  `bool <a href="todo">operator!=</a>(const char * rhs)` |  |
| *operator<* |  `bool <a href="todo">operator<</a>(const <a href="classstring.html">string</a> & rhs)` |  |
| *operator<* |  `bool <a href="todo">operator<</a>(const char * rhs)` |  |
| *operator[]* |  `char32_t <a href="todo">operator[]</a>(const int32_t charIndex)` |  |
| *charAt* |  `char32_t <a href="todo">charAt</a>(const int32_t charIndex)` |  |
| *substr* |  `<a href="classstring.html">string</a> <a href="todo">substr</a>(int32_t charIndexStart, int32_t charIndexEnd)` |  |
| *find* |  `int32_t <a href="todo">find</a>(const <a href="classstring.html">string</a> & str)` |  |
| *find* |  `int32_t <a href="todo">find</a>(const <a href="classstring.html">string</a> & str, int32_t start)` |  |
| *find* |  `int32_t <a href="todo">find</a>(const char * s)` |  |
| *find* |  `int32_t <a href="todo">find</a>(char32_t ch)` |  |
| *contains* |  `bool <a href="todo">contains</a>(const <a href="classstring.html">string</a> & str)` |  |
| *contains* |  `bool <a href="todo">contains</a>(const char * s)` |  |
| *contains* |  `bool <a href="todo">contains</a>(char32_t ch)` |  |
| *assign* |  `void <a href="todo">assign</a>(const char * p, int32_t cb)` |  |
| *operator=* |  `<a href="classstring.html">string</a> & <a href="todo">operator=</a>(const <a href="classstring.html">string</a> & str)` |  |
| *operator=* |  `<a href="classstring.html">string</a> & <a href="todo">operator=</a>(const char * s)` |  |
| *append* |  `void <a href="todo">append</a>(const <a href="classstring.html">string</a> & str)` |  |
| *append* |  `void <a href="todo">append</a>(const char * s)` |  |
| *append* |  `void <a href="todo">append</a>(const char * s, int32_t cb)` |  |
| *append* |  `void <a href="todo">append</a>(char32_t ch)` |  |
| *operator+=* |  `<a href="classstring.html">string</a> & <a href="todo">operator+=</a>(const <a href="classstring.html">string</a> & str)` |  |
| *operator+=* |  `<a href="classstring.html">string</a> & <a href="todo">operator+=</a>(const char * s)` |  |
| *insert* |  `void <a href="todo">insert</a>(int32_t charIndex, const <a href="classstring.html">string</a> & str)` |  |
| *insert* |  `void <a href="todo">insert</a>(int32_t charIndex, const char * str)` |  |
| *insert* |  `void <a href="todo">insert</a>(int32_t charIndex, const char * str, int32_t cb)` |  |
| *erase* |  `void <a href="todo">erase</a>(int32_t charIndex)` |  |
| *erase* |  `void <a href="todo">erase</a>(int32_t charIndexStart, int32_t charIndexEnd)` |  |
| *trim* |  `void <a href="todo">trim</a>()` |  |
| *hasPrefix* |  `bool <a href="todo">hasPrefix</a>(const <a href="classstring.html">string</a> & prefix)` |  |
| *hadPrefix* |  `bool <a href="todo">hadPrefix</a>(const <a href="classstring.html">string</a> & prefix)` |  |
| *hasSuffix* |  `bool <a href="todo">hasSuffix</a>(const <a href="classstring.html">string</a> & suffix)` |  |
| *hadSuffix* |  `bool <a href="todo">hadSuffix</a>(const <a href="classstring.html">string</a> & suffix)` |  |
| *string* |  ` <a href="todo">string</a>(const iterator & start, const iterator & end)` |  |
| *begin* |  `iterator <a href="todo">begin</a>()` |  |
| *end* |  `iterator <a href="todo">end</a>()` |  |
| *extractUpTo* |  `<a href="classstring.html">string</a> <a href="todo">extractUpTo</a>(const <a href="classstring.html">string</a> & sep, bool remove)` |  |
| *tokenise* |  `<a href="classstring.html">string</a> <a href="todo">tokenise</a>(const <a href="classstring.html">string</a> & sep)` |  |
| *hash* |  `size_t <a href="todo">hash</a>()` |  |
| *lowercase* |  `<a href="classstring.html">string</a> <a href="todo">lowercase</a>()` |  |
| *split* |  `vector< <a href="classstring.html">string</a> > <a href="todo">split</a>(const <a href="classstring.html">string</a> & delimiter)` |  |
| *unhex* |  `<a href="classbytearray.html">bytearray</a> <a href="todo">unhex</a>()` |  |
| *asInt* |  `int32_t <a href="todo">asInt</a>()` |  |
| *urlEncode* |  `<a href="classstring.html">string</a> <a href="todo">urlEncode</a>()` |  |
| *toByteArray* |  `<a href="classbytearray.html">bytearray</a> <a href="todo">toByteArray</a>(bool copy)` |  |
| *format* |  `<a href="classstring.html">string</a> <a href="todo">format</a>(const char * fmt, ... )` |  |
| *uuid* |  `<a href="classstring.html">string</a> <a href="todo">uuid</a>()` |  |
| *hex* |  `<a href="classstring.html">string</a> <a href="todo">hex</a>(const void * p, int32_t cb)` |  |
| *charIndexToByteIndex* |  `int32_t <a href="todo">charIndexToByteIndex</a>(int32_t charIndex)` |  |
| *readUtf8* |  `char32_t <a href="todo">readUtf8</a>(int32_t & byteOffset)` |  |
| *normaliseCharRange* |  `void <a href="todo">normaliseCharRange</a>(int32_t & charIndexStart, int32_t & charIndexEnd)` |  |
| *countChars* |  `void <a href="todo">countChars</a>()` |  |
