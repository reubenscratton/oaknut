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
    size_t cb = 0;
    if (!readBytes(sizeof(cb), (uint8_t*)&cb)) {
        return false;
    }
    str->resize(cb);
    return readBytes(cb, (uint8_t*)(str->data()));
}

bool Stream::writeString(const string& str) {
    size_t cb = str.length();
    if (!writeBytes(sizeof(cb), (uint8_t*)&cb)) {
        return false;
    }
    return writeBytes(str.length(), (uint8_t*)str.data());
}


