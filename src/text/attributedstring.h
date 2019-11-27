//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//



/**
 String with display attributes, color and size and so on. Whereas the string's text buffer is shared, the attribute buffer is *not* shared between
 instances and care should be taken to avoid unnessary copying.
 */
class attributed_string : public string {
public:
    
    typedef enum {
        Forecolor,
        BackgroundColor,
        Font,
        LeadingSpace,
        FontWeight
    } attribute_type;

    // Construction
    attributed_string() : string() {}
    template<size_t N>
    attributed_string(const char(&p)[N]) : string(p) {}
    attributed_string(const string& str) : string(str) {}
    attributed_string(const attributed_string& str);
    
    
    class attribute {
    public:
        attribute_type _type;
        union {
            COLOR _color;
            sp<class Font> _font;
            float _f;
        };
        
        attribute(attribute_type type, COLOR color) : _type(type), _color(color) {}
        attribute(attribute_type type, float f) : _type(type), _f(f) {}
        attribute(oak::Font* font) : _type(Font), _font(font) {}
        ~attribute() { if (_type == Font) { _font.~sp(); } }
        attribute(const attribute& attr) : _type(attr._type) {
            assign(attr);
        }
        attribute& operator=(const attribute& rhs) {
            assign(rhs);
            return *this;
        }
        bool operator<(const attribute& rhs) const {
            if (_type<rhs._type) return true;
            return false;
        }
        void setType(attribute_type newType) {
            if (_type == newType) return;
            if (_type == Font && newType != Font) {
                _font.~sp();
            } else if (_type != Font && newType == Font) {
                new (&_font) sp<class Font>();
            }
            _type = newType;
        }
        void assign(const attribute& src) {
            setType(src._type);
            switch (src._type) {
                case Forecolor: _color = src._color; break;
                case BackgroundColor: _color = src._color; break;
                case Font: _font = src._font; break;
                case LeadingSpace: _f = src._f; break;
                case FontWeight: _f = src._f; break;
            }
        }
    };

    void setAttribute(const attribute& attribute, int32_t start, int32_t end);
    const attribute* getAttribute(int32_t pos, attribute_type type);

    inline void clear() {
        string::clear();
        clearAttributes();
    }
    void clearAttributes();
    attributed_string& operator=(const attributed_string& str);

    void applyStyle(const string& s);
    void applyStyle(const class style* s);
    
    void append(const attributed_string& str);

    // Helpers for standard attributes
    static attribute font(oak::Font* font) { return attribute(font); }
    static attribute font_weight(float weight) { return attribute(FontWeight, weight); }
    static attribute bold() { return attribute(FontWeight, FONT_WEIGHT_BOLD); }
    static attribute forecolor(COLOR color) { return attribute(Forecolor, color); }
    static attribute leadingSpace(float space) { return attribute(LeadingSpace, space); }

    friend class TextLayout;

private:
    struct attribute_usage : public attribute {
        int32_t start;
        int32_t end;
        
        attribute_usage(const attribute& a, int32_t astart, int32_t aend) : attribute(a), start(astart), end(aend) {
        }
    };

    list<attribute_usage> _attributes;

    struct cmp_start {
        bool operator() (const list<attribute_usage>::iterator& a, const list<attribute_usage>::iterator& b) const {
            uint32_t aa = *(uint32_t*)&a->start;
            uint32_t bb = *(uint32_t*)&b->start;
            if (aa == bb) {
                return (char*)&(*a) < (char*)&(*b);
            }
            return aa < bb;
        }
    };
    struct cmp_end {
        bool operator() (const list<attribute_usage>::iterator& a, const list<attribute_usage>::iterator& b) const {
            uint32_t aa = *(uint32_t*)&a->end;
            uint32_t bb = *(uint32_t*)&b->end;
            if (aa == bb) {
                return (char*)&(*a) < (char*)&(*b);
            }
            return aa < bb;
        }
    };
    set<list<attribute_usage>::iterator, cmp_start> _starts;
    set<list<attribute_usage>::iterator, cmp_end> _ends;
};


