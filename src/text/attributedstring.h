//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


struct mul_add {
    float mul;
    float add;
};

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
    
    typedef enum {
        Forecolor,
        BackgroundColor,
        Font,
        FontWeight,
        FontSize,
        ParagraphMetrics,
        BaselineOffset,
        Underline
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
            struct mul_add _mul_add;
            struct PARAGRAPH_METRICS _paragraph_metrics;
        };
        
        attribute(attribute_type type, COLOR color) : _type(type), _color(color) {}
        attribute(attribute_type type, float f) : _type(type), _f(f) {}
        attribute(oak::Font* font) : _type(Font), _font(font) {}
        attribute(attribute_type type, const mul_add& m) : _type(type), _mul_add(m) {}
        attribute(const PARAGRAPH_METRICS& metrics) : _type(ParagraphMetrics), _paragraph_metrics(metrics) {}
    
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
                case Forecolor:
                case BackgroundColor: _color = src._color; break;
                case Font: _font = src._font; break;
                case FontWeight: _f = src._f; break;
                case FontSize: _mul_add = src._mul_add; break;
                case ParagraphMetrics: _paragraph_metrics = src._paragraph_metrics; break;
                case BaselineOffset: _f=src._f; break;
                case Underline: break;
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
    static attribute fontsize(const mul_add& ma) { return attribute(FontSize, ma); }
    static attribute paragraphMetrics(const PARAGRAPH_METRICS& metrics) { return attribute(metrics); }
    static attribute baselineOffset(float offset) { return attribute(BaselineOffset, offset); }
    static attribute underline() { return attribute(Underline, 0.f); }

    friend class TextLayout;

private:
    struct attribute_usage : public attribute {
        int32_t start;
        int32_t end;
        
        attribute_usage(const attribute& a, int32_t astart, int32_t aend) : attribute(a), start(astart), end(aend) {
        }
    };

    struct cmp_start {
        bool operator() (const attribute_usage& a, const attribute_usage& b) const {
            uint32_t aa = *(uint32_t*)&a.start;
            uint32_t bb = *(uint32_t*)&b.start;
            if (aa == bb) {
                return (char*)&a < (char*)&b;
            }
            return aa < bb;
        }
    };
    set<attribute_usage, cmp_start> _attributes;

    
    template<attribute_type TYPE, class T>
    class enumerator {
    public:
        enumerator(const attributed_string& str, T* defaultVal) : _str(str), _it(str._attributes.begin()) {
            _strLength = str.length();
            _stack.push(std::make_pair(_strLength+1, defaultVal));
        }
        
        void advanceTo(int32_t index) {
            while (_stack.top().first <= index) {
                _stack.pop();
            }
            while (_it != _str._attributes.end()) {
                int32_t start = _it->start;
                if (start<0) start += _strLength;
                if (start > index) {
                    break;
                }
                int32_t end = _it->end;
                if (end<0) end += _strLength;
                if (end > index) {
                    handleApplyingAttrib(*_it, end);
                }
                _it++;
            }
        }
        inline void handleApplyingAttrib(const attribute_usage& a, int32_t end) {
            if (a._type == TYPE) {
                _stack.push(std::make_pair(end, (T*)&a._color));
            }
        }
        void enumerate(int32_t from, int32_t to, std::function<void(const attribute_usage& attr)> callback) {
            while (_it != _str._attributes.end() && from<to) {
                if (_it->_type != TYPE) {
                    _it++;
                    continue;
                }
                
                // Handle -ve char indexes
                int32_t end = _it->end;
                if (end<0) end += _strLength;
                int32_t start = _it->start;
                if (start<0) start += _strLength;
                
                // If span ended before the range of interest, continue
                if (end<=from) {
                    _it++;
                    continue;
                }
                
                // If span starts beyond the range of interest, break
                if (start>=to) {
                    break;
                }
                
                start = MAX(start, from);
                end = MIN(end, to);
                callback(*_it);
                
                //_it++;
                from=end+1;
            }

        }
        
        T* current() {
            return _stack.top().second;
        }
        
        const attributed_string& _str;
        int32_t _strLength;
        set<attribute_usage>::iterator _it;
        stack<pair<int32_t, T*>> _stack;
    };
    
};


