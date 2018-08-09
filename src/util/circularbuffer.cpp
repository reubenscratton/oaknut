//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


CircularBuffer::CircularBuffer(uint32_t length) {
    _bytes = (uint8_t*)malloc(length);
    _length = length;
    _read = _write = _fill = 0;
}

void CircularBuffer::write(uint8_t* bytes, uint32_t cb) {
    uint32_t freeSpace = _length-_fill;
    if (cb > freeSpace) {
        uint32_t newLength = (_length + cb);
        newLength += 4096-(newLength%4096);
        uint8_t* newBytes = (uint8_t*)malloc(newLength);
        if (_write >= _read) {
            memcpy(newBytes+_read, _bytes+_read, _fill);
        } else {
            uint32_t lengthOfSecondRegion = _fill - _write;
            memcpy(newBytes, _bytes, _write);
            memcpy(newBytes + newLength - lengthOfSecondRegion, _bytes + _length - lengthOfSecondRegion, lengthOfSecondRegion);
            _read += newLength-_length;
        }
        free(_bytes);
        _bytes = newBytes;
        _length = newLength;
        freeSpace = _length-_fill;
    }
    
    _fill += cb;
    uint32_t cbSection = std::min(cb, _length - _write);
    memcpy(_bytes+_write, bytes, cbSection);
    cb -= cbSection;
    if (cb == 0) {
        _write += cbSection;
    } else {
        memcpy(_bytes, bytes+cbSection, cb);
        _write = cb;
    }
    if (_write >= _length) {
        _write -= _length;
    }
}
uint32_t CircularBuffer::read(uint8_t* bytes, uint32_t cb) {
    cb = std::min(cb, _fill);
    //[self dump:@">Read" cb:cb];
    if (_write >= _read) {
        memcpy(bytes, _bytes+_read, cb);
    } else {
        uint32_t cbFirst = std::min(cb, _length-_read);
        memcpy(bytes, _bytes+_read, cbFirst);
        if (cb > cbFirst) {
            memcpy(bytes+cbFirst, _bytes, cb-cbFirst);
        }
    }
    _fill -= cb;
    _read += cb;
    if (_read >= _length) {
        _read -= _length;
    }
    //[self dump:@"<Read" cb:cb];
    return cb;
}
