//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



int stringParseInt(const string& str) {
    int val = 0;
    int base = 10;
    StringProcessor it(str);
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


StringProcessor::StringProcessor(const string& str) : _str(str), _it(_str.begin()) {
}
StringProcessor::StringProcessor(string&& str) : _str(std::forward<string>(str)), _it(_str.begin()) {
}

bool StringProcessor::eof() {
    return _it.eof();
}

char32_t StringProcessor::peek() {
	return next(false);
}

char32_t StringProcessor::next() {
	return next(true);
}

char32_t StringProcessor::next(bool advance) {
    if (_it.eof()) {
        return 0;
    }
    char32_t ch = *_it;
    if (advance) {
        _it++;
    }
    return ch;
}

void StringProcessor::skipWhitespace() {
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
string StringProcessor::nextToken() {
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

string StringProcessor::nextIdentifier() {
    auto start = _it;
    auto end = start;
    while (!eof()) {
        char32_t ch = peek();
        if (!isIdentifierChar(ch)) {
            break;
        }
        next();
        end = _it;
    }
    return string(start, end);
}

string StringProcessor::nextNumber() {
    auto start = _it;
    auto end = start;
    bool seenDecimalPoint = false;
    while (!eof()) {
        char32_t ch = peek();
        if (isDigit(ch)) {
            next();
            end = _it;
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

string StringProcessor::nextToEndOfLine() {
    auto start = _it;
    auto end = start;
    while (!eof()) {
        char32_t ch = peek();
        if (ch=='\r' || ch=='\n') {
            break;
        }
        next();
        end = _it;
    }
    return string(start, end);

}


