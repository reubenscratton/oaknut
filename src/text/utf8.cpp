//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

int stringIndexOfChar(const string& str, char32_t ch) {
    return (int) str.find((char)ch); // todo: make unicode aware
}
bool stringContainsChar(const string& str, char32_t ch) {
    return str.find((char)ch) != string::npos; // todo: unicode
}


string stringSubstring(const string& str, int start, int end) {
    return str.substr(start, end); // todo: unicode
}
string stringSubstring(const string& str, int start) {
    return str.substr(start, str.length()); // todo: unicode
}

float stringParseDimension(string str) {
    bool isDP = false;
    bool isSP = false;
    if (stringEndsWith(str, "dp", true)) {
        isDP = true;
    } else if (stringEndsWith(str, "sp", true)) {
        isSP = true;
    } else if (stringEndsWith(str, "px", true)) {
    }
    float val = stringParseDouble(str);
    if (isDP) {
        val = app.dp(val);
    }
    return val;
}

void stringTrim(string& str) {
    while(str.length() > 0) {
        char ch=*str.begin();
        if (ch==' ' || ch=='\t' || ch=='\r' || ch=='\n') {
            str.erase(str.begin(),str.begin()+1);
        } else {
            break;
        }
    }
    while(str.length() > 0) {
        char ch=*str.rbegin();
        if (ch==' ' || ch=='\t' || ch=='\r' || ch=='\n') {
            str.erase(str.length()-1);
        } else {
            break;
        }
    }
}
string stringExtractUpTo(string& str, const string& sep, bool remove) {
    size_t i = str.find(sep);
    if (i > str.length()) {
        return "";
    }
    string result = str.substr(0,i);
    str.erase(str.begin(),str.begin()+i+(remove?sep.length():0));
    return result;
}
bool stringStartsWith(string& str, const string& prefix, bool eat) {
    if (prefix.length()>str.length()) {
        return false;
    }
    auto s = str.begin();
    auto p = prefix.begin();
    size_t l = prefix.length();
    while (l-- > 0) {
        if (*s++ != *p++) {
            return false;
        }
    }
    if (eat) {
        str.erase(0, prefix.length());
    }
    return true;
}

bool stringEndsWith(string& str, const string& suffix, bool remove) {
    if (suffix.length()>str.length()) {
        return false;
    }
    auto s = str.rbegin();
    auto p = suffix.rbegin();
    size_t l = suffix.length();
    while (l-- > 0) {
        if (*s++ != *p++) {
            return false;
        }
    }
    if (remove) {
        str.erase(str.end()-suffix.length(), str.end());
    }
    return true;

}


string stringFromInt(int i) {
    char ach[32];
    sprintf(ach, "%d", i);
    return string(ach);
}
string stringFromDouble(double d) {
    char ach[32];
    sprintf(ach, "%lf", d);
    return string(ach);
}
string stringFromFloat(float f) {
    char ach[32];
    sprintf(ach, "%f", f);
    return string(ach);
}
double stringParseDouble(const string& str) {
    return atof(str.data());
}
int stringParseInt(const string& str) {
    int val = 0;
    int base = 10;
    Utf8Iterator it(str);
    if (it.peek()=='0') {
        it.next();
        if (it.peek()=='x') {
            it.next();
            base = 16;
        }
    }
    while (!it.eof()) {
        char32_t ch = it.peek();
        if ((ch>='0' && ch<='9') || (base==16 && ((ch>='a'&&ch<='f')||(ch>='A'&&ch<='F')))) {
            int digit = (ch-'0');
            if (ch>='a' && ch<='f') digit = (ch+10-'a');
            else if (ch>='A' && ch<='F') digit = (ch+10-'A');
            val = val*base + digit;
            it.next();
        } else {
            break;
        }
    }
    return val;
}


const unsigned char kFirstBitMask = 128; // 1000000
//const unsigned char kSecondBitMask = 64; // 0100000
const unsigned char kThirdBitMask = 32; // 0010000
const unsigned char kFourthBitMask = 16; // 0001000
//const unsigned char kFifthBitMask = 8; // 0000100

Utf8Iterator::Utf8Iterator(const string& str) {
    _p = str.data();
    _cb = str.length();
}
Utf8Iterator::Utf8Iterator(const ByteBuffer* data) {
    _p = (const char*)data->data;
    _cb = data->cb;
}

bool Utf8Iterator::eof() {
    return _cb<=0;
}

char32_t Utf8Iterator::peek() {
	return next(false);
}

char32_t Utf8Iterator::next() {
	return next(true);
}

char32_t Utf8Iterator::next(bool advance) {
    if (_cb<=0) {
        return 0;
    }
    char32_t codePoint = 0;
	string::difference_type offset = 1;

    char firstByte = *_p;
 
    if(firstByte & kFirstBitMask) {
        if(firstByte & kThirdBitMask) { // three-octet code point.
            if(firstByte & kFourthBitMask) { // four-octet code point
                offset = 4;
                codePoint = (firstByte & 0x07) << 18;
                char secondByte = *(_p + 1);
                codePoint +=  (secondByte & 0x3f) << 12;
                char thirdByte = *(_p + 2);
                codePoint +=  (thirdByte & 0x3f) << 6;;
                char fourthByte = *(_p + 3);
                codePoint += (fourthByte & 0x3f);
            }
            else {
                offset = 3;
                codePoint = (firstByte & 0x0f) << 12;
                char secondByte = *(_p + 1);
                codePoint += (secondByte & 0x3f) << 6;
                char thirdByte = *(_p + 2);
                codePoint +=  (thirdByte & 0x3f);
            }
        }
        else {
            offset = 2;
            codePoint = (firstByte & 0x1f) << 6;
            char secondByte = *(_p + 1);
            codePoint +=  (secondByte & 0x3f);
        }
    }
    else {
        codePoint = firstByte;
    }
	if (advance) {
		_p += offset;
        _cb -= offset;
	}

    return codePoint;
}

void Utf8Iterator::skipWhitespace() {
	while(!eof()) {
		char32_t ch = peek();
		if (ch==' ' || ch=='\r' || ch=='\n' || ch=='\t') {
			next();
		} else {
			return;
		}
	}
}
inline bool isDigit(char ch) {
    return ch>='0'&&ch<='9';
}
inline bool isIdentifierChar(char ch) {
    return (ch>='A'&&ch<='Z')
        || (ch>='a'&&ch<='z')
        || isDigit(ch)
        || ch=='_' || ch=='-' || ch=='@';
}
inline bool isPunctuatorChar(char ch) {
    return (ch>=33 && ch<=47)       // !"#$&'()*+,-./
        || (ch>=58 && ch<=64)       // :;<=>?@
        || (ch>=91 && ch<=94)       // [\]^
        || (ch>=123 && ch<=126);    // {|}
}
string Utf8Iterator::nextToken() {
    if (!eof()) {
        char32_t ch = peek();
        if (isDigit(ch)) return nextNumber();
        else if (isIdentifierChar(ch)) return nextIdentifier();
        else if (isPunctuatorChar(ch)) {
            next();
            string str = "";
            str.push_back(ch);
            return str;
        } else {
            app.log("Warning: invalid char '%c'", ch);
        }
    }
    return "";
}

string Utf8Iterator::nextIdentifier() {
    const char* start = _p;
    const char* end = start;
    while (!eof()) {
        char32_t ch = peek();
        if (!isIdentifierChar(ch)) {
            break;
        }
        next();
        end = _p;
    }
    return string(start, end);
}

string Utf8Iterator::nextNumber() {
    const char* start = _p;
    const char* end = start;
    bool seenDecimalPoint = false;
    while (!eof()) {
        char32_t ch = peek();
        if (isDigit(ch)) {
            next();
            end = _p;
            continue;
        }
        if (ch=='.') {
            if (!seenDecimalPoint) {
                seenDecimalPoint = true;
                continue;
            }
        }
        break;
    }
    return string(start, end);
}

string Utf8Iterator::nextToEndOfLine() {
    const char* start = _p;
    const char* end = start;
    while (!eof()) {
        char32_t ch = peek();
        if (ch=='\r' || ch=='\n') {
            break;
        }
        next();
        end = _p;
    }
    return string(start, end);

}


void stringAppendCodepoint(string& str, char32_t ch) {
    if (ch <= 0x7F) {
        str.append((char*)&ch, 1);
    }
    else if (ch <= 0x7FF) {
        char ach[2];
        ach[0] = 0xC0 | (ch>>6);
        ach[1] = 0x80 | (ch&0x3f);
        str.append((char*)ach, 2);
    }
    else if (ch <= 0xFFFF) {
        char ach[3];
        ach[0] = 0xE0 | (ch>>12);
        ach[1] = 0x80 | ((ch>>6)&0x3f);
        ach[2] = 0x80 | (ch&0x3f);
        str.append((char*)ach, 3);
    }
    else if (ch <= 0x10FFFF) {
        char ach[4];
        ach[0] = 0xF0 | (ch>>18);
        ach[1] = 0x80 | ((ch>>12)&0x3f);
        ach[2] = 0x80 | ((ch>>6)&0x3f);
        ach[3] = 0x80 | (ch&0x3f);
        str.append((char*)ach, 4);
    }
    else assert(0);
}
