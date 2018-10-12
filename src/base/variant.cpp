
//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


variant::variant() : type(EMPTY) {
}
variant::variant(const variant& var) : variant(var.type) {
    assign(var);
}
variant::variant(variant&& var) noexcept : type(var.type), _f64(var._f64) {
    var.type = EMPTY;
    var._f64 = 0;
}
variant::variant(int32_t val) : type(INT32), _i32(val) {
}
variant::variant(int64_t val) : type(INT64), _i64(val) {
}
variant::variant(uint32_t val) : type(UINT32), _u32(val) {
}
variant::variant(uint64_t val) : type(UINT64), _u64(val) {
}
variant::variant(float val) : type(FLOAT32), _f32(val) {
}
variant::variant(double val) : type(FLOAT64), _f64(val) {
}
variant::variant(const char* val) : type(STRING), _str(new string(val)) {
}
variant::variant(const string& val) : type(STRING), _str(new string(val)) {
}
variant::variant(const bytearray& val) : type(BYTEARRAY), _bytearray(new bytearray(val)) {
}
variant::variant(ISerializeToVariant* val) : type(val?MAP:EMPTY) {
    if (val) {
        _map = new map<string, variant>();
        val->toVariant(*this);
    }
}
//variant::variant(const vector<variant>& val) : type(ARRAY) {
//    _vec = new vector<variant>(val.begin(), val.end());
//}

variant::variant(const vector<pair<string,variant>>& vals) : type(MAP) {
    _map = new map<string, variant>();
    for (auto it : vals) {
        _map->insert(it);
    }
}

variant::~variant() {
    if (type == STRING) {
        delete _str;
    } else if (type == BYTEARRAY) {
        delete _bytearray;
    } else if (type == ARRAY) {
        delete _vec;
    } else if (type == MAP) {
        delete _map;
    }
}
void variant::setType(enum type newType) {
    if (type == newType) return;
    
    // Handle pointer type changes
    if (type == STRING) {
        delete _str;
        _str = NULL;
    } else if (type == BYTEARRAY) {
        delete _bytearray;
        _bytearray = NULL;
    } else if (type == ARRAY) {
        delete _vec;
        _vec = NULL;
    } else if (type == MAP) {
        delete _map;
        _map = NULL;
    }
    
    
    if (newType == STRING) {
        _str = new string();
    } else if (newType == BYTEARRAY) {
        _bytearray = new bytearray();
    } else if (newType == ARRAY) {
        _vec = new vector<variant>();
    } else if (newType == MAP) {
        _map = new map<string,variant>();
    }
    
    type = newType;

}
void variant::assign(const variant& src) {
    setType(EMPTY);
    switch (src.type) {
        case EMPTY: break;
        case INT32: _i32 = src._i32; break;
        case INT64: _i64 = src._i64; break;
        case UINT32: _u32 = src._u32; break;
        case UINT64: _u64 = src._u64; break;
        case FLOAT32: _f32 = src._f32; break;
        case FLOAT64: _f64 = src._f64; break;
        case STRING: _str = new string(*src._str); break;
        case BYTEARRAY: _bytearray = new bytearray(*src._bytearray); break;
        case ARRAY: _vec = new vector<variant>(src._vec->begin(), src._vec->end()); break;
        case MAP: _map = new map<string,variant>(*src._map); break;
    }
    type = src.type;
}
variant& variant::operator=(const variant& var) {
    assign(var);
    return *this;
}
bool variant::operator<(const variant& rhs) const {
    assert(type == rhs.type); // no cross-type comparison allowed
    switch (type) {
        case EMPTY: return false;
        case INT32: return _i32 < rhs._i32;
        case INT64: return _i64 < rhs._i64;
        case UINT32: return _u32 < rhs._u32;
        case UINT64: return _u64 < rhs._u64;
        case FLOAT32: return _f32 < rhs._f32;
        case FLOAT64: return _f64 < rhs._f64;
        case STRING: return *_str < *(rhs._str);
        case BYTEARRAY: {
            size_t cb = MIN(_bytearray->size(), rhs._bytearray->size());
            int cv = memcmp(_bytearray->data(), rhs._bytearray->data(), cb);
            if (cv != 0) return cv<0;
            return _bytearray->size() < rhs._bytearray->size();
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


int variant::intVal() const {
    switch (type) {
        case EMPTY: return 0;
        case INT32: return _i32;
        case INT64: return _i64;
        case UINT32: return _u32;
        case UINT64: return _u64;
        case FLOAT32: return (int)_f32;
        case FLOAT64: return (int)_f64;
        case STRING: return atoi(_str->data());
        default: break;
    }
    app.warn("intVal() called on non-numeric Variant");
    return 0;
}
int variant::intVal(const char *name) const {
    return get(name)->intVal();
}


float variant::floatVal() const {
    switch (type) {
        case EMPTY: return 0.f;
        case INT32: return (float)_i32;
        case INT64: return (float)_i64;
        case UINT32: return (float)_u32;
        case UINT64: return (float)_u64;
        case FLOAT32: return _f32;
        case FLOAT64: return (float)_f64;
        case STRING: return atof(_str->data());
        default: break;
    }
    app.warn("floatVal() called on non-numeric Variant");
    return 0.f;
}
float variant::floatVal(const char *name) const {
    auto val = get(name);
    if (!val) {
        return 0.f;
    }
    return val->floatVal();
}

double variant::doubleVal() const {
    switch (type) {
        case EMPTY: return 0;
        case INT32: return (double)_i32;
        case INT64: return (double)_i64;
        case UINT32: return (double)_u32;
        case UINT64: return (double)_u64;
        case FLOAT32: return _f32;
        case FLOAT64: return _f64;
        case STRING: return atof(_str->data());
        default: break;
    }
    app.warn("doubleVal() called on non-numeric Variant");
    return 0.;
}
double variant::doubleVal(const char *name) const {
    return get(name)->doubleVal();
}

string variant::stringVal() const {
    switch (type) {
        case EMPTY: return "";
        case INT32: return string::format("%d", _i32);
        case INT64: return string::format("%ld", _i64);
        case UINT32: return string::format("%u", _u32);
        case UINT64: return string::format("%lu", _u64);
        case FLOAT32: return string::format("%f", (double)_f32);
        case FLOAT64: return string::format("%f", _f64);
        case STRING: return *_str;
        default: break;
    }
    if (type==ARRAY) {
        // todo: might be useful to concat the element stringVals...
    }
    app.warn("stringVal() called on non-stringable StyleValue");
    return "";
}
string variant::stringVal(const char *name) const {
    auto val = get(name);
    if (!val) {
        return "";
    }
    return val->stringVal();
}

static vector<variant> s_emptyVec;

const vector<variant>& variant::arrayVal() const {
    if (type==ARRAY) {
        return *_vec;
    }
    app.warn("arrayVal() called on non-array type");
    return s_emptyVec;
}

void variant::appendVal(const variant& v) {
    if (type==EMPTY) {
        setType(ARRAY);
    }
    assert(type==ARRAY);
    _vec->push_back(std::move(v));
}

const vector<variant>& variant::arrayVal(const char* name) const {
    return get(name)->arrayVal();
}

const bytearray& variant::bytearrayVal() const {
    assert (type==BYTEARRAY);
    return *_bytearray;
}

const bytearray& variant::bytearrayVal(const char* name) const {
    return get(name)->bytearrayVal();
}


/*bool variant::hasValue(const string& key) const {
    assert(type==MAP);
    return _map->find(key)!=_map->end();
}*/


const variant* variant::get(const string& key) const {
    if (type==EMPTY) {
        return NULL;
    }
    assert(type==MAP);
    auto val = _map->find(key);
    if (val == _map->end()) {
        return NULL;
    }
    return &val->second;
}
void variant::set(const string& key, const variant& val) {
    if (type==EMPTY) {
        setType(MAP);
    }
    assert(type==MAP);
    auto it = _map->emplace(std::move(key), val);
    if (!it.second) {
        assert(0);
        //it.first = val;
    }
}

void variant::set(const string& key, ISerializeToVariant* val) {
    if (type==EMPTY) {
        setType(MAP);
    }
    assert(type==MAP);
    if (!val) {
        return;
    }
    variant v;
    val->toVariant(v);
    (*_map)[key] = v;
}

void variant::clear() {
    assert(type==MAP);
    _map->clear();
}



string variant::toJson() {
    switch (type) {
        case EMPTY: return "";
        case INT32: return string::format("%d", _i32);
        case UINT32: return string::format("%u", _u32);
        case INT64: return string::format("%ld", _i64);
        case UINT64: return string::format("%lu", _u64);
        case FLOAT32: return string::format("%f", (double)_f32);
        case FLOAT64: return string::format("%f", _f64);
        case STRING: return string::format("\"%s\"", _str->data());
        case BYTEARRAY: {
            app.warn("TODO! bytearray json representation needed");
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
variant variant::parseNumber(StringProcessor& it) {
    variant val = 0;
    
    // Skip leading '+' and '-'
    bool neg = false;
    while (it.peek()=='+' || it.peek()=='-') {
        if (it.peek()=='-') {
            neg=!neg;
        }
        it.next();
        it.skipWhitespace();
    }
    
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
                val._i32 = val._i32*base + digit;
            } else {
                fraction/=base;
                val._f32 += digit*fraction;
            }
            it.next();
        } else if (ch=='.') {
            fraction = 1;
            val.type = variant::FLOAT32;
            val._f32 = val._i32;
            it.next();
        }
        
        else {
            break;
        }
    }
    if (neg) {
        if (val.type==variant::INT32) val._i32 = -val._i32;
        if (val.type==variant::FLOAT32) val._f32 = -val._f32;
    }
    return val;
}

variant variant::parse(StringProcessor& it, int flags) {
    variant val;
    it.skipWhitespace();
    char32_t ch = it.peek();
    if (ch == '{') {
        it.next();
        val.type = MAP;
        val._map = new map<string,variant>();
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
            variant fieldValue = parse(it, flags);
            /* The JSON value 'null' maps to EMPTY, so this block is no longer needed
             if (fieldValue.type == EMPTY) {
                app.log("Invalid json: value expected");
                return val;
            }*/
            
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
        val._vec = new vector<variant>();
        it.skipWhitespace();
        while (it.peek() != ']') {
            variant element = variant::parse(it, flags);
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
    else if (ch == 'n') {
        if (it.nextWas("null")) {
            val.setType(EMPTY); // should we have null? is it really meaningful?
        }
    } else {
        assert(0);
    }
    return val;
}


#ifdef PLATFORM_WEB
val variant::toJavascriptVal() const {
    switch (type) {
        case EMPTY: return val::undefined();
        case INT32: return val(_i32);
        case INT64: {
            if (_i64 & 0x7FC0000000000000LL) app.warn("i64 too big for JS Number");
            return val((double)_i64);
        }
        case UINT32: return val(_u32);
        case UINT64: {
            if (_u64 & 0xFFC0000000000000ULL) app.warn("u64 too big for JS Number");
            return val((double)_u64);
        }
        case FLOAT32: return val(_f32);
        case FLOAT64:
            return val(_f64);
        case STRING: return _str ? val(_str->data()) : val::null();
        case BYTEARRAY: {
            return val(typed_memory_view(_bytearray->size(), _bytearray->data()));
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
                    obj.set(val(it.first.data()), it.second.toJavascriptVal());
                }
                return obj;
            }
        }
    }
    return val::null();
}


#include <string>

/*template <>
inline string const *emscripten::val::as<string const *>(void) const {
    using namespace emscripten;
    using namespace internal;
    
    EM_DESTRUCTORS destructors;
    EM_GENERIC_WIRE_TYPE result = _emval_as(
                                            handle,
                                            TypeID<AllowedRawPointer<string const>>::get(),
                                            &destructors
                                            );
    DestructorsRunner destructors_runner(destructors);
    
    return fromGenericWireType<string *>(result);
}
string *my_constructor(emscripten::val value) {
    return new string("bollocks");
}
EMSCRIPTEN_BINDINGS(String) {
    emscripten::class_<string>("string")
    .constructor<>()
    .constructor<emscripten::val>(&my_constructor, emscripten::allow_raw_pointers())
    ;
}
*/


static void setVariantToByteArray(variant* v, uint8_t* p, int32_t cb) {
    v->setByteArray(p, cb);
}
static void setVariantToString(variant* v, uint8_t* p, int32_t cb) {
    v->setString(p, cb);
}
static void setVariantCompoundField(variant* v, char* szFieldName, int fieldGOTindex) {
    variant field;
    field.fromJavascriptVal(fieldGOTindex);
    v->set(string(szFieldName), field);
}
static void setVariantToNumber(variant* v, double d) {
    v->setDouble(d);
}

void variant::fromJavascriptVal(int valGOTindex) {
    EM_ASM({
        var variantPtr = $0;
        var jsval = gotGet($1);
        var t = typeof(jsval);
        if (t == "object") {
            if (jsval instanceof Uint8Array) {
                var ptr = _malloc(jsval.length);
                (new Uint8Array(HEAPU8.buffer, ptr, jsval.length)).set(jsval);
                Runtime.dynCall("viii", $2, [variantPtr, ptr, jsval.length]);
                _free(ptr);
            } else {
                for (var f in jsval) {
                    var len = lengthBytesUTF8(f)+1;
                    var ptr=_malloc(len);
                    stringToUTF8(f, ptr, len);
                    var field = jsval[f];
                    Runtime.dynCall("viii", $4, [variantPtr, ptr, gotSet(field)]);
                    _free(ptr);
                }
            }
        }
        else if (t == "string") {
            var len = lengthBytesUTF8(jsval)+1;
            var ptr=_malloc(len);
            stringToUTF8(jsval, ptr, len);
            Runtime.dynCall("viii", $3, [variantPtr, ptr, len]);
            _free(ptr);
        } else if (t == "number") {
            Runtime.dynCall("vid", $5, [variantPtr, jsval]);
        } else if (t == "undefined") {
        } else {
            console.log("unsupported JS type " + typeof(jsval));
        }
    }, this, valGOTindex, setVariantToByteArray, setVariantToString, setVariantCompoundField, setVariantToNumber);
}

#endif

