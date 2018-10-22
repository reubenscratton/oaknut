//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


const unsigned char kFirstBitMask = 128; // 1000000
//const unsigned char kSecondBitMask = 64; // 0100000
const unsigned char kThirdBitMask = 32; // 0010000
const unsigned char kFourthBitMask = 16; // 0001000
//const unsigned char kFifthBitMask = 8; // 0000100


int string::compare(const string& str) const noexcept {
    int cb = MIN(_cb, str._cb);
    int r = memcmp(_p, str._p, cb);
    if (r!=0) {
        return r;
    }
    if (_cb < str._cb) return -1;
    else if (_cb > str._cb) return 1;
    return 0;
}
int string::compare(const char* s) const {
    int scb = (int)strlen(s);
    int cb = MIN(_cb, scb);
    int r = memcmp(_p, s, cb);
    if (r!=0) {
        return r;
    }
    if (_cb < scb) return -1;
    else if (_cb > scb) return 1;
    return 0;

}
bool string::operator==(const string& rhs) const {
    return 0==compare(rhs);
}
bool string::operator==(const char* rhs) const {
    return 0==compare(rhs);
}
bool string::operator!=(const string& rhs) const {
    return 0!=compare(rhs);
}
bool string::operator!=(const char* rhs) const {
    return 0!=compare(rhs);
}
bool string::operator<(const string& rhs) const {
    return compare(rhs)<0;
}
bool string::operator<(const char*   rhs) const {
    return compare(rhs)<0;
}
char32_t string::operator[](const int32_t charIndex)  const {
    return charAt(charIndex);
}
char32_t string::charAt(const int32_t charIndex)  const {
    assert(charIndex < _cc);
    if (_cc == _cb) return _p[charIndex];
    int32_t byteIndex = charIndexToByteIndex(charIndex);
    return readUtf8(byteIndex);
}
string string::substr(int32_t charIndexStart, int32_t charIndexEnd) const {
    normaliseCharRange(charIndexStart, charIndexEnd);
    int32_t byteIndexStart = charIndexToByteIndex(charIndexStart);
    int32_t byteIndexEnd = charIndexToByteIndex(charIndexEnd);
    int32_t cb =  byteIndexEnd-byteIndexStart;
    return string(_p+byteIndexStart, cb);
}

int32_t string::charIndexToByteIndex(int32_t charIndex) const {
    if (_cc == _cb) return charIndex;
    char* p = _p;
    while (charIndex > 0) {
        if(*p & kFirstBitMask) {
            if(*p & kThirdBitMask) {
                p += (*p & kFourthBitMask) ? 4 : 3;
            }
            else {
                p += 2;
            }
        }
        else {
            p++;
        }
        charIndex--;
    }
    return int32_t(p - _p);
}
char32_t string::readUtf8(int32_t& byteOffset) const {
    assert(byteOffset >=0);
    if (byteOffset >= _cb) {
        byteOffset = -1;
        return 0;
    }
    char* p = _p+byteOffset;
    char32_t codePoint = 0;
    char firstByte = *p;
    if(firstByte & kFirstBitMask) {
        if(firstByte & kThirdBitMask) { // three-octet code point.
            if(firstByte & kFourthBitMask) { // four-octet code point
                byteOffset += 4;
                codePoint = (firstByte & 0x07) << 18;
                char secondByte = *(p + 1);
                codePoint +=  (secondByte & 0x3f) << 12;
                char thirdByte = *(p + 2);
                codePoint +=  (thirdByte & 0x3f) << 6;;
                char fourthByte = *(p + 3);
                codePoint += (fourthByte & 0x3f);
            }
            else {
                byteOffset += 3;
                codePoint = (firstByte & 0x0f) << 12;
                char secondByte = *(p + 1);
                codePoint += (secondByte & 0x3f) << 6;
                char thirdByte = *(p + 2);
                codePoint +=  (thirdByte & 0x3f);
            }
        }
        else {
            byteOffset += 2;
            codePoint = (firstByte & 0x1f) << 6;
            char secondByte = *(p + 1);
            codePoint +=  (secondByte & 0x3f);
        }
    }
    else {
        codePoint = firstByte;
        byteOffset++;
    }
    if (byteOffset >= _cb) {
        byteOffset = -1;
    }
    return codePoint;
}

int32_t string::find(const string& str) const {
    char* p = strstr(_p, str._p);
    return p ? int32_t(p-_p) : -1;
}
int32_t string::find(const string& str, int32_t start) const {
    auto o = charIndexToByteIndex(start);
    if (o >= _cb) {
        return -1;
    }
    char* p = strstr(_p + o, str._p);
    return p ? int32_t(p-_p) : -1;

}
int32_t string::find(const char* s) const {
    if (!_p) return -1;
    char* p = strstr(_p, s);
    return p ? int32_t(p-_p) : -1;
}
int32_t string::find(char32_t ch) const {
    int32_t byteIndex = 0;
    int32_t charIndex = 0;
    while (byteIndex >= 0) {
        if (ch == readUtf8(byteIndex)) {
            return charIndex;
        }
        charIndex++;
    }
    return -1;
}
bool string::contains(const string& str) const {
    return find(str) >=0;
}
bool string::contains(const char* s) const {
    return find(s) >=0;
}
bool string::contains(char32_t ch) const {
    return find(ch) >=0;
}

int32_t countUtf8Chars(const char* p, int32_t cb) {
    int32_t cc = 0;
    const char* pp = p;
    while (pp < p+cb) {
        if(*pp & kFirstBitMask) {
            if(*pp & kThirdBitMask) { // three-octet code point.
                pp += (*pp & kFourthBitMask) ? 4 : 3;  // four-octet code point
            } else {
                pp += 2;
            }
        } else {
            pp++;
        }
        cc++;
    }
    return cc;
}


void string::append(const string& str) {
    append(str._p, str._cb);
}
void string::append(const char* p) {
    if (!p) return;
    append(p, (int32_t)strlen(p));
}
void string::append(const char* p, int32_t cb) {
    if (!cb) return;
    int32_t newCb = _cb+cb;
    _p = (char*)realloc(_p, newCb+1);
    memcpy(_p+_cb, p, cb);
    _cb = newCb;
    _p[_cb] = 0;
    _cc += countUtf8Chars(p, cb);
}
void string::append(char32_t ch) {
    if (ch <= 0x7F) {
        append((char*)&ch, 1);
    }
    else if (ch <= 0x7FF) {
        char ach[2];
        ach[0] = 0xC0 | (ch>>6);
        ach[1] = 0x80 | (ch&0x3f);
        append((char*)ach, 2);
    }
    else if (ch <= 0xFFFF) {
        char ach[3];
        ach[0] = 0xE0 | (ch>>12);
        ach[1] = 0x80 | ((ch>>6)&0x3f);
        ach[2] = 0x80 | (ch&0x3f);
        append((char*)ach, 3);
    }
    else if (ch <= 0x10FFFF) {
        char ach[4];
        ach[0] = 0xF0 | (ch>>18);
        ach[1] = 0x80 | ((ch>>12)&0x3f);
        ach[2] = 0x80 | ((ch>>6)&0x3f);
        ach[3] = 0x80 | (ch&0x3f);
        append((char*)ach, 4);
    }
    else assert(0);
}

void string::insert(int32_t charIndex, const string& str) {
    insert(charIndex, str._p, str._cb);
}
void string::insert(int32_t charIndex, const char* p) {
    if (!p) return;
    insert(charIndex, p, (int32_t)strlen(p));
}
void string::insert(int32_t charIndex, const char* p, int32_t cb) {
    if (!cb) return;
    int newCb = _cb+cb;
    _p = (char*)realloc(_p, newCb+1);
    auto byteIndex = charIndexToByteIndex(charIndex);
    char* insertionPoint = _p + byteIndex;
    auto cbToMove = _cb - byteIndex;
    memmove(insertionPoint+cb, insertionPoint, cbToMove); // move tail forwards in memory
    memcpy(insertionPoint, p, cb);
    _cb = newCb;
    _p[_cb] = 0;
    _cc += countUtf8Chars(p, cb);
}
void string::erase(int32_t charIndex) {
    erase(charIndex, charIndex+1);
}
void string::normaliseCharRange(int32_t& charIndexStart, int32_t& charIndexEnd) const {
    if (charIndexEnd < 0) {
        charIndexEnd = _cc;
    }
    if (charIndexStart == charIndexEnd) return;
    if (charIndexStart > charIndexEnd) {
        auto tmp = charIndexStart;
        charIndexStart = charIndexEnd;
        charIndexEnd = tmp;
    }
}

void string::erase(int32_t charIndexStart, int32_t charIndexEnd) {
    normaliseCharRange(charIndexStart, charIndexEnd);
    int32_t byteIndexStart = charIndexToByteIndex(charIndexStart);
    int32_t byteIndexEnd = charIndexToByteIndex(charIndexEnd);
    int32_t cb =  byteIndexEnd-byteIndexStart;
    if (byteIndexEnd < _cb) {
        memmove(_p+byteIndexStart, _p+byteIndexEnd, _cb - byteIndexEnd);
    }
    _cb -= cb;
    _cc -= (charIndexEnd - charIndexStart);
    _p = (char*)realloc(_p ,_cb+1);
    _p[_cb] = 0;
}

string& string::operator=(const string& str) {
    assign(str._p, str._cb);
    return *this;
}
string& string::operator=(const char* s) {
    assign(s, s ? (int32_t)strlen(s) : 0);
    return *this;
}

string oak::operator+(const string& lhs, const string& rhs) {
    string r(lhs);
    r.append(rhs);
    return r;
}
string oak::operator+(const string& lhs, const char* s) {
    string r(lhs);
    r.append(s);
    return r;
}

void string::assign(const char* p, int32_t cb) {
    if (_p) {
        free(_p);
    }
    if (p) {
        _cb = cb;
        _p = (char*)malloc(_cb + 1);
        _p[_cb] = 0;
        memcpy(_p, p, _cb);
    } else {
        _p = NULL;
        _cb = 0;
    }
    _cc = countUtf8Chars(_p, _cb);
}

void string::countChars() {
    _cc = countUtf8Chars(_p, _cb);
}

void string::trim() {
    int32_t ci = 0;
    int32_t cLead = 0;
    while(ci < _cc) {
        char32_t ch=charAt(ci++);
        if (ch==' ' || ch=='\t' || ch=='\r' || ch=='\n') {
            cLead++;
        } else {
            break;
        }
    }
    int32_t cTrail = 0;
    ci = _cc-1;
    while(ci>=0) {
        char32_t ch=charAt(ci--);
        if (ch==' ' || ch=='\t' || ch=='\r' || ch=='\n') {
            cTrail++;
        } else {
            break;
        }
    }
    if (cTrail) erase(_cc-cTrail, _cc);
    if (cLead) erase(0, cLead);
}

bool string::hasPrefix(const string& prefix) const {
    if (prefix.length()>length()) {
        return false;
    }
    auto s = begin();
    auto p = prefix.begin();
    size_t l = prefix.length();
    while (l-- > 0) {
        if (*s++ != *p++) {
            return false;
        }
    }
    return true;
}
bool string::hadPrefix(const string& prefix) {
    if (hasPrefix(prefix)) {
        erase(0, prefix.length());
        return true;
    }
    return false;
}
bool string::hasSuffix(const string& suffix) const {
    int32_t ci = _cc - suffix._cc;
    if (ci<0) {
        return false;
    }
    int32_t cs = 0;
    while (cs < suffix.length()) {
        if (charAt(ci++) != suffix.charAt(cs++)) {
            return false;
        }
    }
    return true;

}
bool string::hadSuffix(const string& suffix) {
    if (hasSuffix(suffix)) {
        erase(_cc-suffix.length(), _cc);
        return true;
    }
    return false;
}


string::iterator string::iterator::operator++(int) {
    string::iterator retval = *this; ++(*this); return retval;
}
bool string::iterator::operator==(iterator other) const {
    return _str == other._str && _byteOffset==other._byteOffset;
}
bool string::iterator::operator!=(iterator other) const {
    return !(*this == other);
}
string::iterator::reference string::iterator::operator*() {
    if (_byteOffset<0) return 0;
    _byteOffsetNext = _byteOffset;
    return _str->readUtf8(_byteOffsetNext);
}
bool string::iterator::eof() const {return _byteOffset<0;}
char* string::iterator::data() const { return _str->_p + _byteOffset; }


string string::lowercase() const {
    string newstr;
    for (auto c: *this) {
        char32_t cl;
        if (c>=0x0041 && c<=0x5a) cl=c+0x20;
        else if (c>=0xC0 && c<=0xDE) cl=c+0x20;
        else if (c==0x0178) cl=0x00FF;
        else if (!(c&1) && (c>=0x0100 && c<=0x0136)) cl = c+1;
        else if ((c&1) && (c>=0x0139 && c<=0x0147)) cl = c+1;
        else if (!(c&1) && (c>=0x014A && c<=0x0176)) cl = c+1;
        // TODO: finish this https://www.ibm.com/support/knowledgecenter/en/ssw_ibm_i_72/nls/rbagslowtoupmaptable.htm
        else cl = c;
        newstr.append(cl);
    }
    return newstr;
}

string string::format(const char* fmt, ...) {
    int size = ((int)strlen(fmt)) * 2 + 50;   // Use a rubric appropriate for your code
    int n = 0;
    va_list ap;
    char* buff = NULL;
    while (1) {     // Maximum two passes on a POSIX system...
        buff = (char*)realloc(buff, size);
        va_start(ap, fmt);
        n = vsnprintf(buff, size, fmt, ap);
        va_end(ap);
        if (n > -1 && n < size) {  // Everything worked
            break;
        }
        if (n > -1)  // Needed size returned
            size = n + 1;   // For null char
        else
            size *= 2;      // Guess at a larger size (OS specific)
    }
    string str(buff, n);
    free(buff);
    return str;
}

string string::extractUpTo(const string& sep, bool remove) {
    auto i = find(sep);
    if (i < 0) {
        return "";
    }
    string result = substr(0,i);
    erase(0,i+(remove?sep.length():0));
    return result;
}
string string::tokenise(const string& sep) {
    string token = extractUpTo(sep, true);
    if (0 == token.length()) {
        token = *this;
        *this = "";
    }
    return token;
}

string string::hex(const void* p, int cb) {
    string str;
    str._cb = str._cc = cb * 2;
    str._p = (char*)malloc(str._cb + 1);
    const char* bytes = (const char*)p;
    char* o = (char*)str.data();
    for (size_t i=0 ; i<cb ; i++) {
        uint8_t byte = bytes[i];
        uint8_t hi_nib = (byte>>4);
        uint8_t lo_nib = (byte&15);
        *o++ = (hi_nib>=10) ? ((hi_nib-10)+'A') : (hi_nib+'0');
        *o++ = (lo_nib>=10) ? ((lo_nib-10)+'A') : (lo_nib+'0');
    }
    str._p[str._cb] = 0;
    return str;
}
bytearray string::unhex() {
    bytearray ba((_cb+1)>>1);
    uint8_t* p = (uint8_t*)ba.data();
    for (int i=0 ; i<_cb ; i+=2) {
        char ch = _p[i];
        uint8_t byte = (ch>='A'&&ch<='F') ? (10+ch-'A') : (
                      (ch>='a'&&ch<='f') ? (10+ch-'a') : (
                      (ch>='0'&&ch<='9') ? (ch-'0') : 0));
        byte <<= 4;
        ch = _p[i+1];
        byte |= (ch>='A'&&ch<='F') ? (10+ch-'A') : (
                (ch>='a'&&ch<='f') ? (10+ch-'a') : (
                (ch>='0'&&ch<='9') ? (ch-'0') : 0));
        *p++ = byte;
    }
    return ba;
}


string string::urlEncode() {
    string rv;
    string str(*this);
    while (str.length() > 0) {
        int span = (int)strcspn(str.data(), " :/?#[]@!$&'()*+,;=");
        rv += str.substr(0, span);
        str.erase(0, span);
        if (str.length() > 0) {
            char ch = str.charAt(0);
            str.erase(0,1);
            char fmt[8];
            sprintf(fmt, "%%%02X", ch);
            rv.append(fmt);
        }
    }
    return rv;
}

vector<string> string::split(const string& delimiter) {
    if (delimiter.length()==0) {
        return {*this};
    }
    vector<string> result;
    int32_t start = 0;
    while (start >= 0) {
        int32_t next = start;
        start = find(delimiter, next);
        result.push_back(substr(next, start));
        if (start < 0) {
            break;
        }
        start += delimiter.length();
    }
    return result;
}

int32_t string::asInt() {
    return atoi(_p);
}




size_t string::hash() const {
#if INTPTR_MAX == INT64_MAX
    return CityHash64(_p, _cb);
#elif INTPTR_MAX == INT32_MAX
    return CityHash32(_p, _cb);
#else
#error wtf?
#endif
}

