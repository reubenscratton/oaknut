//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


bytestream::bytestream() {
    offsetRead = 0;
    offsetWrite = 0;
}
bytestream::bytestream(int cb) : bytearray(cb) {
}
bytestream::bytestream(bytearray& src) : bytearray(src) {
}

void bytestream::setWriteOffset(size_t offset) {
    offsetWrite = offset;
}

bytearray bytestream::getWrittenBytes() {
    return bytearray(_p, (int)offsetWrite);
}


bool bytestream::eof() {
    return offsetRead >= _cb;
}
bool bytestream::hasMoreToRead() {
    return offsetRead < _cb;
}

bool bytestream::writeBytes(size_t cb, const void* bytes) {
    size_t cbFree = _cb - offsetWrite;
    if (cb > cbFree) {
        size_t new_cb = _cb + (cb-cbFree);
        new_cb = (new_cb+255) & 0xffffff00;
        uint8_t* new_data = (uint8_t*)malloc(new_cb);
        memcpy(new_data, _p, _cb);
        free(_p);
        _p = new_data;
        _cb = new_cb;
    }
    memcpy(_p+offsetWrite, bytes, cb);
    offsetWrite += cb;
    return true;
}

bool bytestream::readBytes(size_t cb, void* bytes) {
    size_t cbRead = MIN(cb, _cb - offsetRead);
    memcpy(bytes, _p+offsetRead, cbRead);
    offsetRead += cbRead;
    return cbRead==cb;
}


template <>
bool bytestream::read(bytearray& val) {
    uint32_t cb;
    if (!read(cb)) {
        return false;
    }
    val.resize(cb);
    return readBytes(cb, val.data());
}
template <>
bool bytestream::write(const bytearray& val) {
    uint32_t cb = val.size();
    if (!write(cb)) {
        return false;
    }
    return writeBytes(cb, val.data());
}
template <>
bool bytestream::read(string& val) {
    uint32_t cb;
    if (!readBytes(sizeof(cb), (uint8_t*)&cb)) {
        val._cb = 0;
        return false;
    }
    val.alloc(cb);
    return readBytes(cb, val._buf);
}
template <>
bool bytestream::write(const string& val) {
    uint32_t cb = val._cb;
    if (!writeBytes(sizeof(cb), (uint8_t*)&cb)) {
        return false;
    }
    return writeBytes(cb, (uint8_t*)val.start());
}
template <>
bool bytestream::read(variant& val) {
    enum variant::type type;
    if (!readBytes(sizeof(type), &type)) {
        return false;
    }
    val.setType(type);
    switch (type) {
        case variant::EMPTY: return true;
        case variant::INT32: return readBytes(4, &val._i32);
        case variant::INT64: return readBytes(8, &val._i64);
        case variant::UINT32: return readBytes(4, &val._u32);
        case variant::UINT64: return readBytes(8, &val._u64);
        case variant::FLOAT32: return readBytes(sizeof(float), &val._f32);
        case variant::FLOAT64: return readBytes(sizeof(double), &val._f64);
        case variant::STRING: return read(val._str);
        case variant::MEASUREMENT: {
            float f;
            measurement::unit type;
            if (readBytes(4, &f) && readBytes(2, &type)) {
                val._measurement = measurement(f, type);
                return true;
            }
            return false;
        }
        case variant::BYTEARRAY: return read(val._bytearray);
        case variant::ARRAY: {
            uint32_t num = 0;
            if (!read(num)) {
                return false;
            }
            for (int i=0 ; i<num ; i++) {
                variant elem;
                if (!read(elem)) {
                    return false;
                }
                val._vec->push_back(elem);
            }
            return true;
        }
        case variant::MAP: {
            uint32_t num = 0;
            if (!read(num)) {
                return false;
            }
            for (int i=0 ; i<num ; i++) {
                string key;
                if (!read(key)) {
                    return false;
                }
                variant value;
                if (!read(value)) {
                    return false;
                }
                val._map->insert(make_pair(key,value));
            }
            return true;
        }
        case variant::OBJPTR:
            assert(0); // unpossible!
            return false;
        case variant::ERROR: {
            read(val._err->_code);
            read(val._err->_msg);
            return true;
        }
    }
    return false;
}
template <>
bool bytestream::write(const variant& val) {
    if (!writeBytes(sizeof(val.type), &val.type)) {
        return false;
    }
    switch (val.type) {
    case variant::EMPTY: return true;
    case variant::INT32: return writeBytes(4, &val._i32);
    case variant::INT64: return writeBytes(8, &val._i64);
    case variant::UINT32: return writeBytes(4, &val._u32);
    case variant::UINT64:return writeBytes(8, &val._u64);
    case variant::FLOAT32:return writeBytes(sizeof(float), &val._f32);
    case variant::FLOAT64:return writeBytes(sizeof(double), &val._f64);
    case variant::STRING:return write(*val._str);
    case variant::MEASUREMENT: {
        float f = val._measurement._unitVal;
        short type = val._measurement._unit;
        if (writeBytes(4, &f) && writeBytes(2, &type)) {
            return true;
        }
        return false;
    }
    case variant::BYTEARRAY:return write(*val._bytearray);
    case variant::ARRAY: {
        uint32_t num = (uint32_t)val._vec->size();
        if (!write(num)) {
            return false;
        }
        for (auto& it : *val._vec) {
            if (!write(it)) {
                return false;
            }
        }
        return true;
    }
    case variant::MAP: {
        uint32_t num = (uint32_t)val._map->size();
        if (!write(num)) {
            return false;
        }
        for (auto& it : *val._map) {
            if (!write(it.first)) {
                return false;
            }
            const variant& val = it.second;
            if (!write(val)) {
                return false;
            }
        }
        return true;
    }
            
    case variant::OBJPTR:
        assert(0);
        return false;

    case variant::ERROR:
        write(val._err->_code);
        write(val._err->_msg);
        return true;

    }
    return false;
}


