//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class StringProcessor {
public:
 
    StringProcessor(const string& str);
    StringProcessor(string&& str);
    bool eof();
	char32_t next();
	char32_t peek();
    string::iterator current() const { return _it; }

	void skipWhitespace();
    void skipSpacesAndTabs();
    string nextToken();
    string nextNumber();
    string nextIdentifier();
    string nextQuotedString();
    string nextToEndOfLine();
    bool nextWas(const string& s);

private:
 	char32_t next(bool advance);
    
    string _str;
    string::iterator _it;

};



