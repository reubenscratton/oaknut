//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 A discriminated union of all elementary types plus a couple of useful extras.
 */

class Variant : public ISerializable {
public:
    enum VariantType {
        EMPTY,
        INT8,
        INT16,
        INT32,
        INT64,
        UINT8,
        UINT16,
        UINT32,
        UINT64,
        FLOAT32,
        FLOAT64,
        STRING,
        BYTEBUFFER,
        MAP
    } type;
    union {
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        float f;
        double d;
        string* str; // std::string is 24 bytes, hence use pointer
        ObjPtr<class VariantMap> map;
        ObjPtr<ByteBuffer> data; // 8 bytes
    };
    Variant();
    Variant(int32_t val);
    Variant(int64_t val);
    Variant(uint32_t val);
    Variant(uint64_t val);
    Variant(float val);
    Variant(double val);
    Variant(ByteBuffer* val);
    Variant(ObjPtr<ByteBuffer>& val);
    Variant(const char* val);
    Variant(const string& val);
    Variant(const Variant& var);
    Variant(class ISerializeToVariantMap* val);
    Variant(vector<pair<const string&,const Variant&>> vals);
    Variant& operator=(const Variant& rhs);
    bool operator<(const Variant& rhs) const;
    ~Variant();
    
    // Comparison
    bool operator ==(float val) {return type==FLOAT32 && f==val; }
    bool operator ==(int val) {return (type==INT32 && i32==val) || (type==UINT32 && u32==val); }
    bool operator ==(const string& val) {return type==STRING && str->compare(val)==0; }

    // Implicit conversions
    operator int32_t() const { assert(type==INT32); return i32; };
    operator uint32_t() const { assert(type==UINT32); return u32; };
    operator int64_t() const { assert(type==INT64); return i64; };
    operator uint64_t() const { assert(type==UINT64); return u64; };
    operator float() const { assert(type==FLOAT32); return f; };
    operator double() const { assert(type==FLOAT64); return d; };
    operator string() const { assert(type==STRING); return *str; }
    operator ByteBuffer*() const { assert(type==BYTEBUFFER); return data; }
    operator VariantMap&() const { assert(type==MAP); return *map; }

    // ISerializable
    virtual bool readSelfFromStream(class Stream* stream);
    virtual bool writeSelfToStream(Stream* stream) const;
    
private:
    inline void assign(const Variant& src);
    void setType(VariantType newType);
};
