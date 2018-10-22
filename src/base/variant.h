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
    virtual void fromVariant(const class variant& v) =0;
    virtual void toVariant(variant& v) =0; /* ideally this would be const */
};

/**
 * @ingroup base
 * @class variant
 * A discriminated union of all elementary numeric types, a string, a blob,
 * an array of variants, and a compound map of string-variant pairs.
 * Mainly used for serializing data and parsing JSON and layout/style files.
 *
 * variant is a value type, i.e. it does not extend Object and
 * constructors and operator=() all make copies rather than share references.
 * Be mindful of this when dealing with large amounts of data...
 *
 */

class variant {
private:
    union {
        int32_t _i32;
        int64_t _i64;
        uint32_t _u32;
        uint64_t _u64;
        float _f32;
        double _f64;
        string* _str;
        bytearray* _bytearray;
        vector<variant>* _vec;
        map<string, variant>* _map;
    };
public:
    enum type {
        EMPTY,
        INT32,  INT64,
        UINT32, UINT64,
        FLOAT32, FLOAT64,
        STRING,
        BYTEARRAY,
        ARRAY,
        MAP
    } type;
    
    void setType(enum type newType);
    
    variant();
    variant(int32_t val);
    variant(int64_t val);
    variant(uint32_t val);
    variant(uint64_t val);
    variant(float val);
    variant(double val);
    variant(const bytearray& val);
    variant(const char* val);
    variant(const string& val);
    variant(const variant& var);
    variant(variant&& var) noexcept;
    variant(class ISerializeToVariant* val);
    //variant(const vector<variant>& val);
    variant(const vector<pair<string,variant>>& vals);
    variant& operator=(const variant& rhs);
    bool operator<(const variant& rhs) const;
    ~variant();
    
    // Comparison
    bool operator ==(float val) {return type==FLOAT32 && _f32==val; }
    bool operator ==(int val) {return (type==INT32 && _i32==val) || (type==UINT32 && _u32==val); }
    bool operator ==(const string& val) {return type==STRING && _str->compare(val)==0; }
    
    // Accessors
    int intVal() const;
    string stringVal() const;
    float floatVal() const;
    double doubleVal() const;
    const vector<variant>& arrayVal() const;
    const bytearray& bytearrayVal() const;

    // Compound accessors
    int intVal(const char* name) const;
    string stringVal(const char* name) const;
    float floatVal(const char* name) const;
    double doubleVal(const char* name) const;
    const bytearray& bytearrayVal(const char* name) const;
    const vector<variant>& arrayVal(const char* name) const;
    template <class T>
    vector<sp<T>> arrayVal(const char* name) const {
        static_assert(std::is_base_of<ISerializeToVariant, T>::value, "T must implement ISerializeToVariant");
        vector<sp<T>> vec;
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
        if (val == _map->end() || val->second.type != variant::MAP) {
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
        if (val == _map->end() || val->second.type != variant::MAP) {
            return NULL;
        }
        T* obj = new T();
        obj->fromVariant(val->second);
        return obj;
    }

    // No const operator[] cos it appears impossible to avoid copying
    /*const variant operator[](const char* key) const {
        assert(type==MAP);
        return *get(string(key));
    }
    const variant operator[](const string& key) const {
         assert(type==MAP);
        return *get(key);
    }*/
    variant& operator[](const char* key) {
        if (type==EMPTY) {
            type = MAP;
            _map = new map<string, variant>();
        }
        assert(type==MAP);
        return (*((_map->insert(std::make_pair(string(key),variant()))).first)).second;
    }

    const variant* get(const string& keypath) const;
    void set(const string& key, const variant& val);
    void set(const string& key, ISerializeToVariant* object);
    void clear();
    void appendVal(const variant& v);
    

    // Parsing from JSON and config files
    static variant parse(class StringProcessor& it, int flags);
    string toJson();
    static variant parseNumber(StringProcessor& it);

    // Javascript helpers
#ifdef PLATFORM_WEB
    val toJavascriptVal() const;
    void fromJavascriptVal(int valGOTindex);
#endif
    
    void setByteArray(uint8_t* p, int cb) {
        setType(BYTEARRAY);
        _bytearray->append(p, cb);
    }
    void setString(uint8_t* p, int cb) {
        setType(STRING);
        _str->assign((char*)p, cb);
    }
    void setDouble(double d) {
        setType(FLOAT64);
        _f64 = d;
    }

    friend class Stream;
    
private:
    inline void assign(const variant& src);
};

template <>
class Bitmap* variant::getObject<Bitmap>(const char* key) const;
