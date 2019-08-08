//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


/**
 Style work needed:
 
 - Replace StyleValue with a Style type that contains a map of strings to variants and a pointer to its parent.
 - Restore the cascadeability, e.g. looking up a style entry will look to parent style if it doesnt have it.
 -
 
 */


class style {
public:
    style* _parent;
    enum type {
        TypeSimple,
        TypeReference,
        TypeArray,
        TypeCompound,
        TypeQual
    } type;
    union {
        variant var;                   // value is simple
        const style* reference;
        vector<style>* array;
        map<string,style>* compound;   // value is compound (or qualified)
    };
    style();
    style(const style&);
    style(style&& rval) noexcept;
    ~style();
    
    style& operator=(const style& other);
    
    // Type testing
    bool isEmpty() const;
    bool isNumeric() const;
    bool isFloatingPoint() const;
    bool isString() const;
    bool isMeasurement() const;
    bool isArray() const;
    bool isCompound() const;
        
    // Accessors
    const variant& variantVal() const;
    string stringVal() const;
    int intVal() const;
    float floatVal() const;
    bool boolVal() const;
    measurement measurementVal() const;
    EDGEINSETS edgeInsetsVal() const;
    COLOR colorVal() const;
    VECTOR4 cornerRadiiVal() const;
    float fontWeightVal() const;
    const vector<style>& arrayVal() const;

    // Compound accessors
    string stringVal(const char* name) const;
    COLOR colorVal(const char* name) const;
    const vector<style>& arrayVal(const char* name) const;
    EDGEINSETS edgeInsetsVal(const char* name) const;

    bool parse(class StringProcessor& it);
    void importNamedValues(const map<string,style>& styleValues);
    void fromVariant(const variant& v);

protected:
    const style* resolve() const;
    void copyFrom(const style* other);
    void setType(enum type newType);
    const style* get(const string& name) const;
    friend class App;
    friend class Styleable;
};

/**
 * @ingroup app_group
 * @brief Represents a value in the global style namespace. Typical usage is to
 * use `App::getStyleValue()` and friends to get one and then use one of the accessors
 * `intVal()`, `stringVal()` to get the raw value.
 */



/**
 * @ingroup app_group
 * @brief  Abstract base class for objects that can have style
 * values applied.
 *
 * In practice this means Views.
 */
class Styleable : public Object {
public:
    virtual void applyStyle(const string& styleName);
    virtual void applyStyle(const style& style);
    
protected:
    /** Applies a single resolved style for the given attribute name. Custom views
     should override this method to add support for custom attributes. */
    virtual bool applySingleStyle(const string& name, const style& style) = 0;
};

