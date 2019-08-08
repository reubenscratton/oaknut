
//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

float measurement::val() const {
    if (_unit==DP) return app->dp(_unitVal);
    if (_unit==SP) return app->dp(_unitVal);
    if (_unit==PC) return _unitVal/100;
    return _unitVal;
}

string measurement::toString() const {
    const char* szUnit;
    switch (_unit) {
        case DP: szUnit="dp"; break;
        case SP: szUnit="sp"; break;
        case PC: szUnit="pc"; break;
        case PX:
        default: szUnit="px"; break;
    }
    return string::format("%f%s", _unitVal, szUnit);
}

template <>
Bitmap* variant::getObject<Bitmap>(const char* key) const {
    auto val = _map->find(key);
    if (val == _map->end() || val->second.type != variant::MAP) {
        return NULL;
    }
    Bitmap* obj = Bitmap::create(0,0,0);
    obj->fromVariant(val->second);
    return obj;
}


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
variant::variant(const measurement& val) : type(MEASUREMENT), _measurement(val) {
}
variant::variant(const class bytearray& val) : type(BYTEARRAY), _bytearray(new class bytearray(val)) {
}
variant::variant(ISerializeToVariant* val) : type(val?MAP:EMPTY) {
    if (val) {
        _map = new map<string, variant>();
        val->toVariant(*this);
    }
}
variant::variant(const vector<variant>& val) : type(ARRAY) {
    _vec = new vector<variant>(val.begin(), val.end());
}

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
        _bytearray = new class bytearray();
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
        case MEASUREMENT: _measurement = src._measurement; break;
        case BYTEARRAY: _bytearray = new class bytearray(*src._bytearray); break;
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
        case MEASUREMENT: return _measurement.val() < (rhs._measurement.val());
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
        case MEASUREMENT: return (int)_measurement.val();
        default: break;
    }
    app->warn("intVal() called on non-numeric Variant");
    return 0;
}
bool variant::boolVal() const {
    switch (type) {
        case EMPTY: return false;
        case INT32: return _i32!=0;
        case INT64: return _i64!=0;
        case UINT32: return _u32!=0;
        case UINT64: return _u64!=0;
        case FLOAT32: return _f32!=0.f;
        case FLOAT64: return _f64!=0.;
        case STRING: {
            if (_str->compare("false")==0) return false;
            else if (_str->compare("true")==0) return true;
            break;
        }
        case MEASUREMENT: return _measurement.val()!=0.f;
        default: break;
    }
    app->warn("boolVal() type coerce failed");
    return false;
}

int variant::intVal(const char *name) const {
    return get(name).intVal();
}
bool variant::boolVal(const char* name) const {
    return get(name).boolVal();
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
        case MEASUREMENT: return _measurement.val();
        case STRING: return atof(_str->data());
        default: break;
    }
    app->warn("floatVal() called on non-numeric Variant");
    return 0.f;
}
float variant::floatVal(const char *name) const {
    return get(name).floatVal();
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
        case MEASUREMENT: _measurement.val();
        case STRING: return atof(_str->data());
        default: break;
    }
    app->warn("doubleVal() called on non-numeric Variant");
    return 0.;
}
double variant::doubleVal(const char *name) const {
    return get(name).doubleVal();
}

measurement variant::measurementVal() const {
    if (type == MEASUREMENT) {
        return _measurement;
    }
    if (isNumeric()) {
        return measurement(floatVal(), measurement::PX);
    }
    app->warn("measurement() type coerce failed");
    return measurement(0,measurement::PX);
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
    app->warn("stringVal() called on non-stringable StyleValue");
    return "";
}
string variant::stringVal(const char *name) const {
    return get(name).stringVal();
}
vector<string> variant::stringArrayVal() const {
    vector<string> vec;
    if (isArray()) {
        auto& a = arrayRef();
        for (auto e : a) {
            vec.push_back(e.stringVal());
        }
    } else if (!isEmpty()) {
        vec.push_back(stringVal());
    }
    return vec;
}

vector<string> variant::stringArrayVal(const char* name) const {
    return get(name).stringArrayVal();
}

bool variant::isEmpty() const {
    return type == EMPTY;
}

bool variant::isNumeric() const {
    return type == INT32 || type == INT64
        || type == UINT32 || type == UINT64
        || type == FLOAT32 || type == FLOAT64
        || type == MEASUREMENT;
}
bool variant::isFloatingPoint() const {
    return type == FLOAT32 || type == FLOAT64;
}
bool variant::isArray() const {
    return type == ARRAY;
}
bool variant::isString() const {
    return type == STRING;
}
bool variant::isMeasurement() const {
    return type == MEASUREMENT;
}
bool variant::isCompound() const {
    return type == MAP;
}

static vector<variant> s_emptyVec;
static map<string, variant> s_emptyCompound;
static string s_emptyStr;

string& variant::stringRef() const {
    if (type == STRING) {
        return *_str;
    }
    app->warn("stringRef() called on non-string type");
    return s_emptyStr;
}

vector<variant>& variant::arrayRef() const {
    if (type==ARRAY) {
        return *_vec;
    }
    app->warn("arrayRef() called on non-array type");
    return s_emptyVec;
}

map<string, variant>& variant::compoundRef() const {
    if (type==MAP) {
        return *_map;
    }
    app->warn("compoundRef() called on non-compound type");
    return s_emptyCompound;
}

void variant::appendVal(const variant& v) {
    if (type==EMPTY) {
        setType(ARRAY);
    }
    assert(type==ARRAY);
    _vec->push_back(std::move(v));
}

vector<variant>& variant::arrayRef(const char* name) const {
    return get(name).arrayRef();
}
map<string, variant>& variant::compoundRef(const char* name) const {
    return get(name).compoundRef();
}

bytearray& variant::bytearrayRef() const {
    assert (type==BYTEARRAY);
    return *_bytearray;
}

bytearray& variant::bytearrayRef(const char* name) const {
    return get(name).bytearrayRef();
}


bool variant::hasVal(const string& name) const {
    assert(type==MAP);
    return _map->find(name)!=_map->end();
}

static variant s_emptyVar;

variant& variant::get(const string& key) const {
    if (type==EMPTY) {
        return s_emptyVar;
    }
    assert(type==MAP);
    auto val = _map->find(key);
    if (val == _map->end()) {
        return s_emptyVar;
    }
    return val->second;
}
void variant::set(const string& key, const variant& val) {
    if (type==EMPTY) {
        setType(MAP);
    }
    assert(type==MAP);
    auto it = _map->emplace(std::move(key), val);
    if (!it.second) {
        it.first->second = val;
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

void variant::unset(const string& key) {
    if (type==EMPTY) {
        return;
    }
    assert(type==MAP);
    _map->erase(std::move(key));
}

void variant::clear() {
    setType(EMPTY);
    //_map->clear();
}


#if DEBUG
const char* variant::debugString() const {
    string j = toJson();
    char* foo = (char*)malloc(j.lengthInBytes()+1);
    memcpy(foo, j.data(), j.lengthInBytes()+1);
    return foo;
}
#endif

string variant::toJson() const {
    switch (type) {
        case EMPTY: return "";
        case INT32: return string::format("%d", _i32);
        case UINT32: return string::format("%u", _u32);
        case INT64: return string::format("%ld", _i64);
        case UINT64: return string::format("%lu", _u64);
        case FLOAT32: return string::format("%f", (double)_f32);
        case FLOAT64: return string::format("%f", _f64);
        case STRING: return string::format("\"%s\"", _str->data());
        case MEASUREMENT: return string::format("\"%s\"", _measurement.toString().data());
        case BYTEARRAY: {
            app->warn("TODO! bytearray json representation needed");
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


string parseString(StringProcessor& it) {
    string str;
    bool quoted = false, endquote = false;
    char32_t ch = it.peek();
    quoted = (ch == '\"');
    if (quoted) {
        it.next();
    }
    for (;;) {
        auto charStart = it.current();
        ch = it.peek();
        if (!quoted) {
            if (ch=='\n' || ch=='\r' || ch == ',' || ch==':' || ch=='}') {
                break;
            }
        }
        it.next();
        if (ch==0) {
            break;
        }
        if (quoted && ch=='\"') {
            endquote = true;
            break;
        }
        // Escapes
        if (ch=='\\') {
            charStart++;
            ch = it.next();
            bool legalEscape = (ch=='\"' || ch=='\'' || ch=='\\');
            if (!quoted) {
                legalEscape |= (ch == ',' || ch==':' || ch=='}');
            }
            if (!legalEscape) {
                app->warn("Illegal escape \'\\%c\'", ch);
                continue;
            }
        }
        //str.append(string(charStart, it.current()));
        str.append(ch);
    }
    if (quoted && !endquote) {
        app->warn("Expected closing \"");
    }
    
    return str;
}


// I cannot get strtol or stoi to work on Emscripten... they return garbage. Hence
// rolling my own...
variant variant::parseNumber(StringProcessor& it) {
    variant val = 0ULL;
    
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
    
    bool isFloat = false;
    double fraction=0;
    for (;;) {
        char32_t ch = it.peek();
        if ((ch>='0' && ch<='9') || (base==16 && ((ch>='a'&&ch<='f')||(ch>='A'&&ch<='F')))) {
            int digit = (ch-'0');
            if (ch>='a' && ch<='f') digit = (ch+10-'a');
            else if (ch>='A' && ch<='F') digit = (ch+10-'A');
            if (fraction==0) {
                val._i64 = val._i64*base + digit;
            } else {
                fraction/=base;
                val._f64 += digit*fraction;
            }
            it.next();
        } else if (ch=='.') {
            isFloat = true;
            fraction = 1;
            val._f64 = val._i64;
            it.next();
        }
        
        else {
            break;
        }
    }
    if (neg) {
        if (isFloat) val._f64 = -val._f64;
        else val._i64 = -val._i64;
    }
    
    // Choose appropriate size, 64 or 32
    if (isFloat) {
        if (val._f64 < FLT_MIN || val._f64 > FLT_MAX) {
            val.type = variant::FLOAT64;
        } else {
            val.type = variant::FLOAT32;
            val._f32 = (float)val._f64;
        }
    } else {
        if (val._i64 < INT_MIN || val._i64 > INT_MAX) {
            val.type = variant::INT64;
        } else {
            val.type = variant::INT32;
            val._i32 = (int32_t)val._i64;
        }
    }

    
    // Handle measurement suffixes
    if (it.nextWas("%")) {
        val._measurement = measurement(val.floatVal(), measurement::PC);
        val.type=variant::MEASUREMENT;
    } else if (it.nextWas("dp")) {
        val._measurement = measurement(val.floatVal(), measurement::DP);
        val.type=variant::MEASUREMENT;
    } else if (it.nextWas("sp")) {
        val._measurement = measurement(val.floatVal(), measurement::SP);
        val.type=variant::MEASUREMENT;
    } else if (it.nextWas("px")) {
        val._measurement = measurement(val.floatVal(), measurement::PX);
        val.type=variant::MEASUREMENT;
    }
    return val;
}


variant variant::parse(StringProcessor& it, int flags) {
    variant val;
    it.skipWhitespaceAndComments();
    

    // Parse a compound value
    char32_t ch = it.peek();
    if (ch == '{') {
        it.next();
        val.type = MAP;
        val._map = new map<string,variant>();
        it.skipWhitespaceAndComments();
        while (it.peek() != '}' && !it.eof()) {
            it.skipWhitespaceAndComments();
            string fieldName = parseString(it);
            //app->log(fieldName.data());
            if (!fieldName.length()) {
                app->log("Invalid json: field name expected");
                return val;
            }
            it.skipWhitespaceAndComments();
            if (it.next() != ':') {
                app->log("Invalid json: ':' expected");
                return val;
            }
            variant fieldValue = parse(it, (flags & ~PARSEFLAG_EXPLICIT_ARRAY));
            
            auto tt = val._map->emplace(std::move(fieldName), std::move(fieldValue));
            if (!tt.second) {
                assert(0);
                //it.first = val;
            }
            
            it.skipWhitespaceAndComments();

            // JSON object fields are separated by commas
            if (flags & PARSEFLAG_JSON) {
                if (it.peek() == ',') {
                    it.next();
                    it.skipWhitespace();
                }
            }
            //val.set(fieldName, std::move(fieldValue));
            /*it.skipWhitespace();
            char32_t sep = it.peek();
            if (sep == ',') {
                it.next();
            } else {
                if (sep != '}') {
                    app->log("Invalid json: ',' or object end expected");
                }
                break;
            }*/
        }
        it.next();
    }
    
    // Parse an explicit array
    else if (ch == '[') {
        it.next();
        val.type = ARRAY;
        val._vec = new vector<variant>();
        it.skipWhitespaceAndComments();
        while (it.peek() != ']' && !it.eof()) {
            variant element = variant::parse(it, flags | PARSEFLAG_EXPLICIT_ARRAY);
            val._vec->push_back(std::move(element));
            it.skipWhitespaceAndComments();
            char32_t sep = it.peek();
            if (sep == ',') {
                it.next();
            } else {
                if (sep != ']') {
                    app->log("Invalid json: ',' or array end expected");
                }
                break;
            }
        }
        if (it.peek() == ']') {
            it.next();
        }
    }
    
    // Scalar values
    else if ((ch>='0' && ch<='9') || ch=='-' || ch=='.') {
        val = parseNumber(it);
    } else if (ch == 'n' && it.nextWas("null")) {
        val.setType(EMPTY); // should we have null? is it really meaningful?
    } else if (ch == 't' && it.nextWas("true")) {
        val.setInt32(1);
    } else if (ch == 'f' && it.nextWas("false")) {
        val.setInt32(0);
    } else {
        val = parseString(it);
    }

    // Implicit array value
    it.skipSpacesAndTabs();
    char32_t sep = it.peek();
    if (sep == ',' && !(flags&(PARSEFLAG_JSON|PARSEFLAG_EXPLICIT_ARRAY))) {
        //it.next();
        variant firstElem(std::move(val));
        val.setType(ARRAY);
        val._vec->push_back(std::move(firstElem));
        while (it.peek() == ',') {
            it.next();
            variant elem = variant::parse(it, 0);
            val._vec->push_back(std::move(elem));
            it.skipSpacesAndTabs();
        }

    }
    

    
    return val;
}


#ifdef PLATFORM_WEB
val variant::toJavascriptVal() const {
    switch (type) {
        case EMPTY: return val::undefined();
        case INT32: return val(_i32);
        case INT64: {
            if (_i64 & 0x7FC0000000000000LL) app->warn("i64 too big for JS Number");
            return val((double)_i64);
        }
        case UINT32: return val(_u32);
        case UINT64: {
            if (_u64 & 0xFFC0000000000000ULL) app->warn("u64 too big for JS Number");
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


//#include <string>

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
static void setVariantToArray(variant* v, int gotIndexArray) {
    v->setType(variant::ARRAY);
    val jsarray = val::global("gotGet")(gotIndexArray).as<val>();
    for (int i=0 ; i<jsarray["length"].as<int>() ; i++) {
        val jselem = jsarray[val(i)];
        variant elem;
        elem.fromJavascriptVal(val::global("gotSet")(jselem).as<int>());
        v->appendVal(elem);
    }
}

void variant::fromJavascriptVal(int valGOTindex) {
    EM_ASM({
        var variantPtr = $0;
        var jsval = gotGet($1);
        var t = typeof(jsval);
        if (t == "object") {
            if (jsval.constructor === Array) {
                Runtime.dynCall("vii", $6, [variantPtr, gotSet(jsval)]);
            }
            else if (jsval instanceof Uint8Array) {
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
    }, this, valGOTindex, setVariantToByteArray, setVariantToString, setVariantCompoundField, setVariantToNumber, setVariantToArray);
}

#endif

