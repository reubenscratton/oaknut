//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

typedef enum {
	jsonInvalid,
	jsonString,
	jsonInt,
	jsonFloat,
	jsonObject,
	jsonArray,
	jsonTrue,
	jsonFalse,
	jsonNull
} JsonType;

#define FLAG_ALLOW_JSONLITE 1

class JsonValue { // NB: this is a proper value type, i.e. no refcounting
public:
	JsonType _type;
	//union {
		string _stringVal;
		int _intVal;
		float _floatVal;
		ObjPtr<class JsonObject> _objVal;
		ObjPtr<class JsonArray> _arrayVal;
	//};
	
	JsonValue();
	JsonValue(const JsonValue&);
	JsonValue(JsonObject* obj);
	JsonValue(JsonArray* array);
	JsonValue(const string& str);
	~JsonValue();
	JsonValue& operator=(const JsonValue& other);
	
	static JsonValue parse(StringProcessor& it, int flags);
	
};



class JsonArray : public Object {
public:
	JsonArray();
	JsonArray(StringProcessor& it, int flags);
	template <class T>
	T* getObject(int index);
	void addValue(const JsonValue& val);
	
	vector<JsonValue> _elements;
	
};

class JsonObject : public Object {
public:
	JsonObject();
	JsonObject(StringProcessor& it, int flags);
	JsonObject(const JsonObject* json);
	void putValue(const char* name, const JsonValue& val);
	string getString(const char* name);
	JsonArray* getArray(const char* name);
	vector<string> getStringArray(const char* name);
	float getFloat(const char* name);
	
	template <class T>
	vector<ObjPtr<T>> getObjectArray(const char* name) {
		vector<ObjPtr<T>> rv;
		JsonArray* a = getArray(name);
		for (int i=0 ; i<a->_elements.size() ; i++) {
			JsonValue& elem = a->_elements.at(i);
			if (elem._type == jsonObject) {
				rv.push_back(new T(elem._objVal));
			}
		}
		return rv;
	}
	
	template <class T>
	void putObjectArray(const char* name, const vector<ObjPtr<T>>& vec) {
		JsonArray* a = new JsonArray();
		for (int i=0 ; i<vec.size() ; i++) {
			T* obj = vec.at(i)._obj;
			a->addValue(obj->toJson());
		}
		putValue(name, a);
	}

//protected:
	map<string, JsonValue> _fields;
};


template <class T>
T* JsonArray::getObject(int index) {
	JsonValue& elem = _elements.at(index);
	if (elem._type == jsonObject) {
		return new T(elem._objVal);
	}
	return nullptr;
}


