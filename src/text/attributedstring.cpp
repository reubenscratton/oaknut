//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


AttributedString::AttributedString() {
    
}
AttributedString::AttributedString(const char* p) : string(p) {
}
AttributedString::AttributedString(const string& str) : string(str) {
}
AttributedString::AttributedString(const AttributedString& str) : string(str) {
    _attributes = str._attributes;
}

void AttributedString::setAttribute(const Attribute& attribute, int32_t start, int32_t end) {
    _attributes.emplace(attribute, start, end);
}
const Attribute* AttributedString::getAttribute(int32_t pos, Attribute::Type type) {
    const Attribute* attr = NULL;
    for (auto& attribUse : _attributes) {
        if (attribUse.attribute._type == type) {
            if (attribUse.start<= pos && attribUse.end>pos) {
                attr = &attribUse.attribute;
            }
        }
    }
    return attr;
}


void AttributedString::clearAttributes() {
    _attributes.clear();
}

AttributedString& AttributedString::operator=(const AttributedString& str) {
    string::operator=(str);
    _attributes = str._attributes;
    return *this;
}

