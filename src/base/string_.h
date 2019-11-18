//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


/**
 * @ingroup base_group
 * @class string
 * @brief An friendly alternative to `std::string`.
 *
 * A primary goal of Oaknut is providing friendly and simple APIs that feel
 * familiar to those coming from Java/Obj-C/Swift app development. To that
 * end oak::string aims to be a performant equivalent for the string types
 * in those worlds.
 *
 * oak::strings use a null-terminated UTF-8 encoded byte buffer. Indexing
 * characters within the buffer is either done by character index (slow), or
 * by iterator (faster). The buffer is refcounted and can be shared between
 * multiple strings or substrings (ie strings returned by substr() or slice()).
 * NB: As with oak::Object, since Oaknut apps are generally single-threaded
 * there is no locking around the buffer refcounts. If you pass strings between
 * threads you should use an external locking scheme.
 *
 * oak::string uses copy-on-write. The modifying APIs insert(), append()
 * and erase() will create new buffers before returning. More specifically
 * if the existing
 *
 * Since strings can be substrings of a shared buffer they are not necessarily
 * null-terminated. For this reason the c_str() method will create a null-terminated
 * private buffer.
 *
 * NB: The allocated buffer is always one char larger than _cb, to contain
 * a terminating null character.
 *
 * Future: Support UTF-32 via a second flag bit. Let the encoding be a settable
 * property which defaults to UTF-8.
 */
class string {
public:
    
    // Construction
    string() : _buf(nullptr), _cb(0), _offset(0), _type(0) {}
    
    // Constructing from a raw char* creates a new buffer
    static inline string static_str(const char* s, uint32_t cb) {
        string str;
        str._buf = (char*)s;
        str._cb = cb;
        str._type = 1;
        return str;
    }
    template<size_t N>
    string(const char(&p)[N]) : _buf((char*)p), _cb(N-1), _offset(0), _type(1) { }
    //string(const char* p) :  string() { assign(p, p ? (int32_t)strlen(p) : 0);}
    string(const char* p, int32_t cb) :  string() { assign(p, cb); }
    
    // Constructing from an existing string does not create a copy
    string(const string& s);
    string(const string& s, uint32_t offsetFrom, uint32_t offsetTo);
    string(string&& other) noexcept :  _buf(other._buf),  _cb(other._cb), _offset(other._offset), _type(other._type) {
        other._buf = nullptr;
        other._cb = 0;
        other._offset = 0;
        other._type = 0;
    }

    ~string();
    
    static const string empty;
    
    
    // Buffer
    const char* c_str() const;
    int32_t length() const noexcept;
    int32_t lengthInBytes() const noexcept { return _cb; }

    void prepareToModify();

    
    // Equality
    bool operator==(const string& rhs) const;
    bool operator==(const char* rhs) const;
    bool operator!=(const string& rhs) const;
    bool operator!=(const char* rhs) const;
    explicit operator bool() const { return _cb>0; }
    
    // Collation (NB: locale sensitive)
    int compare(const string& str) const noexcept;
    int compare(const char* sz, uint32_t szlen=-1) const noexcept;
    bool operator<(const string& rhs) const;

    // Reading characters and substrings
    char32_t peekChar(uint32_t offset) const;
    char32_t readChar(uint32_t& offset) const;
    void rewind(uint32_t& offset) const; // moves backwards one char
    char32_t operator[](int32_t charIndex) const;
    char32_t charAt(int32_t charIndex) const;
    uint32_t charIndexToOffset(int32_t charIndex) const;
    string substr(uint32_t fromByteOffset, uint32_t toByteOffset=0xFFFFFFFFU) const;
    string substrAt(int32_t fromCharIndex, int32_t toCharIndex=0x7FFFFFFF) const;
    string slice(int32_t fromCharIndex, int32_t toCharIndex=0) const; // Python-style

    // Parsing
    void skipWhitespace(uint32_t& offset) const;
    bool skipChar(uint32_t& offset, char ch) const;
    bool skipString(uint32_t& offset, const char* s) const;
    string readUpTo(uint32_t& offset, const string& str) const;
    string readUpTo(uint32_t& offset, char ch) const;
    string readUpToOneOf(uint32_t& offset, const string& str) const;
    string readToken(uint32_t& offset) const;
    string readNumber(uint32_t& offset) const;
    string readIdentifier(uint32_t& offset) const;
    string readToEndOfLine(uint32_t& offset) const;

    // Search
    uint32_t find(const char* szToFind, int32_t cb=-1, uint32_t fromByteOffset=0) const;
    uint32_t find(const string& textToFind, uint32_t fromByteOffset=0) const { return find(textToFind.start(), textToFind._cb, fromByteOffset); }
    bool contains(const string& str) const { return find(str)<_cb; }
    bool hasPrefix(const string& prefix) const;
    bool hasSuffix(const string& suffix) const;

    // Modification. NB: All these APIs will invalidate existing ptrs/iterators and slices
    void clear();
    void assign(const char* p, int32_t cb);
    string& operator=(const string& str);
    string& operator=(const char* s);
    void append(const string& str) { append(str.start(), str._cb); }
    void append(const char* s, int32_t cb=-1) { insert(_cb, s, cb); }
    void append(char32_t ch);
    void prepend(const string& str) { insert(0, str); }
    string& operator+=(const string& str) { append(str); return *this; }
    string& operator+=(const char* s) { append(s); return *this; }
    friend string operator+(const string& lhs, const string& rhs);
    void insert(uint32_t byteOffset, const string& str) { insert(byteOffset, str.start(), str._cb); }
    void insert(uint32_t byteOffset, const char* str, int32_t cb=-1);
    void insertAt(int32_t charIndex, const string& str) { insert(charIndexToOffset(charIndex), str); }
    void erase(uint32_t fromByteOffset, uint32_t toByteOffset=0xFFFFFFFF);
    void eraseAt(int32_t fromCharIndex, int32_t toCharIndex=0x7FFFFFFF);
    void eraseLast();
    void trim();
    bool hadPrefix(const string& prefix);
    bool hadSuffix(const string& suffix);
    bool replace(const string& search, const string& replacement);

    
    uint32_t end() const {return _cb;}
    // STL Iterator API
    //inline uint32_t begin() const {return ptr(*this, _offset);}
    //inline ptr end() const {return ptr(*this, _offset+_cb);}
    /*inline ptr at(int32_t charIndex) const {
        uint32_t o = charIndexToOffset(charIndex);
        return ptr(*this, _offset + o);
    }*/


    // Utility API
    bool isOneOf(const vector<string>& vec) const;
    size_t hash() const;
    string lowercase() const;
    vector<string> split(const string& delimiter, int maxSplits=0) const;
    static string format(const char* fmt, ...);
    static string uuid();
    static string hex(const void* p, int32_t cb);
    static string join(const vector<string>& vec, const string& delimiter);
    bytearray unhex();
    int32_t asInt();
    float asFloat();
    string urlEncode();
    string urlDecode();
    bytearray toByteArray();
    static char32_t readUtf8(char* base, uint32_t& offset);

    friend class Stream;
    friend class ByteBuffer;
    friend class bytearray;
    
    
protected:
    mutable char* _buf; // 8 bytes
    uint32_t _cb; // 4 bytes
    struct {
        mutable uint32_t _offset:24;
        uint32_t _type:8;
    };
    inline char* start() const {
        return _buf+_offset;
    }

    friend string base64_encode(const string& str);
    
    void alloc(uint32_t cb);
    
};

string operator "" _S(const char *str, std::size_t len);
