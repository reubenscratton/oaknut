//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup base_group
 * @class string
 * @brief An alternative to std::string
 *
 * This is not quite a drop-in replacement. It is the same old byte buffer under the hood
 * but this one treats the bytes as a UTF-8 encoded string.
 *
 * Why? Because UTF-8 is by far the most popular way to encode text and I don't like
 * the memory wasted by wide characters. In my experience the vast majority of strings
 * are short and the runtime cost of finding character boundaries is acceptably small.
 * The code is optimised for when byte count and char count are the same, i.e. the string
 * contains no multibyte characters.
 *
 * NB: The allocated buffer is always one char larger than _cb and _cc indicate, to contain
 * a terminating null character.
 *
 * Future: Support UTF-32 as well via pointer-tagging. Let the encoding be a settable
 * property of the string which just defaults to UTF-8.
 */
class string {
public:
    string() : _p(NULL), _cb(0), _cc(0) {
    }
    string(const char* p) : string() {
        assign(p, p ? (int32_t)strlen(p) : 0);
    }
    string(const char* p, int32_t cb) : string() {
        assign(p, cb);
    }
    string(const string& str) : _p(NULL), _cb(0), _cc(0)  {
        assign(str._p, str._cb);
    }
    ~string() {
        if (_p) {
            free(_p);
        }
    }
    
    const char* data() const noexcept {
        return _p;
    }
    int32_t length() const noexcept {
        return _cc;
    }
    int32_t lengthInBytes() const noexcept {
        return _cb;
    }
    
    // Comparison
    int compare(const string& str) const noexcept;
    int compare(const char* s) const;
    bool operator==(const string& rhs) const;
    bool operator==(const char*   rhs) const;
    bool operator!=(const string& rhs) const;
    bool operator!=(const char*   rhs) const;
    bool operator<(const string& rhs) const;
    bool operator<(const char*   rhs) const;
    
    // Character access
    char32_t operator[](const int32_t charIndex)  const;
    char32_t charAt(const int32_t charIndex)  const;
    string substr(int32_t charIndexStart, int32_t charIndexEnd) const;
    
    // Search
    int32_t find(const string& str) const;
    int32_t find(const char* s) const;
    int32_t find(char32_t ch) const;
    bool contains(char32_t ch) const;
    
    static string format(const char* fmt, ...);
    
    // Modification
    string& operator=(const string& str);
    string& operator=(const char* s);
    void append(const string& str);
    void append(const char* s);
    void append(const char* s, int32_t cb);
    void append(char32_t ch);
    string& operator+=(const string& str) { append(str); return *this; }
    string& operator+=(const char* s) { append(s); return *this; }
    friend string operator+(const string& lhs, const string& rhs);
    friend string operator+(const string& lhs, const char* s);
    void insert(int32_t charIndex, const string& str);
    void insert(int32_t charIndex, const char* str);
    void insert(int32_t charIndex, const char* str, int32_t cb);
    void erase(int32_t charIndex);
    void erase(int32_t charIndexStart, int32_t charIndexEnd);
    void trim();
    bool hasPrefix(const string& prefix) const;
    bool hadPrefix(const string& prefix);
    bool hasSuffix(const string& suffix) const;
    bool hadSuffix(const string& suffix);

    // Character iteration
    class iterator: public std::iterator<
                                    std::input_iterator_tag,   // iterator_category
                                    char32_t,                      // value_type
                                    char32_t,                      // difference_type
                                    const char32_t*,               // pointer
                                    char32_t                       // reference
                                    >{
        const string& _str;
        int32_t _byteOffset;
    public:
        explicit iterator(const string& str, int32_t byteOffset) : _str(str), _byteOffset(byteOffset) {}
        iterator& operator++() {_str.readUtf8(_byteOffset); return *this;}
        iterator operator++(int) {iterator retval = *this; ++(*this); return retval;}
        bool operator==(iterator other) const {return &_str == &other._str && _byteOffset==other._byteOffset;}
        bool operator!=(iterator other) const {return !(*this == other);}
        reference operator*() const {
            auto byteOffset = _byteOffset;
            return _str.readUtf8(byteOffset);
        }
    };
    iterator begin() const {return iterator(*this, 0);}
    iterator end() const {return iterator(*this, -1);}

    size_t hash() const;
    
    friend class Stream;
    
protected:
    char* _p;
    int32_t _cb; // byte count (not including terminating null char)
    int32_t _cc; // character count (not including terminating null char)
    
    void assign(const char* p, int32_t cb);
    int32_t charIndexToByteIndex(int32_t charIndex) const;
    char32_t readUtf8(int32_t& byteOffset) const;
    void normaliseCharRange(int32_t& charIndexStart, int32_t& charIndexEnd) const;
    void countChars();
};

namespace std {
    template <>
    struct hash<::string> {
        size_t operator()( const ::string& k ) const {
            return k.hash();
        }
    };
}
