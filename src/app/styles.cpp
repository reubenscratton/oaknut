//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"

/**

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



void StyleValueUber::setValue(const string& qual, StyleValue* value) {
    StyleValue* existingValue = _defaultValue;
    if (qual.length() > 0) {
        existingValue = _qualifiedValues[qual];
    }
    
    // Assigning a map where one already exists must not *replace*, we must merge the new map into the old
    if (existingValue && value->type == StyleValue::Type::StyleMap) {
        for (auto jt : value->styleMap->_values) {
            existingValue->styleMap->_values[jt.first] = jt.second;
        }
    } else {
        if (qual.length() > 0) {
            _qualifiedValues[qual] = value;
        } else {
            _defaultValue = value;
        }
    }

}



static StyleMap s_root;

string Styles::getLocalizedString(const string& key) {
    return getString(key);
}

string Styles::defaultFontName() {
    return getString("font-name");
}
float Styles::defaultFontSize() {
    return getFloat("font-size");
}

static map<string, Font*> s_loadedFonts;

Font* Styles::getFont(const string &key) {
    string fontName = getString(key + ".font-name");
    float fontSize = getFloat(key + ".font-size");
    char ach[260];
    sprintf(ach, "%f-%s", fontSize, fontName.data());
    string fkey(ach);
    auto it = s_loadedFonts.find(fkey);
    if (it != s_loadedFonts.end()) {
        return it->second;
    }
    Font* font = oakFontGet(fontName, fontSize);
    s_loadedFonts[fkey] = font;
    return font;
}

string Styles::getString(const string& keypath) {
    StyleValue* value = s_root.getValue(keypath);
    if (!value) {
        oakLog("Warning: missing string style info '%s'", keypath.data());
        return "";
    }
    if (value->type==StyleValue::Type::String) return value->str;
    else if (value->type==StyleValue::Type::Int) return stringFromInt(value->i);
    else if (value->type==StyleValue::Type::Double) return stringFromDouble(value->d);
    return "";
}

float Styles::getFloat(const string& keypath) {
    StyleValue* value = s_root.getValue(keypath);
    if (!value) {
        oakLog("Warning: missing float style info '%s'", keypath.data());
        return 0;
    }
    return value->getAsFloat();
}
COLOUR Styles::getColour(const string& key) {
    StyleValue* value = s_root.getValue(key);
    if (!value) {
        oakLog("Warning: missing colour style info '%s'", key.data());
        return 0;
    }
    if (value->type==StyleValue::Type::Int) return value->i;
    return 0;
}

StyleValue* StyleValueUber::select() {
    StyleValue* val = _defaultValue;
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
            oakLog("Warning: unsupported qualifier '%s'", qual.data());
        }
        if (applies) {
            // TODO: apply precedence that favours higher specificity
            val = it.second;
        }
    }
    while (val && val->type == StyleValue::Type::Reference) {
        val = s_root.getValue(val->str);
    }
    return val;
}

StyleValue* StyleMap::getValue(const string& keypath) {
    int dotIndex = stringIndexOfChar(keypath, L'.');
    if (dotIndex>0) {
        string key = stringSubstring(keypath, 0, dotIndex);
        string subkey = stringSubstring(keypath, dotIndex+1);
        StyleValueUber* uberval = _values[key];
        if (uberval) {
            StyleValue* val = uberval->select();
            if (!val) {
                return NULL;
            }
            if (val->type != StyleValue::Type::StyleMap) {
                oakLog("Error: map was expected for '%s'", key.data());
                return NULL;
            }
            return val->styleMap->getValue(subkey);
        }
    } else {
        // Keypath has no dots so is just a value name
        StyleValueUber* uberval = _values[keypath];
        if (uberval) {
            StyleValue* val = uberval->select();
            if (val) {
                return val;
            }
        }
        if (_parent) {
            return _parent->getValue(keypath);
        }
    }
    return NULL;
}

void Styles::loadAsset(const string& assetPath) {
    ObjPtr<Data> data = oakLoadAsset(assetPath.data());
    if (!data) {
        return;
    }
    Utf8Iterator it(data);
    s_root.parse(it);
}


bool StyleMap::parse(Utf8Iterator& it) {
    it.skipWhitespace();
    if ('{' != it.next()) {
        oakLog("Error: expected '{'");
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
            oakLog("Error: expected a field name");
            return false;
        }
        
        // Comment line
        if (fieldName == "#") {
            it.nextToEndOfLine();
            continue;
        }
        
        // Split out the qualifier suffix, if there is one
        string fieldNameQualifier = "";
        int qualifierStartsAt = stringIndexOfChar(fieldName, '@');
        if (qualifierStartsAt > 0) {
            fieldNameQualifier = stringSubstring(fieldName, qualifierStartsAt+1);
            fieldName = stringSubstring(fieldName, 0, qualifierStartsAt);
        }
        
        it.skipWhitespace();
        if (it.next() != ':') {
            oakLog("Error: expected \':\' after identifier \'%s\'", fieldName.data());
            return false;
        }
        it.skipWhitespace();
        
        StyleValue* value = new StyleValue();
        if (it.peek() == '{') {
            value->type = StyleValue::Type::StyleMap;
            value->styleMap = new StyleMap();
            if (!value->styleMap->parse(it)) {
                return false;
            }
            value->styleMap->_parent = this;
        } else {
            string str = it.nextToEndOfLine();
            if (str.length() == 0) {
                oakLog("Error: expected a value");
                return false;
            }
            char ch = *str.begin();
            if (ch>='0' && ch<='9') {
                bool isFloat = stringContainsChar(str, '.');
                if (stringEndsWith(str, "dp", true)) {
                    value->unit = StyleValue::Unit::DP;
                    isFloat = true;
                }
                if (isFloat) {
                    value->type = StyleValue::Type::Double;
                    value->d = stringParseDouble(str);
                } else {
                    value->type = StyleValue::Type::Int;
                    value->i = stringParseInt(str);
                }
            } else {
                if (stringStartsWith(str, "\"", true)) {
                    Utf8Iterator j(str);
                    string unesc_str = "";
                    while (char32_t ch = j.next()) {
                        if (ch=='\"') {
                            break;
                        }
                        if (ch=='\\') {
                            char32_t peekch = j.peek();
                            switch (peekch) {
                                case 'n': ch='\n'; j.next(); break;
                                case 'r': ch='\r'; j.next(); break;
                                case 't': ch='\t'; j.next(); break;
                                case 'u': assert(0); // todo! implement unicode escapes
                            }
                        }
                        stringAppendCodepoint(unesc_str, ch);
                    }
                    str = unesc_str;
                }
                
                if (stringStartsWith(str, "$", true)) {
                    value->type = StyleValue::Type::Reference;
                } else {
                    value->type = StyleValue::Type::String;
                }
                value->str = str;
            }
        }


        StyleValueUber* mapval = _values[fieldName];
        if (!mapval) {
            mapval = new StyleValueUber();
            mapval->setValue(fieldNameQualifier, value);
            _values[fieldName] = mapval;
            _valuesList.push_back(make_pair(fieldName, mapval));
        } else {
            mapval->setValue(fieldNameQualifier, value);
        
            StyleValueUber* listval = new StyleValueUber();
            listval->setValue(fieldNameQualifier, value);
            _valuesList.push_back(make_pair(fieldName, listval));
        }
        
        
        
    }
    return true;
}

StyleValue::StyleValue() {
    //new(&str) string();
    //new(&styleMap) ObjPtr();
}
StyleValue::StyleValue(const StyleValue& rval) : type(rval.type) {
	switch (rval.type) {
	case String: str = rval.str; break;
    case Reference: str = rval.str; break;
	case StyleMap: styleMap = rval.styleMap; break;
	case Int: i = rval.i; break;
	case Double: d = rval.d; break;
	}
}
StyleValue& StyleValue::operator=(const StyleValue& other) {
	if (this != &other)  {
		type = other.type;
		switch (other.type) {
		case String: str = other.str; break;
        case Reference: str = other.str; break;
		case StyleMap: styleMap = other.styleMap; break;
		case Int: i = other.i; break;
		case Double: d = other.d; break;
		}
	}
	return *this;
}
float StyleValue::getAsFloat() {
    float f = 0;
    if (type==Type::Double) f= d;
    else if (type==Type::Int) f= i;
    else assert(0);
    if (unit==StyleValue::DP) {
        f = dp(f);
    }
    return f;
}

static View* inflateFromResource(pair<string, StyleValue*> r, View* parent) {
    string viewClassName = r.first;
    View* view = DYNCREATE(viewClassName);
    if (parent) {
        parent->addSubview(view);
    }
    
    // Extract the attributes from the uber
    StyleValue* props = r.second;
    assert(props->type == StyleValue::Type::StyleMap);
    vector<pair<string, StyleValue*>> attribs;
    vector<pair<string, StyleValue*>> subviews;
    for (auto i : props->styleMap->_valuesList)  {
        StyleValue* val = i.second->select();
        if (val) {
            auto a = make_pair(i.first, val);
            if (val->type == StyleValue::Type::StyleMap && i.first != "style") {
                subviews.push_back(a);
            } else {
                attribs.push_back(a);
            }
        }
    }
    
    // Apply the attributes to the inflated view
    view->applyStyleValues(attribs);
    
    // Inflate the subviews
    for (auto j : subviews) {
        inflateFromResource(j, view);
    }
    
    return view;
}

View* Styles::layoutInflate(const string& assetPath) {
    ObjPtr<Data> data = oakLoadAsset(assetPath.data());
    if (!data) {
        return NULL;
    }
    Utf8Iterator it(data);
    StyleMap layoutRoot;
    bool parsed = layoutRoot.parse(it);
    assert(parsed);
    assert(layoutRoot._values.size()==1);
    auto i = layoutRoot._values.begin();
    return inflateFromResource(make_pair(i->first, i->second->select()), NULL);
}



