//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


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
    list<attribute_usage>::iterator it = _attributes.insert(_attributes.end(), attribute_usage(attribute, start, end));
    assert(_starts.size() == _ends.size());
    _starts.insert(it);
    _ends.insert(it);
    assert(_starts.size() == _ends.size());
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
    _starts.clear();
    _ends.clear();
}

attributed_string& attributed_string::operator=(const attributed_string& str) {
    string::operator=(str);
    _starts.clear();
    _ends.clear();
    _attributes = str._attributes;
    for (auto it = _attributes.begin() ; it!=_attributes.end() ; it++) {
        _starts.insert(it);
        _ends.insert(it);
    }
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
