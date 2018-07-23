//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class StyleValue {
public:
    enum Type {
        Int,
        String,
        Double,
        StyleMap,
        Reference
    } type;
    enum Unit {
        None,
        DP,
        SP
    } unit;
    union {
        string str;
        float d;
        int i;
        ObjPtr<class StyleMap> styleMap;
    };
    StyleValue();
    StyleValue(const StyleValue&);
    ~StyleValue();
    StyleValue& operator=(const StyleValue& other);
    
    float getAsFloat();

    void setType(Type newType);
    
    
    static float parseDimension(string str);
};

class StyleValueUber : public Object {
public:
    StyleValue* _defaultValue;
    map<string, StyleValue*> _qualifiedValues;
    
    void setValue(const string& qual, StyleValue* value);
    StyleValue* select();
};

class StyleMap : public Object {
public:
    map<string, StyleValueUber*> _values;
    vector<pair<string,StyleValueUber*>> _valuesList;
    StyleMap* _parent;
    StyleValue* getValue(const string& keypath);
    bool parse(class StringProcessor& it);
    
};


typedef vector<pair<string,StyleValue*>> StyleValueList;





