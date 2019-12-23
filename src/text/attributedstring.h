//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


struct PARAGRAPH_METRICS {
    EDGEINSETS insets;
    int32_t hangingIndentChars;
};

/**
 String with display attributes, color and size and so on. Whereas the string's text buffer is shared, the attribute buffer is *not* shared between
 instances and care should be taken to avoid unnessary copying.
 */
class attributed_string : public string {
public:
    

    // Construction
    attributed_string() : string() {}
    template<size_t N>
    attributed_string(const char(&p)[N]) : string(p) {}
    attributed_string(const string& str) : string(str) {}
    attributed_string(const attributed_string& str);
    

    struct FONT_CHANGE {
        sp<oak::Font> font;  // null = inherit, if non-null then other members must be null
        float sizeMul;  // 0 = use abs, or inherit if abs also 0
        float sizeAbs;  // 0 = use mul, or inherit if mul also 0
        float weight;   // 0 = inherit
        int italic;     // 0 = inherit
        
        FONT_CHANGE(oak::Font* afont=nullptr, float asizeMul=0, float asizeAbs=0, float aweight=0, int aitalic=0) : font(afont), sizeMul(asizeMul), sizeAbs(asizeAbs), weight(aweight), italic(aitalic) {}
        FONT_CHANGE(const FONT_CHANGE& src) : font(src.font), sizeMul(src.sizeMul), sizeAbs(src.sizeAbs), weight(src.weight), italic(src.italic) {}
    };
    
    class attribute {
    public:
        enum type {
            Forecolor,
            Backcolor,
            Font,           // NB: unlike the others, font attributes are cumulative
            ParagraphMetrics,
            BaselineOffset,
            Underline
        } _type;

        union {
            COLOR _color;
            FONT_CHANGE _font;
            PARAGRAPH_METRICS _paragraphMetrics;
            float _floatVal;
            bool _underline;
        };
        
        attribute() : _type(Forecolor) {}
        attribute(type atype, float floatVal) : _type(atype), _floatVal(floatVal) {}
        attribute(type atype, COLOR color) : _type(atype), _color(color) {}
        attribute(const FONT_CHANGE& fontChange) : _type(Font), _font(fontChange) {}
        attribute(const PARAGRAPH_METRICS& metrics) : _type(ParagraphMetrics), _paragraphMetrics(metrics) {}
    
        ~attribute() { if (_type == Font) { _font.font.~sp(); } }
        attribute(const attribute& attr) : _type(Forecolor) {
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
        void setType(type newType) {
            if (_type == newType) return;
            if (_type == Font && newType != Font) {
                _font.font.~sp();
            } else if (_type != Font && newType == Font) {
                new (&_font.font._obj) sp<class Font>();
            }
            _type = newType;
        }
        void assign(const attribute& src) {
            setType(src._type);
            switch (src._type) {
                case Forecolor: _color = src._color; break;
                case Backcolor: _color = src._color; break;
                case Font: _font = src._font; break;
                case ParagraphMetrics: _paragraphMetrics = src._paragraphMetrics; break;
                case BaselineOffset: _floatVal = src._floatVal; break;
                case Underline: break;
            }
        }
    };

    // Helpers for creating standard attributes
    static attribute font(oak::Font* font);
    static attribute font_size(float mul, float add);
    static attribute font_weight(float weight);
    static attribute bold();
    static attribute italic();
    static attribute forecolor(COLOR color);
    static attribute backcolor(COLOR color);
    static attribute paragraphMetrics(const PARAGRAPH_METRICS& metrics);
    static attribute baselineOffset(float offset);
    static attribute underline();

    
    void setAttribute(const attribute& attribute, int32_t start, int32_t end);
    const attribute* getAttribute(int32_t pos, attribute::type type);

    inline void clear() {
        string::clear();
        clearAttributes();
    }
    void clearAttributes();
    attributed_string& operator=(const attributed_string& str);

    void applyStyle(const string& s);
    void applyStyle(const class style* s);
    
    void append(const attributed_string& str);


    friend class TextLayout;

private:
    struct attribute_usage : public attribute {
        int32_t start;
        int32_t end;
        
        attribute_usage(const attribute& a, int32_t astart, int32_t aend) : attribute(a), start(astart), end(aend) {
        }
    };

    struct cmp_start {
        bool operator() (const attribute_usage& a, const attribute_usage& b) const;
    };
    set<attribute_usage, cmp_start> _attributes;


    class enumerator {
    public:
        enumerator(attribute::type type, const attributed_string& str, void* defaultVal);
        void advanceTo(int32_t index);
        void enumerate(int32_t from, int32_t to, std::function<void(const attribute_usage& attr)> callback);
        void* current();
        
        const attributed_string& _str;
        int32_t _strLength;
        attribute::type _type;
        set<attribute_usage>::iterator _it;
        void* _defaultVal;
        list<const attribute_usage*> _activeList;
        bool _activeListChanged;
        
        // Cumulative calc: should be in a specialization or subclass
        sp<class Font> _currentFont;
    };
    
};


