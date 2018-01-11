//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


typedef uint32_t COLOUR;

class StyleValue {
public:
    enum Type {
        String,
        Double,
        Int,
        StyleMap,
        Reference
    } type;
    enum Unit {
        None,
        DP,
        SP
    } unit;
    //union  { god i fucking detest C++ sometimes. CANNOT get this simple thing to work! Should be trivial in C++11.
    string str;
    float d;
    int i;
    ObjPtr<class StyleMap> styleMap;
    //  Variant& operator=(const string& s) { new(&str) string(s); return *this; }
    //} var;
    StyleValue();
    StyleValue(const StyleValue&);
    ~StyleValue();
    StyleValue& operator=(const StyleValue& other);
    
    float getAsFloat();
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
    bool parse(Utf8Iterator& it);
    
};


typedef vector<pair<string,StyleValue*>> StyleValueList;



class Styles {
public:
    static void loadAsset(const string& assetPath);
    static string defaultFontName();
    static float defaultFontSize();
    static class Font* getFont(const string& key);
    static string getString(const string& key);
    static string getLocalizedString(const string& key);
    static float getFloat(const string& key);
    static COLOUR getColour(const string& key);
    
    static class View* layoutInflate(const string& assetPath);
};




