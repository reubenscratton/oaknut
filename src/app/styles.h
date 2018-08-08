//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class Measurement {
public:
    float val() const;
    enum {
        PX,
        DP,
        SP
    };
    Measurement(float v, int unit) : _val(v), _unit(unit) {}
private:
    float _val;
    int _unit;
};

class StyleValue {
public:
    enum Type {
        Empty,
        Int,
        Float,
        String,
        Measure,
        Reference, // value is the name of another StyleValue, defined elsewhere
        Array, // value is an array of two or more values
        Compound // value is a set of key-value pairs
    } type;

    StyleValue();
    StyleValue(const StyleValue&);
    ~StyleValue();
    StyleValue& operator=(const StyleValue& other);
    
    // Accessors. Use these instead of accessing the private data to benefit from some implicit conversions.
    bool isEmpty() const;
    int intVal() const;
    float floatVal() const;
    string stringVal() const;
    COLOR colorVal() const;
    const vector<const StyleValue*> arrayVal() const;
    class StyleMap* compoundVal() const;
    
    bool isNumeric() const;

private:
    void setType(Type newType);
    void assign(const StyleValue* other);
    union {
        int i;
        float f;
        string str;
        Measurement measurement;
        vector<const StyleValue*> array;
        ObjPtr<StyleMap> compound;
    };

    void setQualifiedValue(const string& qual, const StyleValue* value);
    map<string, const StyleValue*> _qualifiedValues;
    const StyleValue* select() const;

    friend class StyleMap;
};


class StyleMap : public Object {
public:
    map<string, StyleValue*> _values;
    vector<pair<string,StyleValue*>> _valuesList;
    StyleMap* _parent;
    StyleValue* getValue(const string& keypath);
    bool parse(class StringProcessor& it);
    void parseSingleValue(StyleValue* value, string& str);
};





