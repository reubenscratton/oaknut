//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


XmlParser::XmlParser(const string& str) : _str(str), _ptr(0), _currentTag() {
}

XmlParser XmlParser::currentTagContents() {
    auto start = _ptr;
    auto openingTag = currentTag();
    string closingTag = "/"+openingTag;
    while (currentTag() != closingTag) {
        nextTag();
    }
    auto end = _ptr - (closingTag.lengthInBytes()+2);
    nextTag();
    return XmlParser(_str.substr(start, end));
}

string XmlParser::readAttributeName(const string& s, uint32_t& o) {
    s.skipWhitespace(o);
    auto start = o;
    while (o < s.lengthInBytes()) {
        char32_t ch = s.readChar(o);
        if ((ch>='A'&&ch<='Z')
         || (ch>='a'&&ch<='z')
         || (ch>='0'&&ch<='9')
         || ch=='_' || ch=='-' || ch==':') {
            continue;
        }
        s.rewind(o);
        break;
    }
    return string(s, start, o);
}

string XmlParser::readQuotedString(const string& s, uint32_t& o) {
    char quoteChar = '\0';
    if (s.skipChar(o, '\"')) quoteChar='\"';
    else if (s.skipChar(o, '\'')) quoteChar='\'';
    
    string q = s.readUpTo(o, quoteChar);
    s.readChar(o); // todo: need a readPast() api...
    
    uint32_t qo=0;
    string r;
    while (qo < q.lengthInBytes()) {
        r += q.readUpTo(qo, "&");
        if (qo < q.lengthInBytes()) {
            string code = q.readUpTo(qo, ";");
            if (code == "lt") r+="<";
            else if (code == "gt") r+=">";
            else if (code == "quot") r+="\"";
            else if (code == "apos") r+="\'";
            else if (code == "amp") r+="&";
            else assert(0); //todo: support char encoding &#60;
        }
    }
    return r;
}

string XmlParser::nextTag() {
    
    // Extract content up to the opening bracket
    string content = _str.readUpTo(_ptr, "<");
    if (_ptr < _str.lengthInBytes()) {
        _ptr++;
        if (_str.peekChar(_ptr)=='/') {
            _currentTag = _str.readUpTo(_ptr, ">");
        } else {
            _currentTag = _str.readUpToOneOf(_ptr, "> \t\r\n/");
            _currentAttribsStr = _str.readUpTo(_ptr, ">");
            _currentAttribs.clear();
        }
        _ptr++;
    } else {
        _currentTag.clear();
    }
    return content;
}

string XmlParser::attributeValue(const string& name) {
    if (_currentAttribsStr.lengthInBytes()) {
        uint32_t o=0;
        while (o<_currentAttribsStr.lengthInBytes()) {
            _currentAttribsStr.skipWhitespace(o);
            string attribName = readAttributeName(_currentAttribsStr, o);
            if (!attribName.lengthInBytes()) {
                break;
            }
            _currentAttribsStr.skipWhitespace(o);
            string attribValue;
            if (_currentAttribsStr.skipChar(o, '=')) {
                _currentAttribsStr.skipWhitespace(o);
                attribValue = readQuotedString(_currentAttribsStr, o);
            }
            _currentAttribs[attribName] = attribValue;
        }
        _currentAttribsStr.clear();
    }
    return _currentAttribs[name];
}


