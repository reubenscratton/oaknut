//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 A key-value store that lives in memory. It is expected that most app types
 will elect to serialize themselves via ISerializable.
 */
class VariantMap : public Object, ISerializableBase {
public:
    
    VariantMap();
    VariantMap(vector<pair<const string&,const Variant&>> vals);
    
    bool hasValue(const string& key) const;
    Variant get(const string& key) const;
    int getInt(const string& key) const;
    uint64_t getUint64(const string& key) const;
    template <typename T>
    T* getSerializable(const string& key) const {
        auto val = _map.find(key);
        if (val == _map.end() || val->second.type != MAP) {
            return NULL;
        }
        T* obj = new T(val->second.map); // T must be constructable from a VariantMap
        return obj;
    }
    void setSerializable(const string& key, class ISerializable* val);
    float getFloat(const string& key) const;
    void setFloat(const string& key, float val);
    void setInt(const string& key, int val);
    void setUint64(const string& key, uint64_t val);
    string getString(const string& key) const;
    void setString(const string& key, const string& val);
    ByteBuffer* getByteBuffer(const string& key) const;
    void setByteBuffer(const string& key, const ByteBuffer* val);

    map<string, Variant> _map;
    
    
    // ISerializableBase
    virtual bool readSelfFromStream(Stream* stream);
    virtual bool writeSelfToStream(Stream* stream) const;
};

