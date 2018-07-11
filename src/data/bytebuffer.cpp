//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


ByteBuffer::ByteBuffer() {
    data = nullptr;
    cb = 0;
    _owns = true;
}
ByteBuffer::ByteBuffer(size_t cb) {
    this->cb = cb;
    data = (uint8_t*)malloc(cb);
    _owns = true;
}
ByteBuffer::ByteBuffer(uint8_t* data, size_t cb, bool copy, bool owns) {
    this->cb = cb;
    if (copy) {
        this->data = (uint8_t*)malloc(cb);
        memcpy(this->data, data, cb);
    } else {
        this->data =  data;
    }
    _owns = owns;
}
ByteBuffer::ByteBuffer(const ByteBuffer& other) {
    cb = other.cb;
    data = (uint8_t*)malloc(cb);
    memcpy(data, other.data, cb);
    _owns = true;
}
ByteBuffer::ByteBuffer(const string& str) {
    cb = str.length();
    data = (uint8_t*)malloc(cb);
    memcpy(data, str.data(), cb);
    _owns = true;
}

ByteBuffer::~ByteBuffer() {
    if (_owns && data) {
        free(data);
    }
}

// ISerializable
bool ByteBuffer::readSelfFromStream(Stream* stream) {
    if (!stream->readBytes(sizeof(cb), &cb)) {
        return false;
    }
    data = (uint8_t*)malloc(cb);
    return stream->readBytes(cb, data);
}
bool ByteBuffer::writeSelfToStream(Stream* stream) const {
    if (!stream->writeBytes(sizeof(cb), &cb)) {
        return false;
    }
    return stream->writeBytes(cb, data);
}


ByteBuffer* ByteBuffer::createFromFile(const string &path) {
    FILE *file = fopen(path.data(), "rb");
    if (!file) {
        app.log("Failed to open file: %s", path.data());
        return NULL;
    }
    ByteBuffer* data = new ByteBuffer();
    fseek(file, 0, SEEK_END);
    data->cb = ftell(file);
    rewind (file);
    data->data = (uint8_t*) malloc(data->cb);
    size_t read = fread(data->data, 1, data->cb, file);
    assert(read == data->cb);
    fclose(file);
    return data;
}

void ByteBuffer::saveToFile(const string& path) {
    FILE *file = fopen(path.data(), "w+b");
    if (!file) {
        app.log("Failed to open %s", path.data());
        return;
    }
    fwrite(data, cb, 1, file);
    fclose(file);
}
