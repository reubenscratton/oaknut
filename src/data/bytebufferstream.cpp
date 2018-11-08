//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


ByteBufferStream::ByteBufferStream() {
}
ByteBufferStream::ByteBufferStream(int cb) : _data(cb) {
}
ByteBufferStream::ByteBufferStream(ByteBuffer* data) : _data(data->data, data->cb, false) {
}

void ByteBufferStream::setWriteOffset(size_t offset) {
    offsetWrite = offset;
}

bool ByteBufferStream::hasMoreToRead() {
    return offsetRead < _data.cb;
}

bool ByteBufferStream::writeBytes(size_t cb, const void* bytes) {
    size_t cbFree = _data.cb - offsetWrite;
    if (cb > cbFree) {
        size_t new_cb = _data.cb + (cb-cbFree);
        new_cb = (new_cb+255) & 0xffffff00;
        uint8_t* new_data = (uint8_t*)malloc(new_cb);
        memcpy(new_data, _data.data, _data.cb);
        free(_data.data);
        _data.data = new_data;
        _data.cb = new_cb;
    }
    memcpy(_data.data+offsetWrite, bytes, cb);
    offsetWrite += cb;
    return true;
}

bool ByteBufferStream::readBytes(size_t cb, void* bytes) {
    size_t cbRead = MIN(cb, _data.cb - offsetRead);
    memcpy(bytes, _data.data+offsetRead, cbRead);
    offsetRead += cbRead;
    return cbRead==cb;
}

