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
}
ByteBuffer::ByteBuffer(size_t cb) {
    this->cb = cb;
    data = (uint8_t*)malloc(cb);
}
ByteBuffer::ByteBuffer(uint8_t* data, size_t cb, bool copy) {
    this->cb = cb;
    if (copy) {
        this->data = (uint8_t*)malloc(cb);
        memcpy(this->data, data, cb);
    } else {
        this->data =  data;
    }
}
ByteBuffer::ByteBuffer(const ByteBuffer& other) {
    cb = other.cb;
    data = (uint8_t*)malloc(cb);
    memcpy(data, other.data, cb);
}
ByteBuffer::ByteBuffer(const string& str) {
    cb = str.length();
    data = (uint8_t*)malloc(cb);
    memcpy(data, str.data(), cb);
}

ByteBuffer::~ByteBuffer() {
    if (data) {
        free(data);
    }
}

void ByteBuffer::set(const void* rawData, size_t len) {
    if (data) {
        free(data);
    }
    cb = len;
    data = (uint8_t*)malloc(cb);
    memcpy(data, rawData, cb);
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

string ByteBuffer::toString(bool copy) {
    int32_t cb = (int32_t)this->cb;
    if (copy) {
        return string((char*)data, cb);
    }
    // Move the data to ownership of the string without making a copy
    string str;
    str._p = (char*)data;
    str._cb = cb;
    data = NULL;
    this->cb = 0;
    return str;

}

void ByteBuffer::attach(uint8_t* data, size_t cb) {
    this->data = data;
    this->cb = cb;
}
void ByteBuffer::detach() {
    this->data = NULL;
    this->cb = 0;
}
