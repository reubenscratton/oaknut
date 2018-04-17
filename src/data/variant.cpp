//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


Variant::Variant() : Variant(EMPTY) {
}
Variant::Variant(VariantType type) : type(EMPTY) {
    setType(type);
}
Variant::Variant(const Variant& var) : Variant(var.type) {
    assign(var);
}
Variant::~Variant() {
    if (type == BYTEBUFFER) {
        data.~ObjPtr();
    }
    if (type == MAP) {
        map.~ObjPtr();
    }
}
void Variant::setType(VariantType newType) {
    if (type == newType) return;
    
    // Handle smart-pointer type changes
    if (type == BYTEBUFFER && newType != BYTEBUFFER) {
        data.~ObjPtr();
    } else if (type != BYTEBUFFER && newType == BYTEBUFFER) {
        new (&data) ObjPtr<ByteBuffer>();
    }
    if (type == MAP && newType != MAP) {
        map.~ObjPtr();
    } else if (type != MAP && newType == MAP) {
        new (&map) ObjPtr<VariantMap>();
    }
    
    type = newType;

}
void Variant::assign(const Variant& src) {
    setType(src.type);
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
        case FLOAT32: f = src.f; break;
        case FLOAT64: d = src.d; break;
        case BYTEBUFFER: data = src.data; break;
        case MAP: map = src.map; break;
    }
}
Variant& Variant::operator=(const Variant& var) {
    assign(var);
    return *this;
}
bool Variant::operator<(const Variant& rhs) const {
    assert(type == rhs.type); // no cross-type comparison allowed
    switch (type) {
        case EMPTY: return false;
        case INT8: return i8 < rhs.i8;
        case INT16: return i16 < rhs.i16;
        case INT32: return i32 < rhs.i32;
        case INT64: return i64 < rhs.i64;
        case UINT8: return u8 < rhs.u8;
        case UINT16: return u16 < rhs.u16;
        case UINT32: return u32 < rhs.u32;
        case UINT64: return u64 < rhs.u64;
        case FLOAT32: return f < rhs.f;
        case FLOAT64: return d < rhs.d;
        case BYTEBUFFER: {
            size_t cb = min(data->cb, rhs.data->cb);
            int cv = memcmp(data->data, rhs.data->data, cb);
            if (cv != 0) return cv<0;
            return data->cb < rhs.data->cb;
        }
        case MAP: assert(0); break; // how would this work??
    }

}


bool Variant::readSelfFromStream(Stream* stream) {
    VariantType newType;
    if (!stream->readBytes(sizeof(newType), &newType)) {
        return false;
    }
    setType(newType);
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
        case FLOAT32:return stream->readBytes(sizeof(float), &f);
        case FLOAT64:return stream->readBytes(sizeof(double), &d);
        case BYTEBUFFER: {
            data = new ByteBuffer();
            return data->readSelfFromStream(stream);
        }
        case MAP: {
            map = new VariantMap();
            return map->readSelfFromStream(stream);
        }
    }
    return false;
}
bool Variant::writeSelfToStream(Stream* stream) const {
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
        case FLOAT32:return stream->writeBytes(sizeof(float), &f);
        case FLOAT64:return stream->writeBytes(sizeof(double), &d);
        case BYTEBUFFER: return data->writeSelfToStream(stream);
        case MAP: return map->writeSelfToStream(stream);
    }
    return false;
}
