//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 Interface for objects that serialize to and from VariantMap. Necessary for LocalStorage
 */
class ISerializeToVariantMap {
public:
    virtual void writeSelfToVariantMap(class VariantMap& map) =0;
};


/**
 A collection of named values. Or, more accurately, a serializable std::map<string,Variant>.
 */
class VariantMap : public Object, ISerializable {
public:
    
    VariantMap();
    VariantMap(vector<pair<const string&,const Variant&>> vals);
    
    bool hasValue(const string& key) const;
    Variant get(const string& key) const;
    void set(const string& key, const Variant& val);
    void set(const string& key, ISerializeToVariantMap* object);

    template <typename T>
    T* get(const string& key) const {
        auto val = _map.find(key);
        if (val == _map.end() || val->second.type != Variant::MAP) {
            return NULL;
        }
        T* obj = new T(*(val->second.map)); // T must be constructable from a VariantMap
        return obj;
    }
    
    const Variant operator[](const string& key) const {
        return get(key);
    }
    Variant& operator[](const string& key) {
        return (*((_map.insert(make_pair(key,Variant()))).first)).second;
    }


    map<string, Variant> _map;
    
    
    // ISerializable
    virtual bool readSelfFromStream(Stream* stream);
    virtual bool writeSelfToStream(Stream* stream) const;
};

