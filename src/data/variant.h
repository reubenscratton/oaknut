//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 A discriminated union of all elementary types plus a couple of foundation types.
 It is only intended to be used during serialization (i.e. to support KeyValueMap).
 It is expected that most classes will serialize via KeyValueMap.
 */
typedef enum {
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
    BYTEBUFFER,
    MAP
} VariantType;

/**
 A discriminated union of all elementary types plus a couple of foundation types.
 It is only intended to be used during serialization (i.e. to support KeyValueMap).
 It is expected that most classes will serialize via KeyValueMap.
 */

class Variant : public ISerializableBase {
public:
    VariantType type;
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
        ObjPtr<ByteBuffer> data;
        ObjPtr<class VariantMap> map;
    };
    Variant();
    Variant(VariantType type);
    Variant(const Variant& var);
    Variant& operator=(const Variant& rhs);
    bool operator<(const Variant& rhs) const;
    ~Variant();
    
    // ISerializableBase
    virtual bool readSelfFromStream(class Stream* stream);
    virtual bool writeSelfToStream(Stream* stream) const;
    
private:
    inline void assign(const Variant& src);
    void setType(VariantType newType);
};
