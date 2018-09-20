//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

/*

 - Resources are a hierarchy of JSON-style key-value pairs but without quote marks around everything.
 - No commas needed to separate fields, field ends at newline unless it is a quoted string which is
   allowed to contain newlines and escaped characters.
 - Resources are treated as both a map and a list.
 - Keys are always strings, values are [string|int|float|map]
 - Keys are chainable, i.e. the hierarchy can be traversed by "key.subkey.value".
 - Values cascade, i.e. if a requested value is not present in a map then the parent map is checked, then the grandparent, etc.
 - Values can refer to other values via a $ prefix.
 - Maps can 'extend' other maps that aren't ancestors via a $ declaration in the key name
 - Later declarations override earlier ones if they have exactly the same name.

*/

float Measurement::val() const {
    if (_unit==DP) return app.dp(_val);
    if (_unit==SP) return app.dp(_val);
    return _val;
}

StyleValue::StyleValue() : type(Empty) {
}
StyleValue::StyleValue(const StyleValue& rval) : type(Empty) {
    copyFrom(&rval);
}
StyleValue::StyleValue(StyleValue&& rval) noexcept : type(rval.type), str(NULL) {
    switch (rval.type) {
        case Empty: break;
        case Int: i = rval.i; rval.i=0; break;
        case Float: f = rval.f; rval.f=0; break;
        case String: str = rval.str; rval.str = NULL; break;
        case Measure: measurement = rval.measurement; break;
        case Reference: str = rval.str; rval.str = NULL; break;
        case Array: array = rval.array; rval.array=NULL; break;
        case Compound: compound = rval.compound; rval.compound=NULL; break;
        case QualifiedCompound: compound = rval.compound; rval.compound=NULL; break;
    }
    rval.type = Empty;
}
StyleValue::~StyleValue() {
    if (type == Compound) {
        delete compound;
    } else if (type == Array) {
        delete array;
    } else if (type == String) {
        str.~string();
    }
}

bool StyleValue::isEmpty() const {
    return type==Type::Empty;
}
bool StyleValue::isNumeric() const {
    return type == Type::Int || type == Type::Float || type == Type::Measure;
}
bool StyleValue::isString() const {
    return type==Type::String;
}
bool StyleValue::isArray() const {
    return type == Type::Array;
}

int StyleValue::intVal() const {
    auto val = select();
    if (val->type==Type::Int) return val->i;
    else if (val->type==Type::Float) return (int)val->f;
    else if (val->type==Type::Measure) return (int)val->measurement.val();
    app.warn("intVal() type coerce failed");
    return 0.f;
}
bool StyleValue::boolVal() const {
    auto val = select();
    if (val->type==Type::Int) return val->i != 0;
    else if (val->type==Type::Float) return ((int)val->f) != 0;
    else if (val->type==Type::String) {
        if (val->str == "false") return false;
        else if (val->str == "true") return true;
    }
    app.warn("boolVal() type coerce failed");
    return 0.f;
}

int StyleValue::intVal(const string& name) const {
    auto val = select();
    assert(val->type == Compound);
    auto val2 = val->compound->find(name);
    if (val2 == val->compound->end()) {
        app.warn("Value missing for field '%'", name.data());
        return 0;
    }
    return val2->second.intVal();
}
float StyleValue::floatVal() const {
    auto val = select();
    if (val->type==Type::Int) return (float)val->i;
    else if (val->type==Type::Float) return val->f;
    else if (val->type==Type::Measure) return val->measurement.val();
    app.warn("floatVal() type coerce failed");
    return 0.f;
}

string StyleValue::stringVal() const {
    auto val = select();
    if (val->type==Type::String) return val->str;
    else if (val->type==Type::Int) return string::format("%d", val->i);
    else if (val->type==Type::Float) return string::format("%f", val->f);
    else if (val->type==Type::Array) {
        // todo: might be useful to concat the element stringVals...
    }
    app.warn("stringVal() type coerce failed");
    return "";
}
string StyleValue::stringVal(const string& name) const {
    return get(name)->stringVal();
}

static vector<StyleValue> s_emptyArray;

const vector<StyleValue>& StyleValue::arrayVal() const {
    auto val = select();
    if (val->type==Array) {
        return *array;
    }
    app.warn("arrayVal() type coerce failed");
    return s_emptyArray;
}
const vector<StyleValue>& StyleValue::arrayVal(const string& name) const {
    auto val = get(name);
    if (val) {
        return val->arrayVal();
    }
    return s_emptyArray;
}

static map<string, StyleValue> s_emptyMap;

const map<string, StyleValue>& StyleValue::compoundVal() const {
    auto val = select();
    if (val->type==Type::Compound) return *(val->compound);
    app.warn("compoundVal() type coerce failed");
    return s_emptyMap;
}


COLOR StyleValue::colorVal()  const {
    auto val = select();
    if (val->type==Type::Int) {
        return (COLOR)val->i;
    }
    else if (val->type==Type::Float) {
        uint8_t lum = val->f * 255;
        return 0xFF000000 | (lum<<16) | (lum<<8) | lum;
    } else if (val->type==Type::String) {
        // TODO: make this method non-const and have it mutate type to Int after the colour is parsed
        string colorVal = val->str;
        // value is in either '#fff', '#ffffff', '#ffffffff',
        if (colorVal.hadPrefix("#")) {
            unsigned int val = (unsigned int)strtol(colorVal.data(), NULL, 16);
            if (colorVal.length() == 3) {
                int r = (val & 0xF00)>>8;
                int g = (val & 0xF0)>>4;
                int b = val & 0xF;
                return 0xFF000000
                     | (((r<<4)|r)<<16)
                     | (((g<<4)|g)<<8)
                     | ((b<<4)|b);
            }
            else if (colorVal.length()==6) {
                return 0xFF000000 | val;
            }
            else if (colorVal.length()==8) {
                return val;
            }
            else {
                app.warn("Malformed hex color value");
                return 0;
            }
        }
        // 'rgb(255,255,255)' or 'rgba(255,255,255,255) notation
        else if (colorVal.hadPrefix("rgb(")) {
            //
        }
        // Web color names
        else {
            static map<string, COLOR> htmlColors = {
                // Pinks
                {"pink", 0xffffc0cb},
                {"lightpink", 0xffffb6c1},
                {"hotpink", 0xffff69b4},
                {"deeppink", 0xffff1493},
                {"palevioletred", 0xffdb7093},
                {"mediumvioletred", 0xffc71585},
                // reds
                {"lightsalmon", 0xffffa07a},
                {"salmon", 0xfffa8072},
                {"darksalmon", 0xffe9967a},
                {"lightcoral", 0xfff08080},
                {"indianred", 0xffcd5c5c},
                {"crimson", 0xffdc143c},
                {"firebrick", 0xffb22222},
                {"darkred", 0xff8b0000},
                {"red", 0xffff0000},
                // oranges
                {"orangered", 0xffff4500},
                {"tomato", 0xffff6347},
                {"coral", 0xffff7f50},
                {"darkorange", 0xffff8c00},
                {"orange", 0xffffa500},
                // yellows
                {"yellow", 0xffffff00},
                {"lightyellow", 0xffffffe0},
                {"lemonchiffon", 0xfffffacd},
                {"lightgoldenrodyellow", 0xfffafad2},
                {"papayawhip", 0xffffefd5},
                {"moccasin", 0xffffe4b5},
                {"peachpuff", 0xffffdab9},
                {"palegoldenrod", 0xffeee8aa},
                {"khaki", 0xfff0e68c},
                {"darkkhaki", 0xffbdb76b},
                {"gold", 0xffffd700},
                // browns
                {"cornsilk", 0xfffff8dc},
                {"blanchedalmond", 0xffffebcd},
                {"bisque", 0xffffe4c4},
                {"navajowhite", 0xffffdead},
                {"wheat", 0xfff5deb3},
                {"burlywood", 0xffdeb887},
                {"tan", 0xffd2b48c},
                {"rosybrown", 0xffbc8f8f},
                {"sandybrown", 0xfff4a460},
                {"goldenrod", 0xffdaa520},
                {"darkgoldenrod", 0xffb8860b},
                {"peru", 0xffcd853f},
                {"chocolate", 0xffd2691e},
                {"saddlebrown", 0xff8b4513},
                {"sienna", 0xffa0522d},
                {"brown", 0xffa52a2a},
                {"maroon", 0xff800000},
                // greens
                {"darkolivegreen", 0xff556b2f},
                {"olive", 0xff808000},
                {"olivedrab", 0xff6b8e23},
                {"yellowgreen", 0xff9acd32},
                {"limegreen", 0xff32cd32},
                {"lime", 0xff00ff00},
                {"lawngreen", 0xff7cfc00},
                {"chartreuse", 0xff7fff00},
                {"greenyellow", 0xffadff2f},
                {"springgreen", 0xff00ff7f},
                {"mediumspringgreen", 0xff00fa9a},
                {"lightgreen", 0xff90ee90},
                {"palegreen", 0xff98fb98},
                {"darkseagreen", 0xff8fbc8f},
                {"mediumaquamarine", 0xff66cdaa},
                {"mediumseagreen", 0xff3cb371},
                {"seagreen", 0xff2e8b57},
                {"forestgreen", 0xff228b22},
                {"green", 0xff008000},
                {"darkgreen", 0xff006400},
                // cyans
                {"aqua", 0xff00ffff},
                {"cyan", 0xff00ffff},
                {"lightcyan", 0xffe0ffff},
                {"paleturquoise", 0xffafeeee},
                {"aquamarine", 0xff7fffd4},
                {"turquoise", 0xff40e0d0},
                {"mediumturquoise", 0xff48d1cc},
                {"darkturquoise", 0xff00ced1},
                {"lightseagreen", 0xff20b2aa},
                {"cadetblue", 0xff5f9ea0},
                {"darkcyan", 0xff008b8b},
                {"teal", 0xff008080},
                // blues
                {"lightsteelblue", 0xffb0c4de},
                {"powderblue", 0xffb0e0e6},
                {"lightblue", 0xffadd8e6},
                {"skyblue", 0xff87ceeb},
                {"lightskyblue", 0xff87cefa},
                {"deepskyblue", 0xff00bfff},
                {"dodgerblue", 0xff1e90ff},
                {"cornflowerblue", 0xff6495ed},
                {"steelblue", 0xff4682b4},
                {"royalblue", 0xff4169e1},
                {"blue", 0xff0000ff},
                {"mediumblue", 0xff0000cd},
                {"darkblue", 0xff00008b},
                {"navy", 0xff000080},
                {"midnightblue", 0xff191970},
                // purples
                {"lavender", 0xffe6e6fa},
                {"thistle", 0xffd8bfd8},
                {"plum", 0xffdda0dd},
                {"violet", 0xffee82ee},
                {"orchid", 0xffda70d6},
                {"fuchsia", 0xffff00ff},
                {"magenta", 0xffff00ff},
                {"mediumorchid", 0xffba55d3},
                {"mediumpurple", 0xff9370db},
                {"blueviolet", 0xff8a2be2},
                {"darkviolet", 0xff9400d3},
                {"darkorchid", 0xff9932cc},
                {"darkmagenta", 0xff8b008b},
                {"purple", 0xff800080},
                {"indigo", 0xff4b0082},
                {"darkslateblue", 0xff483d8b},
                {"slateblue", 0xff6a5acd},
                {"mediumslateblue", 0xff7b68ee},
                // whites
                {"white", 0xffffffff},
                {"snow", 0xfffffafa},
                {"honeydew", 0xfff0fff0},
                {"mintcream", 0xfff5fffa},
                {"azure", 0xfff0ffff},
                {"aliceblue", 0xfff0f8ff},
                {"ghostwhite", 0xfff8f8ff},
                {"whitesmoke", 0xfff5f5f5},
                {"seashell", 0xfffff5ee},
                {"beige", 0xfff5f5dc},
                {"oldlace", 0xfffdf5e6},
                {"floralwhite", 0xfffffaf0},
                {"ivory", 0xfffffff0},
                {"antiquewhite", 0xfffaebd7},
                {"linen", 0xfffaf0e6},
                {"lavenderblush", 0xfffff0f5},
                {"mistyrose", 0xffffe4e1},
                // grays and black
                {"gainsboro", 0xffdcdcdc},
                {"lightgray", 0xffd3d3d3},
                {"silver", 0xffc0c0c0},
                {"darkgray", 0xffa9a9a9},
                {"gray", 0xff808080},
                {"dimgray", 0xff696969},
                {"lightslategray", 0xff778899},
                {"slategray", 0xff708090},
                {"darkslategray", 0xff2f4f4f},
                {"black", 0xff000000}
            };
            auto it = htmlColors.find(colorVal.lowercase());
            if (it != htmlColors.end()) {
                return it->second;
            }
        }

    }
    app.warn("colorVal() called on incompatible StyleValue");
    return 0;
}


Vector4 StyleValue::cornerRadiiVal() const {
    Vector4 r;
    auto val = select();
    if (val->isNumeric()) {
        r.x = r.y = r.z = r.w = val->floatVal();
    } else {
        assert(val->isArray());
        auto& radii = val->arrayVal();
        if (radii.size()==1) {
            r.x = r.y = r.z = r.w = radii[0].floatVal();
        } else if (radii.size()==4) {
            r.x = radii[0].floatVal();
            r.y = radii[1].floatVal();
            r.z = radii[2].floatVal();
            r.w = radii[3].floatVal();
        } else {
            app.warn("Invalid corner-radii, must be 1 or 4 values");
            r = {0,0,0,0};
        }
    }
    return r;
}

EDGEINSETS StyleValue::edgeInsetsVal() const {
    EDGEINSETS insets;
    auto val = select();
    if (val->isNumeric()) {
        insets.left = insets.top = insets.right = insets.bottom = val->floatVal();
    } else {
        assert(val->isArray());
        auto& a = val->arrayVal();
        if (a.size()==1) {
            insets.left = insets.top = insets.right = insets.bottom = a[0].floatVal();
        } else if (a.size()==2) {
            insets.left = insets.right = a[0].floatVal();
            insets.top = insets.bottom = a[1].floatVal();
        } else if (a.size()==4) {
            insets.left = a[0].floatVal();
            insets.top = a[1].floatVal();
            insets.right = a[2].floatVal();
            insets.bottom = a[3].floatVal();
        } else {
            insets = {0,0,0,0};
            app.warn("Invalid inset, must be 1 or 4 values");
        }
    }
    return insets;
}

float StyleValue::fontWeightVal() const {
    auto val = select();
    if (val->isNumeric()) {
        return val->floatVal();
    } else if (val->isString()) {
        string fw = val->stringVal().lowercase();
        if (fw=="bold") return FONT_WEIGHT_BOLD;
        if (fw=="regular") return FONT_WEIGHT_REGULAR;
        if (fw=="medium") return FONT_WEIGHT_MEDIUM;
        if (fw=="semibold") return FONT_WEIGHT_SEMIBOLD;
        if (fw=="light") return FONT_WEIGHT_LIGHT;
        if (fw=="thin") return FONT_WEIGHT_THIN;
        if (fw=="ultralight") return FONT_WEIGHT_ULTRA_LIGHT;
        if (fw=="heavy") return FONT_WEIGHT_HEAVY;
        if (fw=="black") return FONT_WEIGHT_BLACK;
    }
    app.warn("Invalid fontWeight");
    return 0;
}

const StyleValue* StyleValue::select() const {
    const StyleValue* val = this;
    if (type == QualifiedCompound) {
        val = NULL;
        for (auto it = compound->begin() ; it != compound->end() ; it++) {
            const string& qual = it->first;
            bool applies = false;
            if (qual == "") {
                applies = true;
            }
            else if (qual == "iOS") {
    #if TARGET_OS_IOS
                applies = true;
    #endif
            } else if (qual == "OSX") {
    #if TARGET_OS_OSX
                applies = true;
    #endif
            } else if (qual == "android") {
    #ifdef ANDROID
                applies = true;
    #endif
            } else {
                app.warn("Unsupported qualifier '%s'", qual.data());
            }
            if (applies) {
                // TODO: apply precedence that favours higher specificity
                val = &it->second;
            }
        }
    }
    
    // Follow references to other declarations
    while (val && val->type == Type::Reference) {
        val = app.getStyleValue(val->str);
    }

    return val;
}

const StyleValue* StyleValue::get(const string& keypath) const {
    auto val = this;
    string subkey, key = keypath;
    do {
        val = val->select();
        if (!val) {
            return NULL;
        }
        auto dotIndex = key.find(L'.');
        if (dotIndex>0) {
            subkey = key.substr(dotIndex+1, -1);
            key = key.substr(0, dotIndex);
        } else {
            subkey = "";
        }
        assert(val->type == Compound);
        auto it = val->compound->find(key);
        if (it == val->compound->end()) {
            //app.warn("Value missing for field '%s'", keypath.data());
            return NULL;
        }
        val = &it->second;
        key = subkey;
    } while (key.length() > 0);
    return val->select();
}


bool StyleValue::parse(StringProcessor& it, bool inArrayVal/*=false*/) {
    it.skipWhitespace();

    // Parse a compound value
    if (it.peek() == '{') {
        it.next();
        setType(StyleValue::Type::Compound);
        while (!it.eof()) {
            it.skipWhitespace();
            if (it.peek()=='}') {
                it.next();
                break;
            }
            string fieldName = it.nextToken();
            if (fieldName.length() == 0) {
                app.log("Error: expected a field name");
                return false;
            }
            
            // Comment line
            if (fieldName == "#") {
                it.nextToEndOfLine();
                continue;
            }
            
            // Split out the qualifier suffix, if there is one
            string fieldNameQualifier = "";
            int qualifierStartsAt = fieldName.find('@');
            if (qualifierStartsAt > 0) {
                fieldNameQualifier = fieldName.substr(qualifierStartsAt+1, -1);
                fieldName = fieldName.substr(0, qualifierStartsAt);
            }
            
            it.skipWhitespace();
            if (it.next() != ':') {
                app.log("Error: expected \':\' after identifier \'%s\'", fieldName.data());
                return false;
            }
            it.skipWhitespace();
            
            // Parse the value
            StyleValue value;
            if (!value.parse(it)) {
                return false;
            }
            
            // If there's no qualifier then set the default value
            auto existingField = compound->find(fieldName);
            if (fieldNameQualifier.length() <= 0) {
                if (existingField == compound->end()) {
                    compound->insert(std::pair<string, StyleValue>(fieldName, std::move(value)));
                } else {
                    if (existingField->second.type == QualifiedCompound) {
                        existingField->second.compound->insert(make_pair("", std::move(value)));
                    } else {
                        if (existingField->second.type == Compound && value.type == Compound) {
                            for (auto i=value.compound->begin() ; i!=value.compound->end() ; i++) {
                                existingField->second.compound->operator[](i->first) = std::move(i->second);
                            }
                        } else {
                            existingField->second.copyFrom(&value);
                        }
                    }
                }
            }
            
            // Qualified value
            else {
                // If this is the first value (and it has a qualifier) then create the
                // qualified compound and add the first element to it.
                if (existingField == compound->end()) {
                    StyleValue qualCom;
                    qualCom.setType(QualifiedCompound);
                    qualCom.compound->insert(make_pair(fieldNameQualifier, std::move(value)));
                    compound->insert(make_pair(fieldName, std::move(qualCom)));
                } else {
                    // This is true if we've parsed the default value only, now we have to
                    // promote existingField to being a qualified compound
                    if (existingField->second.type != QualifiedCompound) {
                        StyleValue defaultValue(std::move(existingField->second));
                        existingField->second.setType(QualifiedCompound);
                        existingField->second.compound->insert(make_pair("", std::move(defaultValue)));
                        existingField->second.compound->insert(make_pair(fieldNameQualifier, std::move(value)));
                    }
                    else {
                        existingField->second.compound->insert(make_pair(fieldNameQualifier, std::move(value)));
                    }
                }
            }
        }
    }
    
    // Array (explitly declared with square brackets)
    else if (it.peek() == '[') {
        it.next();
        setType(Array);
        while (!it.eof()) {
            it.skipWhitespace();
            if (it.peek()==']') {
                it.next();
                break;
            }
            
            // Parse the next element
            StyleValue elem;
            if (!elem.parse(it, true)) {
                return false;
            }
            array->push_back(std::move(elem));
            
            // Next non-whitespace char must be a comma or the closing square bracket
            it.skipWhitespace();
            if (it.peek()==',') {
                it.next();
            } else {
                if (it.peek()!=']') {
                    app.warn("expected ']'");
                    return false;
                }
            }
        }
    }
    
    // Non-compound
    else {
        char ch = it.peek();
        if ((ch>='0' && ch<='9') || ch=='-') {
            variant v = variant::parseNumber(it);
            if (it.nextWas("dp")) {
                setType(Measure);
                measurement = Measurement(v.floatVal(), Measurement::DP);
            }
            else if (it.nextWas("sp")) {
                setType(Measure);
                measurement = Measurement(v.floatVal(), Measurement::SP);
            }
            else if (it.nextWas("px")) {
                setType(Measure);
                measurement = Measurement(v.floatVal(), Measurement::PX);
            } else {
                if (v.type == variant::FLOAT32) {
                    setType(Float);
                    f = v.floatVal();
                } else {
                    setType(Int);
                    i = v.floatVal();
                }
            }
            
            // String value
        } else {
            setType(it.nextWas("$") ? Reference : String);
            bool quotedString = it.nextWas("\"");
            while (!it.eof()) {
                ch = it.peek();
                if (!quotedString) {
                    if (ch=='\r' || ch=='\n' || ch==',') {
                        break;
                    }
                    it.next();
                } else {
                    it.next();
                    if (ch=='\"') {
                        break;
                    }
                    if (ch=='\\') {
                        char32_t escapeChar = it.next();
                        switch (escapeChar) {
                            case '\\': ch='\\'; break;
                            case '\"': ch='\"'; break;
                            case '\'': ch='\''; break;
                            case 'n': ch='\n'; break;
                            case 'r': ch='\r'; break;
                            case 't': ch='\t'; break;
                            case 'u': assert(0); // todo! implement unicode escapes
                            default: app.warn("invalid escape '\\%c'", escapeChar); break;
                        }
                    }
                }
                str.append(ch);
            }
        }
    }

    // Implicit array (detected when there's a comma after the value)
    it.skipWhitespace();
    if (!inArrayVal && it.peek() == ',') {
        StyleValue firstElem(std::move(*this));
        setType(Array);
        array->push_back(std::move(firstElem));
        while (it.peek() == ',') {
            it.next();
            StyleValue elem;
            if (!elem.parse(it, true)) {
                return false;
            }
            array->push_back(std::move(elem));
        }
    }

    

    return true;
}



void StyleValue::setType(Type newType) {
    if (type == newType) return;
    
    // Handle non-trivial type changes
    if (type == Compound && newType != Compound) {
        delete compound;
    } else if (type != Compound && newType == Compound) {
        compound = new map<string,StyleValue>();
    }
    if (type == QualifiedCompound && newType != QualifiedCompound) {
        delete compound;
    } else if (type != QualifiedCompound && newType == QualifiedCompound) {
        compound = new map<string,StyleValue>();
    }
    bool wasString = (type==String || type==Reference);
    bool isString = (newType==String || newType==Reference);
    if (wasString && !isString) {
        str.~string();
    } else if (!wasString && isString) {
        new (&str) string();
    }
    bool wasArray = (type==Array);
    bool isArray = (newType==Array);
    if (wasArray && !isArray) {
        delete array;
    } else if (!wasArray && isArray) {
        array = new vector<StyleValue>();
    }

    type = newType;
    
}

StyleValue& StyleValue::operator=(const StyleValue& other) {
	if (this != &other)  {
        copyFrom(&other);
    }
    return *this;
}

void StyleValue::copyFrom(const StyleValue* other) {
    setType(other->type);
    switch (other->type) {
        case Empty: break;
        case Int: i = other->i; break;
        case Float: f = other->f; break;
        case String: str = other->str; break;
        case Measure: measurement = other->measurement; break;
        case Reference: str = other->str; break;
        case Array: array->insert(array->end(), other->array->begin(), other->array->end()); break;
        case Compound:
        case QualifiedCompound: compound->insert(other->compound->begin(), other->compound->end()); break;
    }
}

void StyleValue::importValues(const map<string, StyleValue>& values) {
    assert(type == Compound);
    compound->insert(values.begin(), values.end());
}
