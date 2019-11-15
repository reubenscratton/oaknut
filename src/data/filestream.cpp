//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



FileStream::FileStream(const string& path) : _path(path) {
    _file = NULL;
}
FileStream::~FileStream() {
    close();
}

bool FileStream::openForRead() {
    assert(!_file);
    if (!app->fileResolve(_path)) {
        return false;
    }
    _file = fopen(_path.c_str(), "rb");
    if (!_file) {
        app->log("Failed to open file: %s", _path.c_str());
        return false;
    }
    return true;
}

bool FileStream::openForWrite() {
    assert(!_file);
    if (!app->fileResolve(_path)) {
        return false;
    }
    _file = fopen(_path.c_str(), "wb");
    if (!_file) {
        app->log("Failed to open file: %s", _path.c_str());
        return false;
    }
    return true;
}

void FileStream::setWriteOffset(size_t offset) {
    fseek(_file, offset, SEEK_SET);
}


void FileStream::close() {
    if (_file) {
        fclose(_file);
        _file = nullptr;
    }
}

bool FileStream::hasMoreToRead() {
    assert(_file);
    return !feof(_file);
}
bool FileStream::writeBytes(size_t cb, const void* bytes) {
    assert(_file);
    size_t written = fwrite(bytes, 1, cb, _file);
    return written == cb;
}
bool FileStream::readBytes(size_t cb, void* bytes) {
    assert(_file);
    size_t read = fread(bytes, 1, cb, _file);
    return read == cb;
}

