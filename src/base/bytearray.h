//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup base_group
 * @class bytearray
 * @brief An alternative to std::vector<uint8_t>
 *
 * A simple array of bytes as distinct from a string. Exists because I felt it ought to.
 */

class bytearray {
public:
    bytearray() : _p(NULL), _cb(0) {
    }
    bytearray(int32_t cb) : _cb(cb) {
        _p = ((uint8_t*)malloc(cb));
    }
    bytearray(const uint8_t* p, int32_t cb) : bytearray() {
        assign(p, cb);
    }
    bytearray(const bytearray& src) : _p(NULL), _cb(0) {
        assign(src._p, src._cb);
    }
    bytearray(bytearray&& other) noexcept : _p(other._p), _cb(other._cb) {
        other._p = NULL;
        other._cb = 0;
    }
    ~bytearray() {
        if (_p) {
            free(_p);
        }
    }
    
    uint8_t* data() const noexcept {
        return _p;
    }
    int32_t size() const noexcept {
        return _cb;
    }
    int32_t length() const noexcept {
        return _cb;
    }
    
    // Modification
    void assign(const uint8_t* p, int32_t cb);
    void assignNoCopy(uint8_t* p, int32_t cb);
    bytearray& operator=(const bytearray& str);
    void append(const bytearray& str);
    void append(const uint8_t* p, int32_t cb);
    void append(uint8_t byte);
    bytearray& operator+=(const bytearray& str) { append(str); return *this; }
    friend bytearray operator+(const bytearray& lhs, const bytearray& rhs);
    void insert(int32_t offset, const bytearray& str);
    void insert(int32_t offset, const uint8_t* p, int32_t cb);
    void erase(int32_t offset);
    void erase(int32_t offsetStart, int32_t cb);
    void resize(int32_t newSize);
    void detach();
    void clear();
    
    class string toString();
    
protected:
    uint8_t* _p;
    int32_t _cb;
};

