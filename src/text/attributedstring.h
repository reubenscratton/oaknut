//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class Attribute {
public:
    typedef enum {
        Forecolor,
        BackgroundColor,
        Font,
        LeadingSpace
    } Type;
    Type _type;
    union {
        COLOR _color;
        ObjPtr<class Font> _font;
        float _space;
    };
    
    Attribute(Type type, COLOR color) : _type(type), _color(color) {}
    Attribute(Type type, float space) : _type(type), _space(space) {}
    ~Attribute() { if (_type == Font) { _font.~ObjPtr(); } }
    Attribute(const Attribute& attr) : _type(attr._type) {
        assign(attr);
    }
    Attribute& operator=(const Attribute& rhs) {
        assign(rhs);
        return *this;
    }
    void setType(Type newType) {
        if (_type == newType) return;
        if (_type == Font && newType != Font) {
            _font.~ObjPtr();
        } else if (_type != Font && newType == Font) {
            new (&_font) ObjPtr<class Font>();
        }
        _type = newType;
    }
    void assign(const Attribute& src) {
        setType(src._type);
        switch (src._type) {
            case Forecolor: _color = src._color; break;
            case BackgroundColor: _color = src._color; break;
            case Font: _font = src._font; break;
            case LeadingSpace: _space = src._space; break;
        }
    }
    static Attribute forecolor(COLOR color) { return Attribute(Forecolor, color); }
    static Attribute leadingSpace(float space) { return Attribute(LeadingSpace,  space); }
};


class AttributedString : public string {
public:
    
    AttributedString();
    AttributedString(const char* p);
    AttributedString(const string& str);
    AttributedString(const AttributedString& str);
    
    void setAttribute(const Attribute& attribute, int start, int end);
    void clearAttributes();
    AttributedString& operator=(const AttributedString& str);

    
    friend class TextRenderer;
    
private:
    struct AttributeUse {
        Attribute attribute;
        int32_t start;
        int32_t end;
        bool operator<(const AttributeUse& rhs) const {
            return start<rhs.start;
        }
        AttributeUse(const Attribute& aattribute, int32_t start, int32_t end)  : attribute(aattribute) {
            this->start = start;
            this->end = end;
        }
    };
    set<AttributeUse> _attributes;
};


