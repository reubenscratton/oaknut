//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#define PARSEFLAG_JSON 1
#define PARSEFLAG_EXPLICIT_ARRAY 2 // for non-Json


/**
 * @ingroup app_group
 * @brief Represents a measurement in one of the following units:
 *
 * * `PX`  Pixels
 * * `DP`  Device-independent Pixels
 * * `SP`  Scaled Pixels
 * * `PC`  Percent
 */
class measurement {
public:
    
    /** Returns the measurement value in pixels */
    float val() const;
    float _unitVal;
    enum unit {
        PX,
        DP,
        SP,
        PC
    } _unit;
    measurement(float v, unit unit) : _unitVal(v), _unit(unit) {}
    measurement(const measurement& m) : _unitVal(m._unitVal), _unit(m._unit) {}
    string toString() const;
};

/**
 * @ingroup base
 * @interface ISerializeToVariant
 * @brief Interface for objects that serialize to and from Variant.
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
 * Mainly used for serializing data and JSON.
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
        measurement _measurement;
        string* _str;
        bytearray* _bytearray;
        vector<variant>* _vec;
        map<string, variant>* _map;
        Object* _obj;
        error* _err;
    };
public:
    enum type {
        EMPTY,
        INT32,  INT64,
        UINT32, UINT64,
        FLOAT32, FLOAT64,
        MEASUREMENT,
        STRING,
        BYTEARRAY,
        ARRAY,
        MAP,
        OBJPTR,
        ERROR,
    } type;
    
    static variant empty();
    void setType(enum type newType);

    variant();
    variant(int32_t val);
    variant(int64_t val);
    variant(uint32_t val);
    variant(uint64_t val);
    variant(float val);
    variant(double val);
    variant(const bytearray& val);
    template<size_t N>
    variant(const char(&p)[N]) : type(STRING), _str(new string(p)) { }
    variant(const string& val);
    variant(const measurement& val);
    variant(const variant& var);
    variant(variant&& var) noexcept;
    //variant(class ISerializeToVariant* val);
    variant(const vector<variant>& val);
    variant(const vector<pair<string,variant>>& vals);
    variant(Object* obj);
    variant(const error& val);
    variant& operator=(const variant& rhs);
    bool operator<(const variant& rhs) const;
    ~variant();

    // Type tests
    bool isEmpty() const;
    bool isNumeric() const;
    bool isFloatingPoint() const;
    bool isString() const;
    bool isByteArray() const;
    bool isMeasurement() const;
    bool isArray() const;
    bool isCompound() const;
    bool isObject() const;
    bool isError() const;

    // Comparison
    bool operator ==(float val) {return type==FLOAT32 && _f32==val; }
    bool operator ==(int val) {return (type==INT32 && _i32==val) || (type==UINT32 && _u32==val); }
    bool operator ==(const string& val) {return type==STRING && _str->compare(val)==0; }
    
    // Accessors (by value, some types are coerced)
    int intVal() const;
    int intVal(const string& name) const;
    string stringVal() const;
    string stringVal(const string& name) const;
    bool boolVal() const;
    bool boolVal(const string& name) const;
    float floatVal() const;
    float floatVal(const string& name) const;
    double doubleVal() const;
    double doubleVal(const string& name) const;
    measurement measurementVal() const;
    vector<string> stringArrayVal() const;
    vector<string> stringArrayVal(const string& name) const;
    template <typename T>
    T* objectVal() const {
        static_assert(std::is_base_of<Object, T>::value, "type must be Object-derived");
        return (type==OBJPTR) ? static_cast<T*>(_obj) : nullptr;
    }

    // Accessors (by reference, no coercion)
    string& stringRef() const;
    vector<variant>& arrayRef() const;
    vector<variant>& arrayRef(const string& name) const;
    bytearray& bytearrayRef() const;
    bytearray& bytearrayRef(const string& name) const;
    map<string, variant>& compoundRef() const;
    map<string, variant>& compoundRef(const string& name) const;
    error& errorRef() const;

    template <class T>
    vector<sp<T>> arrayOf(const string& name) const {
        static_assert(std::is_base_of<ISerializeToVariant, T>::value, "T must implement ISerializeToVariant");
        vector<sp<T>> vec;
        auto& a = arrayRef(name);
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
    T get(const string& key) const {
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
    T* getObject(const string& key) const {
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
    variant& operator[](const string& key) {
        if (type==EMPTY) {
            type = MAP;
            _map = new map<string, variant>();
        }
        assert(type==MAP);
        return (*((_map->insert(std::make_pair(key,variant()))).first)).second;
    }

    variant& get(const string& keypath) const;
    bool hasVal(const string& key) const;
    void set(const string& key, const variant& val);
    void set(const string& key, ISerializeToVariant* object);
    void unset(const string& key);
    void clear();
    void appendVal(const variant& v);
    

    // Parsing from JSON and config files
    static variant parse(const string& str, int flags);
    string toJson() const;
    static variant parseNumber(const string& str, uint32_t& offset, bool convertExpressionsToStrings=false);

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
    void setInt32(int32_t i) {
        setType(INT32);
        _i32 = i;
    }
    void setDouble(double d) {
        setType(FLOAT64);
        _f64 = d;
    }

    uint32_t getRamCost() const;
    
    friend class bytestream;
    friend class Task;
    
#if DEBUG
    const char* debugString() const;
#endif
    
private:
    inline void assign(const variant& src);
    static variant parse(const string& str, uint32_t& o, int flags);
};

template <>
class Bitmap* variant::getObject<Bitmap>(const string& key) const;
