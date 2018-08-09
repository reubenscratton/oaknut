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
bool StyleValue::isEmpty() const {
    return type==Type::Empty;
}
bool StyleValue::isNumeric() const {
    return type == Type::Int || type == Type::Float || type == Type::Measure;
}
int StyleValue::intVal() const {
    auto val = select();
    if (val->type==Type::Int) return val->i;
    else if (val->type==Type::Float) return (int)val->f;
    else if (val->type==Type::Measure) return (int)val->measurement.val();
    app.warn("intVal() called on non-numeric StyleValue");
    return 0.f;
}
float StyleValue::floatVal() const {
    auto val = select();
    if (val->type==Type::Int) return (float)val->i;
    else if (val->type==Type::Float) return val->f;
    else if (val->type==Type::Measure) return val->measurement.val();
    app.warn("floatVal() called on non-numeric StyleValue");
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
    app.warn("stringVal() called on non-stringable StyleValue");
    return "";
}

const vector<const StyleValue*> StyleValue::arrayVal() const {
    auto val = select();
    if (val->type==Type::Array) {
        return array;
    }
    return {this};
}

StyleMap* StyleValue::compoundVal() const {
    auto val = select();
    if (val->type==Type::Compound) return val->compound;
    app.warn("compoundVal() called on non-compound StyleValue");
    return NULL;
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

void StyleValue::setQualifiedValue(const string& qual, const StyleValue* value) {
    
    // Setting default value
    if (qual.length() <= 0) {
        if (type==Compound && value->type==Compound) {
            for (auto jt : value->compound->_values) {
                compound->_values[jt.first] = jt.second;
            }
        } else {
            assign(value);
        }
    }

    //
    else {
        auto existingValue = _qualifiedValues.find(qual);

        // Assigning a map where one already exists must not *replace*, we must merge the new map into the old
        if (existingValue!=_qualifiedValues.end() && value->type == Type::Compound) {
            assert(0); // when does this happen?
        } else {
            _qualifiedValues.insert(make_pair(qual, value));
        }
    }
}

Vector4 StyleValue::cornerRadiiVal() const {
    auto radii = arrayVal();
    Vector4 r;
    if (radii.size()==1) {
        r.x = r.y = r.z = r.w = radii[0]->floatVal();
    } else if (radii.size()==4) {
        r.x = radii[0]->floatVal();
        r.y = radii[1]->floatVal();
        r.z = radii[2]->floatVal();
        r.w = radii[3]->floatVal();
    } else {
        app.warn("Invalid corner-radii, must be 1 or 4 values");
        r = {0,0,0,0};
    }
    return r;
}


const StyleValue* StyleValue::select() const {
    const StyleValue* val = this;
    for (auto it : _qualifiedValues) {
        const string& qual = it.first;
        bool applies = false;
        if (qual == "iOS") {
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
            val = it.second;
        }
    }
    
    // Follow references to other declarations
    while (val && val->type == Type::Reference) {
        val = app.getStyleValue(val->str);
    }

    return val;
}

StyleValue* StyleMap::getValue(const string& keypath) {
    auto dotIndex = keypath.find(L'.');
    if (dotIndex>0) {
        string key = keypath.substr(0, dotIndex);
        string subkey = keypath.substr(dotIndex+1, -1);
        StyleValue* val = _values[key];
        if (val) {
            StyleMap* compound = val->compoundVal();
            if (!compound) {
                app.log("Error: compound val expected for '%s'", key.data());
                return NULL;
            }
            return compound->getValue(subkey);
        }
    } else {
        // Keypath has no dots so is just a value name
        StyleValue* val = _values[keypath];
        if (val) {
            return val;
        }
        if (_parent) {
            return _parent->getValue(keypath);
        }
    }
    return NULL;
}


bool StyleMap::parse(StringProcessor& it) {
    it.skipWhitespace();
    if ('{' != it.next()) {
        app.log("Error: expected '{'");
        return false;
    }
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
        
        StyleValue* value = new StyleValue();
        if (it.peek() == '{') {
            value->setType(StyleValue::Type::Compound);
            value->compound = new StyleMap();
            if (!value->compound->parse(it)) {
                return false;
            }
            value->compound->_parent = this;
        } else {
            string str = it.nextToEndOfLine();
            if (str.length() == 0) {
                delete value;
                app.log("Error: expected a value");
                return false;
            }
            
            if (str.contains(',')) {
                value->setType(StyleValue::Type::Array);
                while (str.length() > 0) {
                    auto substr = str.tokenise(",");
                    substr.trim();
                    auto elem = new StyleValue();
                    parseSingleValue(elem, substr);
                    value->array.push_back(elem);
                }
                
            } else {
                parseSingleValue(value, str);
            }
            
        }


        StyleValue* mapval = _values[fieldName];
        if (!mapval) {
            mapval = new StyleValue();
            _values[fieldName] = mapval;
        }
        mapval->setQualifiedValue(fieldNameQualifier, value);
        StyleValue* listval = new StyleValue();
        listval->setQualifiedValue(fieldNameQualifier, value);
        _valuesList.push_back(make_pair(fieldName, listval));
        
    }
    return true;
}

void StyleMap::parseSingleValue(StyleValue* value, string& str) {
    char ch = *str.begin();
    if (ch>='0' && ch<='9') {
        if (str.hadSuffix("dp")) {
            value->setType(StyleValue::Type::Measure);
            value->measurement = Measurement(atof(str.data()), Measurement::DP);
        }
        else if (str.hadSuffix("sp")) {
            value->setType(StyleValue::Type::Measure);
            value->measurement = Measurement(atof(str.data()), Measurement::SP);
        }
        else if (str.hadSuffix("px")) {
            value->setType(StyleValue::Type::Measure);
            value->measurement = Measurement(atof(str.data()), Measurement::PX);
        } else {
            bool isFloat = str.contains('.');
            if (isFloat) {
                value->setType(StyleValue::Type::Float);
                value->f = atof(str.data());
            } else {
                value->setType(StyleValue::Type::Int);
                value->i = stringParseInt(str);
            }
        }
    } else {
        if (str.hadPrefix("\"")) {
            string unesc_str = "";
            for (auto it=str.begin() ; it !=str.end() ; it++) {
                char32_t ch = *it;
                if (ch=='\"') {
                    break;
                }
                if (ch=='\\') {
                    it++;
                    ch = *it;
                    switch (ch) {
                        case 'n': ch='\n'; break;
                        case 'r': ch='\r'; break;
                        case 't': ch='\t'; break;
                        case 'u': assert(0); // todo! implement unicode escapes
                    }
                }
                unesc_str.append(ch);
            }
            str = unesc_str;
        }
        
        if (str.hadPrefix("$")) {
            value->setType(StyleValue::Type::Reference);
        } else {
            value->setType(StyleValue::Type::String);
        }
        value->str = str;
    }

}

StyleValue::StyleValue() : type(Empty) {
}
StyleValue::StyleValue(const StyleValue& rval) : type(rval.type) {
	switch (rval.type) {
    case Empty: break;
    case Int: i = rval.i; break;
    case Float: f = rval.f; break;
	case String: str = rval.str; break;
    case Measure: measurement = rval.measurement; break;
    case Reference: str = rval.str; break;
    case Array: array = rval.array; break;
	case Compound: compound = rval.compound; break;
	}
}
StyleValue::~StyleValue() {
    if (type == Compound) {
        compound.~ObjPtr();
    } else if (type == String) {
        str.~string();
    }
}
void StyleValue::setType(Type newType) {
    if (type == newType) return;
    
    // Handle non-trivial type changes
    if (type == Compound && newType != Compound) {
        compound.~ObjPtr();
    } else if (type != Compound && newType == Compound) {
        new (&compound) ObjPtr<class StyleMap>();
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
        array.~vector();
    } else if (!wasArray && isArray) {
        new (&array) vector<StyleValue*>();
    }

    type = newType;
    
}

StyleValue& StyleValue::operator=(const StyleValue& other) {
	if (this != &other)  {
        assign(&other);
    }
    return *this;
}

void StyleValue::assign(const StyleValue* other) {
    setType(other->type);
    switch (other->type) {
        case Empty: break;
        case Int: i = other->i; break;
        case Float: f = other->f; break;
        case String: str = other->str; break;
        case Measure: measurement = other->measurement; break;
        case Reference: str = other->str; break;
        case Array: array = other->array; break;
        case Compound: compound = other->compound; break;
    }
}
