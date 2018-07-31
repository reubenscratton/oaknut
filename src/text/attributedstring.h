//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class Attribute {
public:
    typedef enum {
        Forecolour,
        BackgroundColour,
        Font
    } Type;
    Type _type;
    union {
        COLOUR _colour;
        ObjPtr<class Font> _font;
    };
    
    Attribute(Type type, COLOUR colour) : _type(type), _colour(colour) {}
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
            case Forecolour: _colour = src._colour; break;
            case BackgroundColour: _colour = src._colour; break;
            case Font: _font = src._font; break;
        }
    }
    static Attribute forecolour(COLOUR colour) { return Attribute(Forecolour, colour); }
    
};


class AttributedString : public Object, public string {
public:
    
    AttributedString();
    AttributedString(const string& str);
    AttributedString(const AttributedString& str);
    
    void setAttribute(const Attribute& attribute, int start, int end);
    
    friend class TextRenderer;
    
private:
    struct AttributeUse {
        Attribute attribute;
        int start;
        int end;
        bool operator<(const AttributeUse& rhs) const {
            return start<rhs.start;
        }
        AttributeUse(const Attribute& aattribute, int start, int end)  : attribute(aattribute) {
            this->start = start;
            this->end = end;
        }
    };
    set<AttributeUse> _attributes;
};


