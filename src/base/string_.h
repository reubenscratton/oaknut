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
    string() : _buf(nullptr), _cb(0), _offset(0) {}
    
    // Constructing from a raw char* creates a new buffer
    string(const char* p) :  string() { assign(p, p ? (int32_t)strlen(p) : 0);}
    string(const char* p, int32_t cb) :  string() { assign(p, cb); }
    
    // Constructing from an existing string does not create a copy
    string(const string& s) : _buf(s._buf), _cb(s._cb), _offset(s._offset) { if (_buf) buf_retain(_buf);  }
    string(const string& s, uint32_t offsetFrom, uint32_t offsetTo) : _buf(s._buf), _cb(offsetTo-offsetFrom), _offset(s._offset+offsetFrom) { buf_retain(_buf);  }
    string(string&& other) noexcept :  _buf(other._buf), _offset(other._offset), _cb(other._cb) {
        other._buf = nullptr;
        other._cb = 0;
        other._offset = 0;
    }

    ~string() {
        if (_buf) {
            buf_release(_buf);
        }
    }
    
    static const uint32_t eof;
    
    struct bufhdr {
        uint32_t refs;
        uint32_t cb;
        uint32_t cap;
    };
    inline struct bufhdr* bufhdr() const {
        return _buf ? (((struct bufhdr*)_buf)-1) : nullptr;
    }
    
    // Buffer
    inline const char* c_str() const {
        
        // If this buffer is a substring (i.e. end() is less than the buffer's end) then
        // create a new buffer/
        if (_buf) {
            auto hdr = bufhdr();
            if (_offset+_cb < hdr->cb) {
                auto newbuf = buf_new(_cb);
                memcpy(newbuf, start(), _cb);
                newbuf[_cb] = '\0';
                _offset = 0;
                buf_release(_buf);
                _buf = newbuf;
            }
        }
        return _buf;
    }
    int32_t length() const noexcept;
    int32_t lengthInBytes() const noexcept { return _cb; }

    static inline char* buf_new(uint32_t cb);
    static inline void buf_retain(char* buf);
    static inline void buf_release(char* buf);
    inline void buf_realloc(int32_t cbDelta);
    void prepareToModify();

    // Pointer ("ptr") is a UTF-8 iterator with some additional methods for parsing and extracting text
    /*class ptr : public std::iterator<
        std::input_iterator_tag,   // iterator_category
        char32_t,                      // value_type
        char32_t,                      // difference_type
        const char32_t*,               // pointer
        char32_t                       // reference
        > {
    public:
        ptr(const string& s, uint32_t offset) : _buf(s._buf), _offsetBase(s._offset), _offset(offset), _cb(s._cb) { buf_retain(_buf); }
        ptr(const ptr& p) :  _buf(p._buf), _offsetBase(p._offsetBase), _offset(p._offset), _cb(p._cb) { buf_retain(_buf); }
        ptr(ptr&& p) noexcept :  _buf(p._buf), _offsetBase(p._offsetBase), _offset(p._offset), _cb(p._cb) { p._buf=nullptr; }
        ~ptr() { if (_buf) buf_release(_buf); }
        ptr& operator=(const ptr& p) {
            buf_release(_buf);
            _buf = p._buf;
            buf_retain(_buf);
            _offsetBase=p._offsetBase;
            _offset=p._offset;
            return *this;
        }
        ptr& operator++() { readUtf8(base(), _offset); return *this; }
        ptr operator++(int);
        friend ptr operator+(ptr lhs, int32_t rhs) { lhs._offset += rhs; return lhs; }
        friend ptr operator-(ptr lhs, int32_t rhs) { lhs._offset -= rhs; return lhs; }
        ptr& operator+=(int32_t dBytes) { _offset += dBytes; return *this; }
        ptr& operator-=(int32_t dBytes) { _offset -= dBytes; return *this; }
        reference operator*() { auto o=_offset; auto ch = readUtf8(base(), o); return ch; }
        bool operator==(const ptr& p) const { return (current() == p.current()); }
        bool operator!=(const ptr& p) const { return !(current() == p.current()); }
        //operator const char*() const {  return current(); }
        //operator uint32_t() { return _offset; }

        char32_t next();
        char32_t peek();
        bool eof() const { return _offset>=_cb;}
                                        
    private:
        char* _buf;
        uint32_t _offsetBase;
        uint32_t _offset;
        uint32_t _cb;

        friend class string;
        inline char* base() const { return _buf + _offsetBase; }
        inline char* current() const { return _buf + _offsetBase + _offset; }
    };*/

    
    // Equality
    bool operator==(const string& rhs) const;
    bool operator!=(const string& rhs) const;
    explicit operator bool() const { return _cb>0; }
    
    // Collation (NB: locale sensitive)
    int compare(const string& str) const noexcept;
    bool operator<(const string& rhs) const;

    // Reading characters and substrings
    char32_t peekChar(uint32_t offset) const;
    char32_t readChar(uint32_t& offset) const;
    void rewind(uint32_t& offset) const; // moves backwards one char
    char32_t operator[](int32_t charIndex) const;
    char32_t charAt(int32_t charIndex) const;
    uint32_t charIndexToOffset(int32_t charIndex) const;
    // string substr(ptr from) const { return substr(from._offset); }
    // string substr(ptr from, ptr to) const { return substr(from._offset, to._offset); }
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
    //void insert(ptr p,  const string& str) { assert(p._buf==_buf); insert(p._offset, str); }
    void insert(uint32_t byteOffset, const string& str) { insert(byteOffset, str.start(), str._cb); }
    void insert(uint32_t byteOffset, const char* str, int32_t cb=-1);
    void insertAt(int32_t charIndex, const string& str) { insert(charIndexToOffset(charIndex), str); }
    //void erase(ptr from) { assert(from._buf==_buf); erase(from._offset); }
    //void erase(ptr from, ptr to) { assert(from._buf==_buf); assert(to._buf==_buf); erase(from._offset, to._offset); }
    void erase(uint32_t fromByteOffset, uint32_t toByteOffset=eof);
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
    mutable char* _buf;
    uint32_t _cb;
    mutable uint32_t _offset;
    inline char* start() const {
        return _buf+_offset;
    }

    friend string base64_encode(const string& str);
    
    /*string(ptr& p, uint32_t offsetFrom, uint32_t offsetTo) : _buf(p._buf) {
        _offset = p._offsetBase + offsetFrom;
        _cb = offsetTo-offsetFrom;
        buf_retain(_buf);
    }*/
};

