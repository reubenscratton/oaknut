//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"

// TODO: Leverage built-in JSON for web builds.

string parseJsonString(Utf8Iterator& it, int flags) {
	string str;
	bool allowLite = (flags & FLAG_ALLOW_JSONLITE);
	char32_t ch;
	if (!allowLite) {
		ch = it.next();
		if (ch != '\"') {
			oakLog("Expected: \"");
			return str;
		}
	}
		for (;;) {
			const char* charStart = it._p;
			char32_t ch = it.next();
			if (ch==0) {
				break;
			}
			if (!allowLite && ch=='\"') {
				break;
			}
			if (allowLite) {
				if (ch == ',' || ch==':' || ch=='}') { // in jsonlite mode these are delimiters
					it._p--;
					break;
				}
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
					oakLog("Warning: illegal escape \'\\%c\'", ch);
					continue;
				}
			}
			// Generic UTF8 character
			str.append(charStart, it._p-charStart);
		}
	
	return str;
}

JsonValue::JsonValue() : _type(jsonInvalid) {
}
JsonValue::JsonValue(const JsonValue& src) {
	*this = src;
}
JsonValue::JsonValue(JsonObject* obj) : _type(jsonObject), _objVal(obj) {
}
JsonValue::JsonValue(JsonArray* arr) : _type(jsonArray), _arrayVal(arr) {
}
JsonValue::JsonValue(const string& str) : _type(jsonString), _stringVal(str) {
}
JsonValue::~JsonValue() {
}

JsonValue& JsonValue::operator=(const JsonValue& other) {
	if (this != &other)  {
		_type = other._type;
		switch (_type) {
			case jsonString: _stringVal = other._stringVal; break;
			case jsonInt: _intVal = other._intVal; break;
			case jsonFloat: _floatVal = other._floatVal; break;
			case jsonObject: _objVal = other._objVal; break;
			case jsonArray: _arrayVal = other._arrayVal; break;
			default: break;
		}
	}
	return *this;
}

// I cannot get strtol or stoi to work on Emscripten... they return garbage. Hence
// rolling my own...
void parseNumber(Utf8Iterator& it, JsonValue* val) {
	val->_type = jsonInt;
	val->_intVal = 0;
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
				val->_intVal = val->_intVal*base + digit;
			} else {
				fraction/=base;
				val->_floatVal += digit*fraction;
			}
			it.next();
		} else if (ch=='.') {
			fraction = 1;
			val->_type = jsonFloat;
			val->_floatVal = val->_intVal;
			it.next();
		}
		
		// Dimension values dp, px
		/*else if (ch=='d') {
			it.next();
			if (it.peek() == 'p') {
				it.next();				
				if (val->_type == jsonInt) {
					val->_type = jsonFloat;
					val->_floatVal = val->_intVal;
				}
				val->_floatVal = dp(val->_floatVal);
				break;
			}
		}*/
		else {
			break;
		}
	}
	if (neg) {
		if (val->_type==jsonInt) val->_intVal=-val->_intVal;
		if (val->_type==jsonFloat) val->_floatVal=-val->_floatVal;
	}
}

JsonValue JsonValue::parse(Utf8Iterator& it, int flags) {
	JsonValue val;
	it.skipWhitespace();
	char32_t ch = it.peek();
	if (ch == '{') {
		it.next();
		val._type = jsonObject;
		val._objVal = new JsonObject(it, flags);
	}
	else if (ch == '[') {
		it.next();
		val._type = jsonArray;
		val._arrayVal = new JsonArray(it, flags);
	}
	else if ((ch>='0' && ch<='9') || ch=='-' || ch=='.') {
		parseNumber(it, &val);
	}
	else if (ch == '\"' || (flags & FLAG_ALLOW_JSONLITE)) {
		val._type = jsonString;
		val._stringVal = parseJsonString(it, flags);
	}
	return val;
}


JsonObject::JsonObject() {
}
JsonObject::JsonObject(Utf8Iterator &it, int flags) {
	it.skipWhitespace();
	while (it.peek() != '}') {
		it.skipWhitespace();
		string fieldName = parseJsonString(it, flags);
		if (!fieldName.length()) {
			oakLog("Invalid json: field name expected");
			return;
		}
		it.skipWhitespace();
		if (it.next() != ':') {
			oakLog("Invalid json: ':' expected");
			return;
		}
		JsonValue fieldValue = JsonValue::parse(it, flags);
		if (fieldValue._type == jsonInvalid) {
			oakLog("Invalid json: value expected");
			return;
		}
		_fields[fieldName] = fieldValue;
		it.skipWhitespace();
		char32_t sep = it.peek();
		if (sep == ',') {
			it.next();
		} else {
			if (sep != '}') {
				oakLog("Invalid json: ',' or object end expected");
				break;
			}
			it.next();
			break;
		}
	}
}
JsonObject::JsonObject(const JsonObject* json) {
	_fields = json->_fields;
}

string JsonObject::getString(const char* name) {
	JsonValue val = _fields[name];
	if (val._type == jsonString) {
		return val._stringVal;
	}
	return "";
}

void JsonObject::putValue(const char *name, const JsonValue &val) {
	_fields[name] = val;
}

JsonArray* JsonObject::getArray(const char* name) {
	JsonValue& val = _fields[name];
	if (val._type == jsonArray) {
		return val._arrayVal;
	}
	return NULL;
}

vector<string> JsonObject::getStringArray(const char* name) {
	JsonArray* jarray = getArray(name);
	vector<string> rv;
	if (jarray) {
		for (int i=0 ; i<jarray->_elements.size() ; i++) {
			JsonValue& elem = jarray->_elements.at(i);
			if (elem._type == jsonString) {
				rv.push_back(elem._stringVal);
			}
		}
	}
	return rv;
}

float JsonObject::getFloat(const char* name) {
	return 0; // todo
}

JsonArray::JsonArray() {
}
JsonArray::JsonArray(Utf8Iterator& it, int flags) {
	it.skipWhitespace();
	while (it.peek() != ']') {
		JsonValue element = JsonValue::parse(it, flags);
		_elements.push_back(element);
		it.skipWhitespace();
		char32_t sep = it.peek();
		if (sep == ',') {
			it.next();
		} else {
			if (sep != ']') {
				oakLog("Invalid json: ',' or array end expected");
				break;
			}
			it.next();
			break;
		}
	}
}

void JsonArray::addValue(const JsonValue &val) {
	_elements.push_back(val);
}

