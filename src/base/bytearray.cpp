//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


void bytearray::assign(const uint8_t* p, int32_t cb) {
    if (_p) {
        free(_p);
    }
    if (p) {
        _cb = cb;
        _p = (uint8_t*)malloc(_cb);
        memcpy(_p, p, _cb);
    } else {
        _p = NULL;
        _cb = 0;
    }
}
void bytearray::assignNoCopy(uint8_t* p, int32_t cb) {
    if (_p) {
        free(_p);
    }
    _p = p;
    _cb = cb;
}

bytearray& bytearray::operator=(const bytearray& ba) {
    assign(ba._p, ba._cb);
    return *this;
}
void bytearray::append(const bytearray& ba) {
    append(ba._p, ba._cb);
}
void bytearray::append(const uint8_t* p, int32_t cb) {
    if (!cb) return;
    int32_t newCb = _cb+cb;
    _p = (uint8_t*)realloc(_p, newCb+1);
    memcpy(_p+_cb, p, cb);
    _cb = newCb;
}
void bytearray::append(uint8_t byte) {
    append(&byte, 1);
}
bytearray operator+(const bytearray& lhs, const bytearray& rhs) {
    bytearray r(lhs);
    r.append(rhs);
    return r;
}
void bytearray::insert(int32_t offset, const bytearray& ba) {
    insert(offset, ba._p, ba._cb);
}
void bytearray::insert(int32_t offset, const uint8_t* p, int32_t cb) {
    if (!cb) return;
    int newCb = _cb+cb;
    _p = (uint8_t*)realloc(_p, newCb);
    auto insertionPoint = _p + offset;
    auto cbToMove = _cb - offset;
    memmove(insertionPoint+cb, insertionPoint, cbToMove); // move tail forwards in memory
    memcpy(insertionPoint, p, cb);
    _cb = newCb;
}

void bytearray::erase(int32_t offset) {
    erase(offset, 1);
}
void bytearray::erase(int32_t offset, int32_t cb) {
    memmove(_p+offset, _p, _cb - cb);
    _cb -= cb;
    _p = (uint8_t*)realloc(_p, _cb);
    
}
void bytearray::resize(int32_t newSize) {
    _cb = newSize;
    _p = (uint8_t*)realloc(_p, _cb);
}

string bytearray::toString(bool copy) {
    if (copy) {
        return string((char*)_p, _cb);
    }
    // Move the data to ownership of the string without making a copy
    string str;
    str._p = (char*)_p;
    str._cb = _cb;
    _p = NULL;
    _cb = 0;
    return str;
    
}

