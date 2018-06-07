//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


VariantMap::VariantMap() {
}
VariantMap::VariantMap(vector<pair<const string&, const Variant&>> vals) {
    for (auto it : vals) {
        _map.insert(it);
    }
}

bool VariantMap::readSelfFromStream(Stream* stream) {
    uint32_t num = 0;
    if (!stream->readUint32(&num)) {
        return false;
    }
    for (int i=0 ; i<num ; i++) {
        string key;
        Variant value(Variant::EMPTY);
        if (!stream->readString(&key)) {
            return false;
        }
        if (!value.readSelfFromStream(stream)) {
            return false;
        }
        _map.insert(make_pair(key,value));
    }
    return true;
}

bool VariantMap::writeSelfToStream(Stream *stream) const {
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


bool VariantMap::hasValue(const string& key) const {
    return _map.find(key)!=_map.end();
}

Variant VariantMap::get(const string& key) const {
    auto val = _map.find(key);
    if (val != _map.end()) {
        return val->second;
    }
    return Variant();
}
void VariantMap::set(const string& key, const Variant& val) {
    _map.insert(make_pair(key, val));
}

void VariantMap::set(const string& key, ISerializeToVariantMap* val) {
    if (!val) {
        return;
    }
    Variant v(Variant::MAP);
    v.map = new VariantMap();
    val->writeSelfToVariantMap(*(v.map));
    _map[key] = v;
}

void VariantMap::clear() {
    _map.clear();
}

/*
void VariantMap::setInt(const string& key, int val) {
    if (sizeof(int)==8) {
        Variant v(Variant::INT64);
        v.i64 = val;
        _map[key] = v;
    } else {
        Variant v(Variant::INT32);
        v.i32 = val;
        _map[key] = v;
    }
}


uint64_t VariantMap::getUint64(const string& key) const {
    auto val = _map.find(key);
    if (val == _map.end()) {
        return 0;
    }
    if (val->second.type != Variant::UINT64) {
        return 0;
    }
    return val->second.u64;
}
float VariantMap::getFloat(const string& key) const {
    auto val = _map.find(key);
    if (val == _map.end()) {
        return 0;
    }
    if (val->second.type == Variant::FLOAT32) {
        return val->second.f;
    }
    if (val->second.type == Variant::FLOAT64) {
        return val->second.d;
    }
    return 0;
}
void VariantMap::setFloat(const string& key, float val) {
    Variant var(Variant::FLOAT32);
    var.f = val;
    _map[key] = var;
}

void VariantMap::setUint64(const string& key, uint64_t val) {
    Variant v;
    v.type = Variant::UINT64;
    v.u64 = val;
    _map[key] = v;
}
VariantMap* VariantMap::getMap(const string &key) const {
    auto val = _map.find(key);
    if (val == _map.end()) {
        return NULL;
    }
    if (val->second.type != MAP) {
        return NULL;
    }
    return val->second.map._obj;
}
 
string VariantMap::getString(const string& key) const {
    auto val = _map.find(key);
    if (val == _map.end()) {
        return NULL;
    }
    if (val->second.type != Variant::BYTEBUFFER) {
        return NULL;
    }
    const Variant& var = val->second;
    ByteBuffer* data = (ByteBuffer*)var.data._obj;
    return string((char*)data->data, data->cb);
}
void VariantMap::setString(const string& key, const string& val) {
    Variant v(Variant::BYTEBUFFER);
    v.data = new ByteBuffer(val);
    _map[key] = v;
}
ByteBuffer* VariantMap::getByteBuffer(const string& key) const {
    auto val = _map.find(key);
    if (val == _map.end() || val->second.type != Variant::BYTEBUFFER) {
        return NULL;
    }
    const Variant& var = val->second;
    ByteBuffer* data = (ByteBuffer*)var.data._obj;
    return data;
}
void VariantMap::setByteBuffer(const string& key, const ByteBuffer* val) {
    Variant v(Variant::BYTEBUFFER);
    v.data = new ByteBuffer(*val);
    _map[key] = v;
}*/


