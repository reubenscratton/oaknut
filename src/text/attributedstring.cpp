//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


template <>
void attributed_string::enumerator<attributed_string::attribute_type::Font, class Font>::handleApplyingAttrib(const attribute_usage& a, int32_t end) {
    if (a._type == attribute_type::Font) {
        _stack.push(std::make_pair(end, a._font));
    }
    else if (a._type == attribute_type::FontSize) {
        class Font* prev = _stack.top().second;
        _stack.push(std::make_pair(end, Font::get(prev->_name, prev->_size * a._mul_add.mul + a._mul_add.add, prev->_weight)));
    }
    else if (a._type == attribute_type::FontWeight) {
        class Font* prev = _stack.top().second;
        _stack.push(std::make_pair(end, Font::get(prev->_name, prev->_size, a._f)));
    }
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
    _attributes.insert(attribute_usage(attribute, start, end));
}
const attributed_string::attribute* attributed_string::getAttribute(int32_t pos, attributed_string::attribute_type type) {
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
