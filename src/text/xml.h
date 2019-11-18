//
// Copyright © 2019 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 A non-validating streaming parser, a good choice if you don't need or want the whole DOM in memory but need to extract a custom
 data representation from a single sweep of the XML data.

currentTag() - returns the name of the current tag, or an empty string if there isn't one (empty or malformed doc).
attributeCount() - returns the number of attributes on the current tag
attributeValue(name) - returns the value of the named attribute, or an empty string if it doesnt exist
attributeValue(index) - returns the value of the indexed attribute, or an empty string if out of bounds
string nextTag() - moves to the next tag and returns the non-tag content leading up to it.
void toClosingTag() - moves to the closing tag corresponding to the current tag. Does nothing if current tag is a closing tag.

 */

class XmlParser : public Object {
public:
    XmlParser(const string& str);
    
    bool eof() { return _ptr >= _str.lengthInBytes(); }
    const string& currentTag() const { return _currentTag; }
    string nextTag();
    string attributeValue(const string& name);
    
    XmlParser currentTagContents();
    
private:
    string _str;
    uint32_t _ptr;
    string _currentTag, _currentAttribsStr;
    map<string, string> _currentAttribs;
    
    string readAttributeName(const string& s, uint32_t& o);
    string readQuotedString(const string& s, uint32_t& o);
};
