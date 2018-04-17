//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class Utf8Iterator {
public:
    const char* _p;
    long _cb;
 
    Utf8Iterator(const string& str);
    Utf8Iterator(const class ByteBuffer* data);
    bool eof();
	char32_t next();
	char32_t peek();

	void skipWhitespace();
    string nextToken();
    string nextNumber();
    string nextIdentifier();
    string nextToEndOfLine();

private:
 	char32_t next(bool advance);

};

void stringAppendCodepoint(string& str, char32_t ch);
void stringTrim(string& str);
bool stringStartsWith(string& str, const string& prefix, bool remove);
bool stringEndsWith(string& str, const string& suffix, bool remove);
string stringExtractUpTo(string& str, const string& sep, bool remove);
bool stringContainsChar(const string& str, char32_t ch);
int stringIndexOfChar(const string& str, char32_t ch);
string stringSubstring(const string& str, int start, int end);
string stringSubstring(const string& str, int start);
string stringFromInt(int i);
string stringFromDouble(double d);
string stringFromFloat(float f);
double stringParseDouble(const string& str);
int stringParseInt(const string& str);
float stringParseDimension(string str);

