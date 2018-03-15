//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"


Data::Data() {
    data = nullptr;
    cb = 0;
}
Data::Data(size_t cb) {
    this->cb = cb;
    data = (uint8_t*)malloc(cb);
}
Data::Data(const Data& other) {
    cb = other.cb;
    data = (uint8_t*)malloc(cb);
    memcpy(data, other.data, cb);
}
Data::Data(const string& str) {
    cb = str.length();
    data = (uint8_t*)malloc(cb);
    memcpy(data, str.data(), cb);
}

Data::~Data() {
    if (data) {
        free(data);
    }
}

// ISerializable
bool Data::readSelfFromStream(Stream* stream) {
    if (!stream->readBytes(sizeof(cb), &cb)) {
        return false;
    }
    data = (uint8_t*)malloc(cb);
    return stream->readBytes(cb, data);
}
bool Data::writeSelfToStream(Stream* stream) {
    if (!stream->writeBytes(sizeof(cb), &cb)) {
        return false;
    }
    return stream->writeBytes(cb, data);
}

Data* Data::createFromFile(const string &path) {
    FILE *file = fopen(path.data(), "rb");
    if (!file) {
        app.log("Failed to open file: %s", path.data());
        return NULL;
    }
    Data* data = new Data();
    fseek(file, 0, SEEK_END);
    data->cb = ftell(file);
    rewind (file);
    data->data = (uint8_t*) malloc(data->cb);
    size_t read = fread(data->data, 1, data->cb, file);
    assert(read == data->cb);
    fclose(file);
    return data;
}

void Data::saveToFile(const string& path) {
    FILE *file = fopen(path.data(), "w+b");
    if (!file) {
        app.log("Failed to open %s", path.data());
        return;
    }
    fwrite(data, cb, 1, file);
    fclose(file);
}

bool KeyValueMap::readSelfFromStream(Stream* stream) {
    uint32_t num = 0;
    if (!stream->readUint32(&num)) {
        return false;
    }
    for (int i=0 ; i<num ; i++) {
        string key;
        Variant value;
        if (!stream->readString(&key)) {
            return false;
        }
        if (!value.readSelfFromStream(stream)) {
            return false;
        }
        _map[key] = value;
    }
    return true;
}
bool KeyValueMap::writeSelfToStream(Stream *stream) {
    uint32_t num = (uint32_t)_map.size();
    if (!stream->writeUint32(num)) {
        return false;
    }
    for (auto it : _map) {
        if (!stream->writeString(it.first)) {
            return false;
        }
        Variant& val = it.second;
        if (!val.writeSelfToStream(stream)) {
            return false;
        }
    }
    return true;
}

KeyValueMap::Variant::Variant() : type(EMPTY) {
	//new(&str) string();
}
void KeyValueMap::Variant::assign(const Variant& src) {
    type = src.type;
    switch (src.type) {
        case EMPTY: break;
        case INT8: i8 = src.i8; break;
        case INT16: i16 = src.i16; break;
        case INT32: i32 = src.i32; break;
        case INT64: i64 = src.i64; break;
        case UINT8: u8 = src.u8; break;
        case UINT16: u16 = src.u16; break;
        case UINT32: u32 = src.u32; break;
        case UINT64: u64 = src.u64; break;
        case FLOAT: f = src.f; break;
        case DOUBLE: d = src.d; break;
        case DATA: data = src.data; break;
        case MAP: map = src.map; break;
    }
}
KeyValueMap::Variant::Variant(const Variant& var) {
    assign(var);
}
KeyValueMap::Variant& KeyValueMap::Variant::operator=(const Variant& var) {
    assign(var);
	return *this;
}
KeyValueMap::Variant::~Variant() {
    if (type == DATA) {
        data.~ObjPtr();
    }
    if (type == MAP) {
        map.~ObjPtr();
    }
}

bool KeyValueMap::Variant::readSelfFromStream(Stream* stream) {
    if (!stream->readBytes(sizeof(type), &type)) {
        return false;
    }
    switch (type) {
        case EMPTY: return true;
        case INT8: return stream->readBytes(1, &i8);
        case UINT8: return stream->readBytes(1, &u8);
        case INT16: return stream->readBytes(2, &i16);
        case UINT16: return stream->readBytes(2, &u16);
        case INT32: return stream->readBytes(4, &i32);
        case UINT32: return stream->readBytes(4, &u32);
        case INT64:return stream->readBytes(8, &i64);
        case UINT64:return stream->readBytes(8, &u64);
        case FLOAT:return stream->readBytes(sizeof(float), &f);
        case DOUBLE:return stream->readBytes(sizeof(double), &d);
        case DATA: {
            data = new Data();
            return data->readSelfFromStream(stream);
        }
        case MAP: {
            map = new KeyValueMap();
            return map->readSelfFromStream(stream);
        }
    }
    return false;
}
bool KeyValueMap::Variant::writeSelfToStream(Stream* stream) {
    if (!stream->writeBytes(sizeof(type), &type)) {
        return false;
    }
    switch (type) {
        case EMPTY: return true;
        case INT8: return stream->writeBytes(1, &i8);
        case UINT8: return stream->writeBytes(1, &u8);
        case INT16: return stream->writeBytes(2, &i16);
        case UINT16: return stream->writeBytes(2, &u16);
        case INT32: return stream->writeBytes(4, &i32);
        case UINT32: return stream->writeBytes(4, &u32);
        case INT64:return stream->writeBytes(8, &i64);
        case UINT64:return stream->writeBytes(8, &u64);
        case FLOAT:return stream->writeBytes(sizeof(float), &f);
        case DOUBLE:return stream->writeBytes(sizeof(double), &d);
        case DATA: return data->writeSelfToStream(stream);
        case MAP: return map->writeSelfToStream(stream);
    }
    return false;
}

bool KeyValueMap::hasValue(const string& key) const {
    return _map.find(key)!=_map.end();
}
int KeyValueMap::getInt(const string& key) const {
    auto val = _map.find(key);
    if (val != _map.end()) {
        switch (val->second.type) {
            case INT8: return val->second.i8;
            case INT16: return val->second.i16;
            case INT32: return val->second.i32;
            case INT64: app.log("Warning: possible data loss truncating int64 to int");
                return (int)val->second.i64;
            case UINT8: return val->second.u8;
            case UINT16: return val->second.u16;
            case UINT32: return val->second.u32;
            case UINT64: app.log("Warning: possible data loss truncating uint64 to int");
                return (int)val->second.u64;
            default: break;
        }
    }
    return 0;
}
void KeyValueMap::setInt(const string& key, int val) {
    Variant v;
    if (sizeof(int)==8) {
        v.type = INT64;
        v.i64 = val;
    } else {
        v.type = INT32;
        v.i32 = val;
    }
    _map[key] = v;
}

KeyValueMap* KeyValueMap::getMap(const string &key) const {
    auto val = _map.find(key);
    if (val == _map.end()) {
        return NULL;
    }
    if (val->second.type != MAP) {
        return NULL;
    }
    return val->second.map._obj;
}
uint64_t KeyValueMap::getUint64(const string& key) const {
    auto val = _map.find(key);
    if (val == _map.end()) {
        return 0;
    }
    if (val->second.type != UINT64) {
        return 0;
    }
    return val->second.u64;
}
float KeyValueMap::getFloat(const string& key) const {
    auto val = _map.find(key);
    if (val == _map.end()) {
        return 0;
    }
    if (val->second.type == FLOAT) {
        return val->second.f;
    }
    if (val->second.type == DOUBLE) {
        return val->second.d;
    }
    return 0;
}
void KeyValueMap::setFloat(const string& key, float val) {
    Variant var;
    var.type = FLOAT;
    var.f = val;
    _map[key] = var;
}

void KeyValueMap::setUint64(const string& key, uint64_t val) {
    Variant v;
    v.type = UINT64;
    v.u64 = val;
    _map[key] = v;
}
void KeyValueMap::setMap(const string& key, KeyValueMap* val) {
    Variant v;
    v.type = MAP;
    v.map = val;
    _map[key] = v;
}
string KeyValueMap::getString(const string& key) const {
    auto val = _map.find(key);
    if (val == _map.end()) {
        return NULL;
    }
    if (val->second.type != DATA) {
        return NULL;
    }
    const Variant& var = val->second;
    Data* data = (Data*)var.data._obj;
    return string((char*)data->data, data->cb);
}
void KeyValueMap::setString(const string& key, const string& val) {
    Variant v;
    v.type = DATA;
    v.data = new Data(val);
    _map[key] = v;
}
Data* KeyValueMap::getData(const string& key) const {
    auto val = _map.find(key);
    if (val == _map.end() || val->second.type != DATA) {
        return NULL;
    }
    const Variant& var = val->second;
    Data* data = (Data*)var.data._obj;
    return data;
}
void KeyValueMap::setData(const string& key, const Data* val) {
    Variant v;
    v.type = DATA;
    v.data = new Data(*val);
    _map[key] = v;
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




bool DataStream::hasMoreToRead() {
	return offsetRead < _data.cb;
}

bool DataStream::writeBytes(size_t cb, const void* bytes) {
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

bool DataStream::readBytes(size_t cb, void* bytes) {
	size_t cbRead = min(cb, _data.cb - offsetRead);
	memcpy(bytes, _data.data+offsetRead, cbRead);
	offsetRead += cbRead;
	return cbRead==cb;
}

FileStream::FileStream(string path) {
    _file = NULL;
    _path = path;
}
FileStream::~FileStream() {
    close();
}

bool FileStream::openForRead() {
    assert(!_file);
    _file = fopen(_path.data(), "rb");
    if (!_file) {
        app.log("Failed to open file: %s", _path.data());
        return false;
    }
    return true;
}

bool FileStream::openForWrite() {
    assert(!_file);
    _file = fopen(_path.data(), "wb");
    if (!_file) {
        app.log("Failed to open file: %s", _path.data());
        return false;
    }
    return true;
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



