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
    cb = str.lengthInBytes();
    data = (uint8_t*)malloc(str._cb);
    memcpy(data, str._buf+str._offset, cb);
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
    string pathcopy(path);
    FILE *file = fopen(pathcopy.c_str(), "rb");
    if (!file) {
        app->log("Failed to open file: %s", path.c_str());
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
    string pathcopy(path);
    FILE *file = fopen(pathcopy.c_str(), "w+b");
    if (!file) {
        app->log("Failed to open %s", path.c_str());
        return;
    }
    fwrite(data, cb, 1, file);
    fclose(file);
}

string ByteBuffer::toString() {
    return string((char*)data, int32_t(this->cb));
}

void ByteBuffer::attach(uint8_t* data, size_t cb) {
    this->data = data;
    this->cb = cb;
}
void ByteBuffer::detach() {
    this->data = NULL;
    this->cb = 0;
}
