//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

Stream::Stream() {
    offsetRead = 0;
    offsetWrite = 0;
}

bool Stream::readInt8(int8_t* val) {
    return readBytes(sizeof(*val), val);
}
bool Stream::writeInt8(int8_t val) {
    return writeBytes(sizeof(val), &val);
}
bool Stream::readInt16(int16_t* val) {
    return readBytes(sizeof(*val), val);
}
bool Stream::writeInt16(int16_t val) {
    return writeBytes(sizeof(val), &val);
}
bool Stream::readInt32(int32_t* val) {
    return readBytes(sizeof(*val), val);
}
bool Stream::writeInt32(int32_t val) {
    return writeBytes(sizeof(val), &val);
}
bool Stream::readUint32(uint32_t* val) {
    return readBytes(sizeof(*val), val);
}
bool Stream::writeUint32(uint32_t val) {
    return writeBytes(sizeof(val), &val);
}

bool Stream::readByteArray(bytearray* ba) {
    uint32_t cb;
    if (!readUint32(&cb)) {
        return false;
    }
    ba->resize(cb);
    return readBytes(cb, ba->data());
}
bool Stream::writeByteArray(const bytearray& ba) {
    uint32_t cb = ba.size();
    if (!writeUint32(cb)) {
        return false;
    }
    return writeBytes(cb, ba.data());
}

bool Stream::readString(string* str) {
    str->_cb = 0;
    if (!readBytes(sizeof(str->_cb), (uint8_t*)&str->_cb)) {
        return false;
    }
    str->_p = (char*)malloc(str->_cb + 1);
    str->_p[str->_cb] = '\0';
    bool ok = readBytes(str->_cb, str->_p);
    if (ok) {
        str->countChars();
    }
    return ok;
}

bool Stream::writeString(const string& str) {
    if (!writeBytes(sizeof(str._cb), (uint8_t*)&str._cb)) {
        return false;
    }
    return writeBytes(str._cb, (uint8_t*)str._p);
}

bool Stream::readVariant(variant* val) {
    enum variant::type type;
    if (!readBytes(sizeof(type), &type)) {
        return false;
    }
    val->setType(type);
    switch (type) {
        case variant::EMPTY: return true;
        case variant::INT32: return readBytes(4, &val->_i32);
        case variant::INT64: return readBytes(8, &val->_i64);
        case variant::UINT32: return readBytes(4, &val->_u32);
        case variant::UINT64: return readBytes(8, &val->_u64);
        case variant::FLOAT32: return readBytes(sizeof(float), &val->_f32);
        case variant::FLOAT64: return readBytes(sizeof(double), &val->_f64);
        case variant::STRING: return readString(val->_str);
        case variant::BYTEARRAY: return readByteArray(val->_bytearray);
        case variant::ARRAY: {
            uint32_t num = 0;
            if (!readUint32(&num)) {
                return false;
            }
            for (int i=0 ; i<num ; i++) {
                variant elem;
                if (!readVariant(&elem)) {
                    return false;
                }
                val->_vec->push_back(elem);
            }
            return true;
        }
        case variant::MAP: {
            uint32_t num = 0;
            if (!readUint32(&num)) {
                return false;
            }
            for (int i=0 ; i<num ; i++) {
                string key;
                if (!readString(&key)) {
                    return false;
                }
                variant value;
                if (!readVariant(&value)) {
                    return false;
                }
                val->_map->insert(make_pair(key,value));
            }
            return true;
        }
    }
    return false;
}

bool Stream::writeVariant(const variant& val) {
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
    case variant::STRING:return writeString(*val._str);
    case variant::BYTEARRAY:return writeByteArray(*val._bytearray);
    case variant::ARRAY: {
        uint32_t num = (uint32_t)val._vec->size();
        if (!writeUint32(num)) {
            return false;
        }
        for (auto& it : *val._vec) {
            if (!writeVariant(it)) {
                return false;
            }
        }
        return true;
    }
    case variant::MAP: {
        uint32_t num = (uint32_t)val._map->size();
        if (!writeUint32(num)) {
            return false;
        }
        for (auto& it : *val._map) {
            if (!writeString(it.first)) {
                return false;
            }
            const variant& val = it.second;
            if (!writeVariant(val)) {
                return false;
            }
        }
        return true;
    }
    }
    return false;
}


