//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#define FLAG_ALLOW_JSONLITE 1

/**
 Interface for objects that serialize to and from Variant.
 */
class ISerializeToVariant {
public:
    virtual void fromVariant(const class Variant& v) =0;
    virtual void toVariant(Variant& v) =0; /* ideally this would be const */
};

/**
 * @ingroup data_group
 * @class Variant
 * A discriminated union of all elementary numeric types, a string, a blob,
 * an array of Variants, and a compound map of string-Variant pairs.
 * Mainly used for serializing data and parsing JSON and layout/style files.
 *
 * Variant is intended as a value type, i.e. it does not extend Object and
 * constructors and operator=() all make copies rather than share references.
 * Be mindful of this when dealing with large amounts of data...
 *
 */

class Variant : public ISerializable {
public:
    enum VariantType {
        EMPTY,
        INT32,  INT64,
        UINT32, UINT64,
        FLOAT32, FLOAT64,
        STRING,
        BYTEBUFFER,
        ARRAY,
        MAP
    } type;
    union {
        int32_t i32;
        int64_t i64;
        uint32_t u32;
        uint64_t u64;
        float f;
        double d;
        string* str; // std::string is 24 bytes, hence use pointer
        ObjPtr<ByteBuffer> _data;
        vector<Variant>* _vec;
        map<string, Variant>* _map;
    };
    Variant();
    Variant(int32_t val);
    Variant(int64_t val);
    Variant(uint32_t val);
    Variant(uint64_t val);
    Variant(float val);
    Variant(double val);
    Variant(ByteBuffer* val, bool copy=false);
    Variant(const char* val);
    Variant(const string& val);
    Variant(const Variant& var);
    Variant(Variant&& var) noexcept;
    Variant(class ISerializeToVariant* val);
    Variant(const vector<Variant>& val);
    Variant(vector<pair<const string&,const Variant&>> vals);
    Variant& operator=(const Variant& rhs);
    bool operator<(const Variant& rhs) const;
    ~Variant();
    
    // Comparison
    bool operator ==(float val) {return type==FLOAT32 && f==val; }
    bool operator ==(int val) {return (type==INT32 && i32==val) || (type==UINT32 && u32==val); }
    bool operator ==(const string& val) {return type==STRING && str->compare(val)==0; }
    
    // Accessors
    int intVal() const;
    string stringVal() const;
    float floatVal() const;
    double doubleVal() const;
    const vector<Variant>& arrayVal() const;
    ByteBuffer* byteBufferVal() const;

    // Compound accessors
    int intVal(const char* name) const;
    string stringVal(const char* name) const;
    float floatVal(const char* name) const;
    double doubleVal(const char* name) const;
    ByteBuffer* byteBufferVal(const char* name) const;
    const vector<Variant>& arrayVal(const char* name) const;
    template <class T>
    vector<ObjPtr<T>> arrayVal(const char* name) const {
        static_assert(std::is_base_of<ISerializeToVariant, T>::value, "T must implement ISerializeToVariant");
        vector<ObjPtr<T>> vec;
        auto a = arrayVal(name);
        for (auto& e : a) {
            T* t = new T();
            t->fromVariant(e);
            vec.push_back(t);
        }
        return vec;
    }
    template <typename T>
    T get() const {
        static_assert(std::is_base_of<ISerializeToVariant, T>::value, "T must implement ISerializeToVariant");
        T obj;
        obj.fromVariant(*this);
        return obj;
    }
    template <typename T>
    T get(const char* key) const {
        static_assert(std::is_base_of<ISerializeToVariant, T>::value, "T must implement ISerializeToVariant");
        assert(type==MAP);
        auto val = _map->find(key);
        if (val == _map->end() || val->second.type != Variant::MAP) {
            assert(0);
        }
        T obj;
        obj.fromVariant(val->second);
        return obj;
    }
    template <typename T>
    T* getObject(const char* key) const {
        static_assert(std::is_base_of<ISerializeToVariant, T>::value, "T must implement ISerializeToVariant");
        static_assert(std::is_base_of<Object, T>::value, "T must extend Object");
        assert(type==MAP);
        auto val = _map->find(key);
        if (val == _map->end() || val->second.type != Variant::MAP) {
            assert(0);
            return NULL;
        }
        T* obj = new T();
        obj->fromVariant(val->second);
        return obj;
    }

    // No const operator[] cos it appears impossible to avoid copying
    /*const Variant operator[](const char* key) const {
        assert(type==MAP);
        return *get(string(key));
    }
    const Variant operator[](const string& key) const {
         assert(type==MAP);
        return *get(key);
    }*/
    Variant& operator[](const char* key) {
        if (type==EMPTY) {
            type = MAP;
            _map = new map<string, Variant>();
        }
        assert(type==MAP);
        return (*((_map->insert(std::make_pair(string(key),Variant()))).first)).second;
    }

    const Variant* get(const string& keypath) const;
    void set(const string& key, const Variant& val);
    void set(const string& key, ISerializeToVariant* object);
    void clear();
    void appendVal(const Variant& v);
    

    // Parsing from JSON and config files
    static Variant parse(class StringProcessor& it, int flags);
    string toJson();

    // ISerializable
    virtual bool readSelfFromStream(class Stream* stream);
    virtual bool writeSelfToStream(Stream* stream) const;
    
    // Javascript helpers
#ifdef PLATFORM_WEB
    val toJavascriptVal() const;
#endif
    
private:
    inline void assign(const Variant& src);
    void setType(VariantType newType);
};
