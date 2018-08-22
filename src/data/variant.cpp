
//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


Variant::Variant() : type(EMPTY) {
}
Variant::Variant(const Variant& var) : Variant(var.type) {
    assign(var);
}
Variant::Variant(Variant&& var) noexcept : type(var.type), d(var.d) {
    var.type = EMPTY;
    var.d = 0;
}
Variant::Variant(int32_t val) : type(INT32), i32(val) {
}
Variant::Variant(int64_t val) : type(INT64), i64(val) {
}
Variant::Variant(uint32_t val) : type(UINT32), u32(val) {
}
Variant::Variant(uint64_t val) : type(UINT64), u64(val) {
}
Variant::Variant(float val) : type(FLOAT32), f(val) {
}
Variant::Variant(double val) : type(FLOAT64), d(val) {
}
Variant::Variant(const char* val) : type(STRING), str(new string(val)) {
}
Variant::Variant(const string& val) : type(STRING), str(new string(val)) {
}
Variant::Variant(ByteBuffer* val, bool copy/*=false*/) : type(BYTEBUFFER) {
    new (&_data) ObjPtr<class ByteBuffer>(copy ? new ByteBuffer(*val) : val);
}
Variant::Variant(ISerializeToVariant* val) : type(val?MAP:EMPTY) {
    if (val) {
        _map = new map<string, Variant>();
        val->toVariant(*this);
    }
}
Variant::Variant(const vector<Variant>& val) : type(ARRAY) {
    _vec = new vector<Variant>(val.begin(), val.end());
}

Variant::Variant(vector<pair<const string&,const Variant&>> vals) : type(MAP) {
    _map = new map<string, Variant>();
    for (auto it : vals) {
        _map->insert(it);
    }
}

Variant::~Variant() {
    if (type == STRING) {
        delete str;
    }
    if (type == BYTEBUFFER) {
        _data.~ObjPtr();
    }
    if (type == ARRAY) {
        delete _vec;
    }
    if (type == MAP) {
        delete _map;
    }
}
void Variant::setType(VariantType newType) {
    if (type == newType) return;
    
    // Handle pointer type changes
    if (type == STRING) {
        delete str;
        str = NULL;
    } else if (type == BYTEBUFFER) {
        _data.~ObjPtr();
    } else if (type == ARRAY) {
        delete _vec;
        _vec = NULL;
    } else if (type == MAP) {
        delete _map;
        _map = NULL;
    }
    
    if (newType == BYTEBUFFER) {
        new (&_data) ObjPtr<class ByteBuffer>();
    }
    
    type = newType;

}
void Variant::assign(const Variant& src) {
    setType(src.type);
    switch (src.type) {
        case EMPTY: break;
        case INT32: i32 = src.i32; break;
        case INT64: i64 = src.i64; break;
        case UINT32: u32 = src.u32; break;
        case UINT64: u64 = src.u64; break;
        case FLOAT32: f = src.f; break;
        case FLOAT64: d = src.d; break;
        case STRING: str = new string(*src.str); break;
        case BYTEBUFFER: _data = new ByteBuffer(*src._data); break;
        case ARRAY: _vec = new vector<Variant>(src._vec->begin(), src._vec->end()); break;
        case MAP: _map = new map<string,Variant>(*src._map); break;
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
        case INT32: return i32 < rhs.i32;
        case INT64: return i64 < rhs.i64;
        case UINT32: return u32 < rhs.u32;
        case UINT64: return u64 < rhs.u64;
        case FLOAT32: return f < rhs.f;
        case FLOAT64: return d < rhs.d;
        case STRING: return *str < *(rhs.str);
        case BYTEBUFFER: {
            size_t cb = MIN(_data->cb, rhs._data->cb);
            int cv = memcmp(_data->data, rhs._data->data, cb);
            if (cv != 0) return cv<0;
            return _data->cb < rhs._data->cb;
        }
        case ARRAY: {
            for (int i=0 ; i<MIN(_vec->size(), rhs._vec->size()) ; i++) {
                if (_vec[i] < rhs._vec[i]) {
                    return true;
                }
            }
            return _vec->size() < rhs._vec->size();
        }
        case MAP: assert(0); break; // how would this work??
    }

}


int Variant::intVal() const {
    switch (type) {
        case EMPTY: return 0;
        case INT32: return i32;
        case INT64: return i64;
        case UINT32: return u32;
        case UINT64: return u64;
        case FLOAT32: return (int)f;
        case FLOAT64: return (int)d;
        case STRING: return atoi(str->data());
        default: break;
    }
    app.warn("intVal() called on non-numeric Variant");
    return 0;
}
int Variant::intVal(const char *name) const {
    return get(name)->intVal();
}


float Variant::floatVal() const {
    switch (type) {
        case EMPTY: return 0.f;
        case INT32: return (float)i32;
        case INT64: return (float)i64;
        case UINT32: return (float)u32;
        case UINT64: return (float)u64;
        case FLOAT32: return f;
        case FLOAT64: return (float)d;
        case STRING: return atof(str->data());
        default: break;
    }
    app.warn("floatVal() called on non-numeric Variant");
    return 0.f;
}
float Variant::floatVal(const char *name) const {
    auto val = get(name);
    if (!val) {
        return 0.f;
    }
    return val->floatVal();
}

double Variant::doubleVal() const {
    switch (type) {
        case EMPTY: return 0;
        case INT32: return (double)i32;
        case INT64: return (double)i64;
        case UINT32: return (double)u32;
        case UINT64: return (double)u64;
        case FLOAT32: return f;
        case FLOAT64: return d;
        case STRING: return atof(str->data());
        default: break;
    }
    app.warn("doubleVal() called on non-numeric Variant");
    return 0.;
}
double Variant::doubleVal(const char *name) const {
    return get(name)->doubleVal();
}

string Variant::stringVal() const {
    switch (type) {
        case EMPTY: return "";
        case INT32: return string::format("%d", i32);
        case INT64: return string::format("%ld", i64);
        case UINT32: return string::format("%u", u32);
        case UINT64: return string::format("%lu", u64);
        case FLOAT32: return string::format("%f", (double)f);
        case FLOAT64: return string::format("%f", d);
        case STRING: return *str;
        default: break;
    }
    if (type==ARRAY) {
        // todo: might be useful to concat the element stringVals...
    }
    app.warn("stringVal() called on non-stringable StyleValue");
    return "";
}
string Variant::stringVal(const char *name) const {
    auto val = get(name);
    if (!val) {
        return "";
    }
    return val->stringVal();
}

static vector<Variant> s_emptyVec;

const vector<Variant>& Variant::arrayVal() const {
    if (type==ARRAY) {
        return *_vec;
    }
    app.warn("arrayVal() called on non-array type");
    return s_emptyVec;
}

void Variant::appendVal(const Variant& v) {
    if (type!=ARRAY) {
        *this = arrayVal();
    }
    _vec->push_back(std::move(v));
}

const vector<Variant>& Variant::arrayVal(const char* name) const {
    return get(name)->arrayVal();
}

ByteBuffer* Variant::byteBufferVal() const {
    return (type==BYTEBUFFER) ? _data : NULL;
}

ByteBuffer* Variant::byteBufferVal(const char* name) const {
    return get(name)->byteBufferVal();
}


/*bool Variant::hasValue(const string& key) const {
    assert(type==MAP);
    return _map->find(key)!=_map->end();
}*/

const Variant* Variant::get(const string& key) const {
    assert(type==MAP);
    auto val = _map->find(key);
    if (val == _map->end()) {
        return NULL;
    }
    return &val->second;
}
void Variant::set(const string& key, const Variant& val) {
    assert(type==MAP);
    auto it = _map->emplace(std::move(key), val);
    if (!it.second) {
        assert(0);
        //it.first = val;
    }
}

void Variant::set(const string& key, ISerializeToVariant* val) {
    assert(type==MAP);
    if (!val) {
        return;
    }
    Variant v;
    val->toVariant(v);
    (*_map)[key] = v;
}

void Variant::clear() {
    assert(type==MAP);
    _map->clear();
}


bool Variant::readSelfFromStream(Stream* stream) {
    VariantType newType;
    if (!stream->readBytes(sizeof(newType), &newType)) {
        return false;
    }
    setType(newType);
    switch (type) {
        case EMPTY: return true;
        case INT32: return stream->readBytes(4, &i32);
        case INT64:return stream->readBytes(8, &i64);
        case UINT32: return stream->readBytes(4, &u32);
        case UINT64:return stream->readBytes(8, &u64);
        case FLOAT32:return stream->readBytes(sizeof(float), &f);
        case FLOAT64:return stream->readBytes(sizeof(double), &d);
        case STRING:{str = new string(); return stream->readString(str);};
        case BYTEBUFFER: {
            _data = new ByteBuffer();
            return _data->readSelfFromStream(stream);
        }
        case ARRAY: {
            _vec = new vector<Variant>();
            uint32_t num = 0;
            if (!stream->readUint32(&num)) {
                return false;
            }
            for (int i=0 ; i<num ; i++) {
                Variant value(Variant::EMPTY);
                if (!value.readSelfFromStream(stream)) {
                    return false;
                }
                _vec->push_back(value);
            }
            return true;
        }
        case MAP: {
            _map = new map<string, Variant>();
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
                _map->insert(make_pair(key,value));
            }
            return true;
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
        case INT32: return stream->writeBytes(4, &i32);
        case INT64:return stream->writeBytes(8, &i64);
        case UINT32: return stream->writeBytes(4, &u32);
        case UINT64:return stream->writeBytes(8, &u64);
        case FLOAT32:return stream->writeBytes(sizeof(float), &f);
        case FLOAT64:return stream->writeBytes(sizeof(double), &d);
        case STRING:return stream->writeString(*str);
        case BYTEBUFFER: return _data->writeSelfToStream(stream);
        case ARRAY: {
            uint32_t num = (uint32_t)_vec->size();
            if (!stream->writeUint32(num)) {
                return false;
            }
            for (auto& it : *_vec) {
                if (!it.writeSelfToStream(stream)) {
                    return false;
                }
            }
            return true;
        }
        case MAP: {
            uint32_t num = (uint32_t)_map->size();
            if (!stream->writeUint32(num)) {
                return false;
            }
            for (auto& it : *_map) {
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
    }
    return false;
}


string Variant::toJson() {
    switch (type) {
        case EMPTY: return "";
        case INT32: return string::format("%d", i32);
        case UINT32: return string::format("%u", u32);
        case INT64: return string::format("%ld", i64);
        case UINT64: return string::format("%lu", u64);
        case FLOAT32: return string::format("%f", (double)f);
        case FLOAT64: return string::format("%f", d);
        case STRING: return string::format("\"%s\"", str->data());
        case BYTEBUFFER: {
            app.warn("TODO! ByteBuffer json representation needed");
            break;
        }
        case ARRAY: {
            string s = "[";
            if (_vec) {
                for (auto it=_vec->begin() ; it != _vec->end() ; it++) {
                    if (it != _vec->begin()) s.append(",");
                    auto& elem = it;
                    s.append(elem->toJson());
                }
            }
            s.append("]");
            return s;
        
        }
        case MAP: {
            string s = "{";
            if (_map) {
                for (auto it=_map->begin() ; it != _map->end() ; it++) {
                    if (it != _map->begin()) s.append(",");
                    auto& field = it;
                    s.append(string::format("\"%s\":", field->first.data()));
                    s.append(field->second.toJson());
                }
            }
            s.append("}");
            return s;
        }
    }
    return "";
}


string parseJsonString(StringProcessor& it, int flags) {
    string str;
    bool allowLite = (flags & FLAG_ALLOW_JSONLITE);
    char32_t ch;
    if (!allowLite) {
        ch = it.next();
        if (ch != '\"') {
            app.log("Expected: \"");
            return str;
        }
    }
    for (;;) {
        auto charStart = it.current();
        char32_t ch = it.peek();
        if (allowLite) {
            if (ch == ',' || ch==':' || ch=='}') { // in jsonlite mode these are delimiters
                break;
            }
        }
        it.next();
        if (ch==0) {
            break;
        }
        if (!allowLite && ch=='\"') {
            break;
        }
        // Escapes
        if (ch=='\\') {
            charStart++;
            ch = it.next();
            bool legalEscape = (ch=='\"' || ch=='\'' || ch=='\\');
            if (allowLite) {
                legalEscape |= (ch == ',' || ch==':' || ch=='}');
            }
            if (!legalEscape) {
                app.warn("Illegal escape \'\\%c\'", ch);
                continue;
            }
        }
        str.append(string(charStart, it.current()));
    }
    
    return str;
}


// I cannot get strtol or stoi to work on Emscripten... they return garbage. Hence
// rolling my own...
Variant parseNumber(StringProcessor& it) {
    Variant val = 0;
    bool neg=(it.peek()=='-');
    if (neg) it.next();
    
    int base = 10;
    
    // Hex?
    if (it.peek()=='0') {
        it.next();
        if (it.peek()=='x') {
            it.next();
            base = 16;
        }
    }
    
    float fraction=0;
    for (;;) {
        char32_t ch = it.peek();
        if ((ch>='0' && ch<='9') || (base==16 && ((ch>='a'&&ch<='f')||(ch>='A'&&ch<='F')))) {
            int digit = (ch-'0');
            if (ch>='a' && ch<='f') digit = (ch+10-'a');
            else if (ch>='A' && ch<='F') digit = (ch+10-'A');
            if (fraction==0) {
                val.i32 = val.i32*base + digit;
            } else {
                fraction/=base;
                val.f += digit*fraction;
            }
            it.next();
        } else if (ch=='.') {
            fraction = 1;
            val.type = Variant::FLOAT32;
            val.f = val.i32;
            it.next();
        }
        
        else {
            break;
        }
    }
    if (neg) {
        if (val.type==Variant::INT32) val.i32 = -val.i32;
        if (val.type==Variant::FLOAT32) val.f = -val.f;
    }
    return val;
}

Variant Variant::parse(StringProcessor& it, int flags) {
    Variant val;
    it.skipWhitespace();
    char32_t ch = it.peek();
    if (ch == '{') {
        it.next();
        val.type = MAP;
        val._map = new map<string,Variant>();
        it.skipWhitespace();
        while (it.peek() != '}') {
            it.skipWhitespace();
            string fieldName = parseJsonString(it, flags);
            if (!fieldName.length()) {
                app.log("Invalid json: field name expected");
                return val;
            }
            it.skipWhitespace();
            if (it.next() != ':') {
                app.log("Invalid json: ':' expected");
                return val;
            }
            Variant fieldValue = Variant::parse(it, flags);
            if (fieldValue.type == Variant::EMPTY) {
                app.log("Invalid json: value expected");
                return val;
            }
            
            auto tt = val._map->emplace(std::move(fieldName), std::move(fieldValue));
            if (!tt.second) {
                assert(0);
                //it.first = val;
            }

            //val.set(fieldName, std::move(fieldValue));
            it.skipWhitespace();
            char32_t sep = it.peek();
            if (sep == ',') {
                it.next();
            } else {
                if (sep != '}') {
                    app.log("Invalid json: ',' or object end expected");
                    break;
                }
                it.next();
                break;
            }
        }
    }
    else if (ch == '[') {
        it.next();
        val.type = ARRAY;
        val._vec = new vector<Variant>();
        it.skipWhitespace();
        while (it.peek() != ']') {
            Variant element = Variant::parse(it, flags);
            val._vec->push_back(std::move(element));
            it.skipWhitespace();
            char32_t sep = it.peek();
            if (sep == ',') {
                it.next();
            } else {
                if (sep != ']') {
                    app.log("Invalid json: ',' or array end expected");
                    break;
                }
                it.next();
                break;
            }
        }
        
    }
    else if ((ch>='0' && ch<='9') || ch=='-' || ch=='.') {
        val = parseNumber(it);
    }
    else if (ch == '\"' || (flags & FLAG_ALLOW_JSONLITE)) {
        val = parseJsonString(it, flags);
    }
    return val;
}


#ifdef PLATFORM_WEB
val Variant::toJavascriptVal() const {
    switch (type) {
        case EMPTY: return val::undefined();
        case INT32: return val(i32);
        case INT64: {
            if (i64 & 0x7FC0000000000000LL) app.warn("i64 too big for JS Number");
            return val((double)i64);
        }
        case UINT32: return val(u32);
        case UINT64: {
            if (u64 & 0xFFC0000000000000ULL) app.warn("u64 too big for JS Number");
            return val((double)u64);
        }
        case FLOAT32: return val(f);
        case FLOAT64: return val(d);
        case STRING: return str ? val(str->data()) : val::null();
        case BYTEBUFFER: {
            app.warn("TODO! ByteBuffer serialization needed");
            break;
        }
        case ARRAY: {
            if (_vec) {
                val obj = val::global("Array").new_();
                for (auto it : *_vec) {
                    obj.call<void>("push", it.toJavascriptVal());
                }
                return obj;
            }
        }
        case MAP: {
            if (_map) {
                val obj = val::global("Object").new_();
                for (auto it : *_map) {
                    obj.set(it.first, it.second.toJavascriptVal());
                }
                return obj;
            }
        }
    }
    return val::null();
}
#endif

