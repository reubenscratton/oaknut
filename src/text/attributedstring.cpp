//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


bool attributed_string::cmp_start::operator() (const attribute_usage& a, const attribute_usage& b) const {
    // First order by start index (using uint32 cast to simplify things, no need to add strlength)
    uint32_t aa = *(uint32_t*)&a.start;
    uint32_t bb = *(uint32_t*)&b.start;
    if (aa != bb) {
        return aa < bb;
    }
    // Second order by end index
    aa = *(uint32_t*)&a.end;
    bb = *(uint32_t*)&b.end;
    if (aa != bb) {
        return aa < bb;
    }
    // Lastly, type
    if (a._type != b._type) {
        return a._type < b._type;
    }
    // Attributes are identical
    return false;
}

attributed_string::enumerator::enumerator(attributed_string::attribute::type type, const attributed_string& str, void* defaultVal) : _str(str), _it(str._attributes.begin()) {
    _type = type;
    _strLength = str.length();
    _defaultVal = defaultVal;
}
    
void attributed_string::enumerator::advanceTo(int32_t index) {
    for (auto e=_activeList.begin() ; e!=_activeList.end() ; e++) {
        int32_t end = (*e)->end;
        if (end<0) end += _strLength+1;
        if (end <= index) {
            e = _activeList.erase(e);
            _activeListChanged = true;
        }
    }
    while (_it != _str._attributes.end()) {
        int32_t start = _it->start;
        if (start<0) {
            start += _strLength+1;
        }
        if (start > index) {
            break;
        }
        int32_t end = _it->end;
        if (end<0) {
            end += _strLength+1;
        }
        if (end > index) {
            if (_it->_type == _type) {
                _activeList.push_back(&(*_it));
                _activeListChanged = true;
            }
        }
        _it++;
    }
}

void attributed_string::enumerator::enumerate(int32_t from, int32_t to, std::function<void(const attribute_usage& attr)> callback) {
    while (_it != _str._attributes.end() && from<to) {
        if (_it->_type != _type) {
            _it++;
            continue;
        }
        
        // Handle -ve char indexes
        int32_t end = _it->end;
        if (end<0) end += _strLength+1;
        int32_t start = _it->start;
        if (start<0) start += _strLength+1;
        
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
    
void* attributed_string::enumerator::current() {
    if (!_activeList.size()) {
        return _defaultVal;
    }
    if (_type != attribute::type::Font) {
        return (void*)&_activeList.back()->_color;
    }
    
    // Special code for Font attribs which accumulate, unlike all other attribs
    if (_activeListChanged) {
        _activeListChanged = false;
        _currentFont = (class Font*)_defaultVal;
        float size =  _currentFont->_size;
        float weight = _currentFont->_weight;
        for (auto& a : _activeList) {
            if (a->_font.font) {
                _currentFont = a->_font.font;
                size =  _currentFont->_size;
                weight = _currentFont->_weight;
            }
            if (a->_font.weight) {
                weight = a->_font.weight;
            }
            if (a->_font.sizeMul || a->_font.sizeAbs) {
                size = (size * a->_font.sizeMul) + a->_font.sizeAbs;
            }
        }
        _currentFont = Font::get(_currentFont->_name, size, weight);
    }
    return _currentFont;
}
    

attributed_string::attributed_string(const attributed_string& str) : string(str) {
    for (auto a : str._attributes) {
        setAttribute(a, a.start, a.end);
    }
}

void attributed_string::append(const attributed_string& str) {
    int32_t c = length();
    string::append(str);
    for (auto a : str._attributes) {
        setAttribute(a, a.start+c, a.end+c);
    }
}

void attributed_string::setAttribute(const attributed_string::attribute& attribute, int32_t start, int32_t end) {
    auto r = _attributes.insert(attribute_usage(attribute, start, end));
    //assert(r.second);
    if (!r.second) {
        _attributes.erase(r.first);
        _attributes.insert(attribute_usage(attribute, start, end));
    }
}
const attributed_string::attribute* attributed_string::getAttribute(int32_t pos, attributed_string::attribute::type type) {
    const attributed_string::attribute* attr = NULL;
    for (auto& attribUse : _attributes) {
        if (attribUse._type == type) {
            if (attribUse.start<= pos && attribUse.end>pos) {
                attr = &attribUse;
            }
        }
    }
    return attr;
}


void attributed_string::clearAttributes() {
    _attributes.clear();
}

attributed_string& attributed_string::operator=(const attributed_string& str) {
    string::operator=(str);
    _attributes = str._attributes;
    return *this;
}

void attributed_string::applyStyle(const string& styleName) {
    auto s = app->getStyle(styleName);
    if (s) {
        applyStyle(s);
    }
}

void attributed_string::applyStyle(const style* s) {
    float weight = s->fontWeightVal("font-weight");
    if (weight>0) {
        setAttribute(attributed_string::font_weight(weight), 0, -1);
    }
}


// Helpers for standard attributes
attributed_string::attribute attributed_string::font(oak::Font* font) {
    return attribute(font);
}
attributed_string::attribute attributed_string::font_size(float mul, float add) {
    return attribute( FONT_CHANGE(nullptr, mul, add, 0));
}
attributed_string::attribute attributed_string::font_weight(float weight) {
    return attribute( FONT_CHANGE(nullptr, 0,0, weight));
}
attributed_string::attribute attributed_string::bold() {
    return attribute( FONT_CHANGE(nullptr,0,0,FONT_WEIGHT_BOLD));
}
attributed_string::attribute attributed_string::forecolor(COLOR color) {
    return attribute(attribute::type::Forecolor, color);
}
attributed_string::attribute attributed_string::paragraphMetrics(const PARAGRAPH_METRICS& metrics) {
    return attribute(metrics);
}
attributed_string::attribute attributed_string::baselineOffset(float offset) {
    return attribute(attribute::type::BaselineOffset, offset);
}
attributed_string::attribute attributed_string::underline() {
    return attribute(attribute::type::Underline, 0.f);
}
