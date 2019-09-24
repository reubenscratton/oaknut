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
        LeadingSpace,
        FontWeight
    } Type;
    Type _type;
    union {
        COLOR _color;
        sp<class Font> _font;
        float _f;
    };
    
    Attribute(Type type, COLOR color) : _type(type), _color(color) {}
    Attribute(Type type, float f) : _type(type), _f(f) {}
    Attribute(oak::Font* font) : _type(Font), _font(font) {}
    ~Attribute() { if (_type == Font) { _font.~sp(); } }
    Attribute(const Attribute& attr) : _type(attr._type) {
        assign(attr);
    }
    Attribute& operator=(const Attribute& rhs) {
        assign(rhs);
        return *this;
    }
    bool operator<(const Attribute& rhs) const {
        if (_type<rhs._type) return true;
        return false;
    }
    void setType(Type newType) {
        if (_type == newType) return;
        if (_type == Font && newType != Font) {
            _font.~sp();
        } else if (_type != Font && newType == Font) {
            new (&_font) sp<class Font>();
        }
        _type = newType;
    }
    void assign(const Attribute& src) {
        setType(src._type);
        switch (src._type) {
            case Forecolor: _color = src._color; break;
            case BackgroundColor: _color = src._color; break;
            case Font: _font = src._font; break;
            case LeadingSpace: _f = src._f; break;
            case FontWeight: _f = src._f; break;
        }
    }
    static Attribute font(oak::Font* font) { return Attribute(font); }
    static Attribute bold() { return Attribute(FontWeight, FONT_WEIGHT_BOLD); }
    static Attribute forecolor(COLOR color) { return Attribute(Forecolor, color); }
    static Attribute leadingSpace(float space) { return Attribute(LeadingSpace,  space); }
};


class AttributedString : public string { // TODO: Should extend Object, not string.
public:
    
    AttributedString();
    AttributedString(const char* p);
    AttributedString(const string& str);
    AttributedString(const AttributedString& str);
    
    void setAttribute(const Attribute& attribute, int32_t start, int32_t end);
    const Attribute* getAttribute(int32_t pos, Attribute::Type type);

    void clearAttributes();
    AttributedString& operator=(const AttributedString& str);

    void applyStyle(const class style* s);
    
    void append(const AttributedString& str);

    friend class TextLayout;

private:
    struct AttributeUse {
        Attribute attribute;
        int32_t start;
        int32_t end;
        bool operator<(const AttributeUse& rhs) const {
            if (start<rhs.start) return true;
            if (attribute<rhs.attribute) return true;
            return false;
        }
        AttributeUse(const Attribute& aattribute, int32_t start, int32_t end)  : attribute(aattribute) {
            this->start = start;
            this->end = end;
        }
    };
    set<AttributeUse> _attributes;
};


