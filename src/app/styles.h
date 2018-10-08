//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class measurement {
public:
    float val() const;
    enum {
        PX,
        DP,
        SP,
        PC
    };
    measurement(float v, int unit) : _val(v), _unit(unit) {}
    int _unit;
private:
    float _val;
};

class StyleValue {
public:
    enum Type {
        Empty,
        Int,
        Float,
        String,
        Array, // value is an array of two or more values
        Compound, // value is a set of key-value pairs
        // Additional style-specific types
        Measure,
        Reference, // value is the name of another StyleValue, defined elsewhere
        QualifiedCompound // value is compound where they keys are qualifiers. Zero-length key maps to default value.
    } type;

    StyleValue();
    StyleValue(const StyleValue&);
    StyleValue(StyleValue&&) noexcept;
    ~StyleValue();
    StyleValue& operator=(const StyleValue& other);

    bool isEmpty() const;
    bool isNumeric() const;
    bool isString() const;
    bool isMeasurement() const;
    bool isArray() const;

    // Accessors. Use these instead of accessing the private data to benefit from some implicit conversions.
    int intVal() const;
    bool boolVal() const;
    float floatVal() const;
    string stringVal() const;
    measurement measurementVal() const;
    COLOR colorVal() const;
    const vector<StyleValue>& arrayVal() const;
    const map<string, StyleValue>& compoundVal() const;
    Vector4 cornerRadiiVal() const;
    EDGEINSETS edgeInsetsVal() const;
    float fontWeightVal() const;

    // Compound accessors
    const StyleValue* get(const string& keypath) const;
    int intVal(const string& name) const;
    float floatVal(const string& name) const;
    string stringVal(const string& name) const;
    const vector<StyleValue>& arrayVal(const string& name) const;
    void importValues(const map<string, StyleValue>& values);

#define PARSEFLAG_IN_ARRAY 1
#define PARSEFLAG_IS_ARGUMENT 2

    bool parse(class StringProcessor& it, int flags=0);

private:
    void setType(Type newType);
    void copyFrom(const StyleValue* other);
    union {
        int i;
        float f;
        string str; // includes refs
        measurement _measurement;
        vector<StyleValue>* array;
        map<string, StyleValue>* compound;
    };

    const StyleValue* select() const;

    bool parseNumberOrMeasurement(StringProcessor& it);
};

/**
 Abstract base class for objects that can have style values applied.
 In practice this means Views, but also TextRenderer.
 */
class Styleable : public Object {
public:
    virtual void applyStyle(const string& style);
    virtual void applyStyle(const class StyleValue& value);
    
protected:
    /** Applies a single style value for the given attribute name. Custom views
     should override this method to add support for custom attributes. */
    virtual bool applyStyleValue(const string& name, const StyleValue* value) = 0;
};

