//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
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
    string nextToken();
    string nextNumber();
    string nextIdentifier();
    string nextToEndOfLine();

private:
 	char32_t next(bool advance);
    
    string _str;
    string::iterator _it;

};


string stringFromInt(int i);
string stringFromDouble(double d);
string stringFromFloat(float f);
double stringParseDouble(const string& str);
int stringParseInt(const string& str);

