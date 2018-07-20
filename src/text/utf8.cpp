//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



float stringParseDimension(string str) {
    bool isDP = false;
    bool isSP = false;
    if (str.hadSuffix("dp")) {
        isDP = true;
    } else if (str.hadSuffix("sp")) {
        isSP = true;
    } else if (str.hadSuffix("px")) {
    }
    float val = stringParseDouble(str);
    if (isDP) {
        val = app.dp(val);
    }
    return val;
}

string stringExtractUpTo(string& str, const string& sep, bool remove) {
    auto i = str.find(sep);
    if (i < 0) {
        return "";
    }
    string result = str.substr(0,i);
    str.erase(0,i+(remove?sep.length():0));
    return result;
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
	int offset = 1;

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
            str.append(ch);
            return str;
        } else {
            app.warn("Invalid char '%c'", ch);
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
    return string(start, int32_t(end-start));
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
    return string(start, int32_t(end-start));
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
    return string(start, int32_t(end-start));

}


