//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

Stream::Stream() {
    offsetRead = 0;
    offsetWrite = 0;
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


