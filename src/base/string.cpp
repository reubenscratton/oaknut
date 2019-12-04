//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

const string string::empty;

// TODO: Implement string buffer excess capacity, to reduce number of reallocs.

oak::string oak::operator "" _S(const char *str, std::size_t len) {
    return oak::string::static_str(str, (int32_t)len);
}

// Buffer types
const uint8_t kTypeStandard = 0;
const uint8_t kTypeStatic = 1;
const uint8_t kTypeTiny = 2;

const unsigned char kFirstBitMask = 128; // 1000000
//const unsigned char kSecondBitMask = 64; // 0100000
const unsigned char kThirdBitMask = 32; // 0010000
const unsigned char kFourthBitMask = 16; // 0001000
//const unsigned char kFifthBitMask = 8; // 0000100

struct bufhdr {
    uint32_t refs;
    uint32_t cb;
    uint32_t cap;
};
inline struct bufhdr* BUFHDR(char* _buf) {
    return _buf ? (((struct bufhdr*)_buf)-1) : nullptr;
}



static inline char* buf_new(uint32_t cb) {
    /* static int s_strmallocs=0;
     static uint32_t s_malloctotal=0;
     s_strmallocs++;
     s_malloctotal += cb;
     app->log("string malloc %d total=%d", s_strmallocs, s_malloctotal);*/
    auto hdr = (struct bufhdr*)malloc(sizeof(struct bufhdr) + cb + 1);
    hdr->cap = cb;
    hdr->cb = cb;
    hdr->refs = 1;
    hdr++;
    return (char*)hdr;
}
static inline void buf_retain(char* buf) {
    struct bufhdr* hdr = (struct bufhdr*)(buf-sizeof(struct bufhdr));
    hdr->refs++;
}
static inline void buf_release(char* buf) {
    struct bufhdr* hdr = (struct bufhdr*)(buf-sizeof(struct bufhdr));
    assert(hdr->refs < 1000);
    if (0==--hdr->refs) {
        free(hdr);
    }
}
static inline void buf_realloc(char*& buf, int32_t cbDelta) {
    auto hdr = BUFHDR(buf);
    uint32_t cbNew = (hdr ? hdr->cap : 0) + cbDelta;
    auto hdrNew = (struct bufhdr*)realloc(hdr, sizeof(struct bufhdr) + cbNew + 1);
    hdrNew->cap = cbNew;
    hdrNew->cb = cbNew;
    if (!hdr) {
        hdrNew->refs = 1;
    }
    hdrNew++;
    buf = (char*)hdrNew;
}
void string::prepareToModify() {
    if (_type == kTypeStatic) {
        auto buf = buf_new(_cb);
        memcpy(buf, _buf+_offset, _cb);
        _buf = buf;
        _offset = 0;
        _type = kTypeStandard;
    } else {
        auto hdr = BUFHDR(_buf);
        if (hdr->refs > 1) {
            buf_release(_buf);
            auto buf = buf_new(_cb);
            memcpy(buf, _buf+_offset, _cb);
            _buf = buf;
            _offset = 0;
        }
    }
}


void string::alloc(uint32_t cb) {
    clear();
    _buf = buf_new(cb);
    _cb = cb;
    _buf[cb] = '\0';
    _offset = 0;
}


string::string(const string& s) : _buf(s._buf), _cb(s._cb), _offset(s._offset), _type(s._type) {
    if (_buf && _type==kTypeStandard) {
        buf_retain(_buf);
    }
}
string::string(const string& s, uint32_t offsetFrom, uint32_t offsetTo) : _buf(s._buf), _cb(offsetTo-offsetFrom), _offset(s._offset+offsetFrom), _type(s._type) {
    if (_type == kTypeStandard) {
        buf_retain(_buf);
    }
}

string::~string() {
   if (_buf && _type == kTypeStandard) {
       buf_release(_buf);
   }
}


const char* string::c_str() const {
    
    // If this buffer is a substring (i.e. end() is less than the buffer's end) then
    // create a new buffer/
    if (_buf && _type==kTypeStandard) {
        auto hdr = BUFHDR(_buf);
        if (_offset+_cb < hdr->cb) {
            auto newbuf = buf_new(_cb);
            memcpy(newbuf, start(), _cb);
            newbuf[_cb] = '\0';
            _offset = 0;
            buf_release(_buf);
            _buf = newbuf;
        }
    }
    return _buf;
}

char32_t lower(char32_t c) {
    if (c>=0x0041 && c<=0x5a) return c+0x20;
    else if (c>=0xC0 && c<=0xDE) return c+0x20;
    else if (c==0x0178) return 0x00FF;
    else if (!(c&1) && (c>=0x0100 && c<=0x0136)) return c+1;
    else if ((c&1) && (c>=0x0139 && c<=0x0147)) return c+1;
    else if (!(c&1) && (c>=0x014A && c<=0x0176)) return c+1;
    // TODO: finish this https://www.ibm.com/support/knowledgecenter/en/ssw_ibm_i_72/nls/rbagslowtoupmaptable.htm
    return c;
}

int32_t string::length() const noexcept {
    int32_t cc = 0;
    const char* pp = _buf+_offset;
    auto end = pp + _cb;
    while (pp < end) {
        if (*pp & kFirstBitMask) {
            if (*pp & kThirdBitMask) { // three-octet code point.
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

int string::compare(const string& str) const noexcept {
    int cb = MIN(_cb, str._cb);
    int r = memcmp(_buf+_offset, str._buf+str._offset, cb);
    if (r!=0) {
        return r;
    }
    if (_cb < str._cb) return -1;
    else if (_cb > str._cb) return 1;
    return 0;
}
int string::compare(const char* sz, uint32_t szlen) const noexcept {
    if (szlen==0xFFFFFFFF) szlen = uint32_t(strlen(sz));
    int cb = MIN(_cb, szlen);
    int r = memcmp(_buf+_offset, sz, cb);
    if (r!=0) {
        return r;
    }
    if (_cb < szlen) return -1;
    else if (_cb > szlen) return 1;
    return 0;
}
bool string::operator==(const string& rhs) const {
    return 0==compare(rhs);
}
bool string::operator==(const char* rhs) const {
    return 0==compare(rhs, 0xFFFFFFFF);
}
bool string::operator!=(const string& rhs) const {
    return 0!=compare(rhs);
}
bool string::operator!=(const char* rhs) const {
    return 0!=compare(rhs, 0xFFFFFFFF);
}
bool string::operator<(const string& rhs) const {
    return compare(rhs)<0;
}

char32_t string::peekChar(uint32_t offset) const {
    auto o = offset;
    return readUtf8(_buf+_offset, o);
}
char32_t string::readChar(uint32_t& offset) const {
    if (offset>=_cb) {
        return 0;
    }
    return readUtf8(_buf+_offset, offset);
}

char32_t string::operator[](const int32_t charIndex)  const {
    return charAt(charIndex);
}
char32_t string::charAt(const int32_t charIndex)  const {
    auto o = charIndexToOffset(charIndex);
    return readUtf8(_buf+_offset, o);
}

uint32_t string::charIndexToOffset(int32_t charIndex) const {
    uint32_t o = 0;
    auto start = _buf + _offset;
    while (charIndex > 0) {
        if(start[o] & kFirstBitMask) {
            if(start[o] & kThirdBitMask) {
                o += (start[o] & kFourthBitMask) ? 4 : 3;
            }
            else {
                o += 2;
            }
        }
        else {
            o++;
        }
        charIndex--;
    }
    if (charIndex < 0) {
        o = _cb;
        while (charIndex<=-1 && o>0) {
            o--;
            if (o>0 && (start[o] & 0xC0)==0x80) { // > 1 byte
                o--;
                if (o>0 && (start[o] & 0xC0)==0x80) { // > 2 bytes
                    o--;
                    if (o>0 && (start[o] & 0xC0)==0x80) { // 4 bytes
                        o--;
                        assert(0xF0 == (start[o] & 0xF8));
                    } else { // 3 bytes
                        assert(0xE0 == (start[o] & 0xF0));
                    }
                } else { // 2-bytes
                    assert(0xC0 == (start[o] & 0xE0));
                }
            } else { // 1-byte
                assert(!(start[o] & 0x80));
            }
            charIndex++;
        }
    }
    return o;
}

void string::rewind(uint32_t& o) const {
    if (o > 0) {
        auto s = start();
        o--;
        if (o>0 && (s[o] & 0xC0)==0x80) { // > 1 byte
            o--;
            if (o>0 && (s[o] & 0xC0)==0x80) { // > 2 bytes
                o--;
                if (o>0 && (s[o] & 0xC0)==0x80) { // 4 bytes
                    o--;
                    assert(0xF0 == (s[o] & 0xF8));
                } else { // 3 bytes
                    assert(0xE0 == (s[o] & 0xF0));
                }
            } else { // 2-bytes
                assert(0xC0 == (s[o] & 0xE0));
            }
        } else { // 1-byte
            assert(!(s[o] & 0x80));
        }
    }
}


string string::substr(uint32_t from, uint32_t to /*=0xFFFFFFFFU*/) const {
    if (from>to) std::swap(from, to);
    from = MIN(_cb, MAX(0, from));
    to = MIN(_cb, MAX(0, to));
    return string(*this, from, to);
}

string string::substrAt(int32_t from, int32_t to /*=-1*/) const {
    auto fromOffset = charIndexToOffset(from);
    auto toOffset = charIndexToOffset(to);
    return substr(fromOffset, toOffset);
}

string string::slice(int32_t charIndex1, int32_t charIndex2 /*=0*/) const {
    auto cc = length();
    // Single-arg form, param is the 'stop' point
    if (charIndex2 == 0) {
        charIndex2 = charIndex1;
        charIndex1 = 0;
    }
    // Convert -ve indexes into +ve indexes
    if (charIndex1 < 0) {
        charIndex1 += cc;
    }
    if (charIndex2 < 0) {
        charIndex2 += cc;
    }
    // Ensure right way round
    if (charIndex1 > charIndex2) {
        std::swap(charIndex1, charIndex2);
    }
    // Clamp
    charIndex1 = MIN(cc, MAX(0, charIndex1));
    charIndex2 = MIN(cc, MAX(0, charIndex2));
    return substr(charIndexToOffset(charIndex1), charIndexToOffset(charIndex2));
}


bool string::skipString(uint32_t& offset, const char* s) const {
    if (!s) {
        return false;
    }
    auto cb = strlen(s);
    if (cb > (_cb-offset)) {
        return false;
    }
    if (0==memcmp(start()+offset, s, cb)) {
        offset += cb;
        return true;
    }
    return false;
}

char32_t string::readUtf8(char* base, uint32_t& offset) {
    char32_t codePoint = 0;
    char firstByte = base[offset];
    if(firstByte & kFirstBitMask) {
        char secondByte = base[offset+1];
        if(firstByte & kThirdBitMask) { // three-octet code point.
            char thirdByte = base[offset+2];
            if(firstByte & kFourthBitMask) { // four-octet code point
                char fourthByte = base[offset+3];
                codePoint = ((firstByte & 0x07) << 18)
                          | ((secondByte & 0x3f) << 12)
                          | ((thirdByte & 0x3f) << 6)
                          | (fourthByte & 0x3f);
                offset += 4;
            }
            else {
                codePoint = ((firstByte & 0x0f) << 12)
                          | ((secondByte & 0x3f) << 6)
                          | (thirdByte & 0x3f);
                offset += 3;
            }
        }
        else {
            codePoint = ((firstByte & 0x1f) << 6)
                      | (secondByte & 0x3f);
            offset += 2;
        }
    }
    else {
        codePoint = firstByte;
        if (firstByte) {
            offset++;
        }
    }
    return codePoint;
}

// helper that returns the offset into 'buf' of the first occurrence of 'text'.
// If 'text' is not found then buflen is returned.
static inline uint32_t findStr(const char* buf, uint32_t buflen, const char* text, int32_t textLen) {
    uint32_t o = 0;
    uint32_t to = MAX(0, (buflen - textLen)+1);
    while (o < to) {
        char* match = (char*)memchr(buf+o, text[0], to-o);
        if (!match) {
            break;
        }
        o = uint32_t(match-buf);
        if (0==memcmp(match+1, text+1, textLen-1)) {
            return o;
        }
        o++;
    }
    return buflen;
}

uint32_t string::find(const char* szTextToFind, int32_t szTextLen/*=-1*/, uint32_t fromOffset/*=0*/) const {
    if (!szTextToFind || !szTextToFind[0] || !_buf) return 0xFFFFFFFF; // searching for an empty string
    auto o = _offset + fromOffset;
    if (szTextLen<0) szTextLen = strlen(szTextToFind);
    return fromOffset + findStr(_buf+o, _cb-fromOffset, szTextToFind, szTextLen);
}

uint32_t string::findLast(const char* szTextToFind, int32_t szTextLen/*=-1*/, uint32_t fromOffset/*=0*/) const {
    if (!szTextToFind || !szTextToFind[0] || !_buf) return 0xFFFFFFFF; // searching for an empty string
    if (szTextLen<0) szTextLen = strlen(szTextToFind);
    char* buf = _buf + _offset + fromOffset;
    uint32_t buflen = _cb-fromOffset;
    uint32_t r = 0xFFFFFFFF;
    while (buflen > 0) {
        uint32_t f = findStr(buf, buflen, szTextToFind, szTextLen);
        if (f > buflen) {
            break;
        }
        buf += f;
        r = buf - (_buf+_offset);
        buf += szTextLen;
        buflen -= (f+szTextLen);
    }
        
    return r;
}

void string::erase(uint32_t fromByteOffset, uint32_t toByteOffset/*=0xFFFFFFFFU*/) {
    if (fromByteOffset>toByteOffset) std::swap(fromByteOffset, toByteOffset);
    fromByteOffset = MIN(_cb, MAX(0, fromByteOffset));
    toByteOffset = MIN(_cb, MAX(0, toByteOffset));
    uint32_t cbToErase = toByteOffset - fromByteOffset;
    if (cbToErase<=0) {
        return;
    }
    prepareToModify();
    auto start = _buf + _offset;
    size_t cbToMove = _cb - toByteOffset;
    if (cbToMove > 0) {
        memmove(start + fromByteOffset, start + toByteOffset, cbToMove);
    }
    _cb -= cbToErase;
    _buf[_cb]='\0';
    buf_realloc(_buf, -cbToErase);
}
void string::eraseAt(int32_t startIndex, int32_t endIndex/*=-1*/) {
    auto fromByteOffset = charIndexToOffset(startIndex);
    if (endIndex == -1) {
        erase(fromByteOffset);
    } else {
        auto toByteOffset = charIndexToOffset(endIndex);
        erase(fromByteOffset, toByteOffset);
    }
}
void string::eraseLast() {
    auto lastCharOffset = charIndexToOffset(-1);
    erase(lastCharOffset);
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

void string::insert(uint32_t byteOffset, const char* p,  int32_t cb /*=-1*/) {
    if (!p) return;
    if (cb < 0) {
        cb = (int32_t)strlen(p);
    }
    if (!cb) return;
    if (!_buf) {
        assign(p, cb);
        return;
    }
    prepareToModify();
    buf_realloc(_buf, cb);
    auto cbToMove = _cb - byteOffset;
    char* insertPoint = this->start() + byteOffset;
    memmove(insertPoint+cb, insertPoint, cbToMove); // move tail forwards in memory
    memcpy(insertPoint, p, cb);
    _cb += cb;
    this->start()[_cb] = 0;
}



string& string::operator=(const string& str) {
    if (_buf && _type==kTypeStandard) {
        buf_release(_buf);
    }
    _buf = str._buf;
    _cb = str._cb;
    _offset = str._offset;
    _type = str._type;
    if (_buf && _type==kTypeStandard) {
        buf_retain(_buf);
    }
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
/*string oak::operator+(const string& lhs, const char* s) {
    string r(lhs);
    r.append(s);
    return r;
}*/

void string::clear() {
    if (_buf) {
        buf_release(_buf);
        _buf = nullptr;
        _cb = 0;
        _offset = 0;
    }
}

void string::assign(const char* p, int32_t cb) {
    if (_buf && _type==kTypeStandard) {
        buf_release(_buf);
    }
    _offset = 0;
    if (p) {
        if (cb<0) {
            cb = strlen(p);
        }
        _buf = buf_new(cb);
        _cb = cb;
        memcpy(_buf, p, _cb);
        _buf[cb] = 0;
    } else {
        _buf = nullptr;
        _cb = 0;
    }
}

void string::trim() {
    int32_t cLead = 0;
    int32_t i = 0;
    char* start = this->start();
    while(i < _cb) {
        char ch = start[i++];
        if (ch==' ' || ch=='\t' || ch=='\r' || ch=='\n') {
            cLead++;
        } else {
            break;
        }
    }
    int32_t cTrail = 0;
    i = _cb-1;
    while(i>=0) {
        char ch = start[i--];
        if (ch==' ' || ch=='\t' || ch=='\r' || ch=='\n') {
            cTrail++;
        } else {
            break;
        }
    }
    auto total = cLead+cTrail;
    if (total > 0) {
        *this = substr(cLead, -1-cTrail);
    }
}


bool string::hasPrefix(const string& prefix) const {
    if (prefix._cb > _cb) {
        return false;
    }
    return 0==memcmp(start(), prefix.start(), prefix._cb);
}
bool string::hadPrefix(const string& prefix) {
    if (hasPrefix(prefix)) {
        erase(0, prefix._cb);
        return true;
    }
    return false;
}
bool string::hasSuffix(const string& suffix) const {
    int32_t ci = _cb - suffix._cb;
    if (ci<0) {
        return false;
    }
    auto s = _cb - suffix._cb;
    return 0 == memcmp(start()+s, suffix.start(), suffix._cb);
}
bool string::hadSuffix(const string& suffix) {
    if (hasSuffix(suffix)) {
        erase(_cb - suffix._cb, _cb);
        return true;
    }
    return false;
}

bool string::replace(const string& search, const string& replacement) {
    int c=0;
    uint32_t start = 0;
    while (_cb > start) {
        start += findStr(this->start()+start, _cb-start, search.start(), search._cb);
        if (start>=_cb) {
            break;
        }
        auto pTo = start+search._cb;
        erase(start, pTo);
        insert(start, replacement);
        start += replacement._cb;
        c++;
    }
    return c>0;
}

/*string::ptr string::ptr::operator++(int) {
    string::ptr retval = *this; ++(*this); return retval;
}*/


string string::lowercase() const {
    string newstr;
    uint32_t o=0;
    while (o<_cb) {
        char32_t cl = lower(readChar(o));
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


string string::hex(const void* p, int cb) {
    string str;
    str._cb = cb * 2;
    str._buf = buf_new(str._cb);
    const char* bytes = (const char*)p;
    char* o = str._buf;
    for (size_t i=0 ; i<cb ; i++) {
        uint8_t byte = bytes[i];
        uint8_t hi_nib = (byte>>4);
        uint8_t lo_nib = (byte&15);
        *o++ = (hi_nib>=10) ? ((hi_nib-10)+'A') : (hi_nib+'0');
        *o++ = (lo_nib>=10) ? ((lo_nib-10)+'A') : (lo_nib+'0');
    }
    *o = '\0';
    return str;
}
bytearray string::unhex() {
    bytearray ba((_cb+1)>>1);
    uint8_t* p = (uint8_t*)ba.data();
    char* start = this->start();
    for (int i=0 ; i<_cb ; i+=2) {
        char ch = start[i];
        uint8_t byte = (ch>='A'&&ch<='F') ? (10+ch-'A') : (
                      (ch>='a'&&ch<='f') ? (10+ch-'a') : (
                      (ch>='0'&&ch<='9') ? (ch-'0') : 0));
        byte <<= 4;
        ch = start[i+1];
        byte |= (ch>='A'&&ch<='F') ? (10+ch-'A') : (
                (ch>='a'&&ch<='f') ? (10+ch-'a') : (
                (ch>='0'&&ch<='9') ? (ch-'0') : 0));
        *p++ = byte;
    }
    return ba;
}


string string::urlEncode() {
    char* buf = (char*)malloc(_cb*3+1);
    char* out = buf;
    char* in = start();
    for (int i=0 ; i<_cb ; i++) {
        char c = in[i];
        bool asciiOk = (c>='A'&&c<='Z') || (c>='a'&&c<='z') || (c>='0'&&c<='9')
                    || (c=='-') || (c=='.') || (c=='_') || (c=='~');
        if (asciiOk) {
            *out++ = c;
        } else if (c==' ') {
            *out++ = '+';
        } else {
            sprintf(out, "%%%02X", c);
            out += 3;
        }
    }
    *out='\0';
    string rv(buf, int32_t(out-buf));
    free(buf);
    return rv;
}

string string::urlDecode() {
    char* buf = (char*)malloc(_cb+1);
    char* out = buf;
    char* in = start();
    for (int i=0 ; i<_cb ; i++) {
        char c = in[i];
        if (c=='+') {
            *out++ = '-';
        } else if (c=='%') {
            string asciiCode(in+i+1, 2);
            auto asciiVal = asciiCode.unhex();
            *out++ = asciiVal.data()[0];
        } else {
            *out++ = c;
        }
    }
    *out='\0';
    string rv(buf, int32_t(out-buf));
    free(buf);
    return rv;
}

vector<string> string::split(const string& delimiter, int maxSplits/*=0*/) const {
    if (delimiter.length()==0) {
        return {*this};
    }
    vector<string> result;
    uint32_t o = 0;
    while (o<_cb) {
        uint32_t next = o;
        o += findStr(start()+o, _cb-o, delimiter.start(), delimiter._cb);
        result.push_back(substr(next, o));
        if (maxSplits>0) {
            maxSplits--;
            if (maxSplits <=0) {
                break;
            }
        }
        o += delimiter._cb;
    }
    return result;
}

int32_t string::asInt() {
    return atoi(_buf+_offset);
}
float string::asFloat() {
    return atof(_buf+_offset);
}

bytearray string::toByteArray() {
    bytearray b;
    b.assign((uint8_t*)start(), _cb);
    return b;
}




size_t string::hash() const {
#if INTPTR_MAX == INT64_MAX
    return CityHash64(start(), _cb);
#elif INTPTR_MAX == INT32_MAX
    return CityHash32(start(), _cb);
#else
#error wtf?
#endif
}

string string::join(const vector<string>& vec, const string& delimiter) {
    string s;
    for (int i=0 ; i<vec.size() ; i++) {
        if (i>0) {
            s += delimiter;
        }
        s += vec[i];
    }
    return s;
}

/*
char32_t string::ptr::peek() {
    if (_offset >= _cb) {
        return 0;
    }
    auto tmp = _offset;
    return readUtf8(base(), tmp);
}

char32_t string::ptr::next() {
    if (_offset >= _cb) {
        return 0;
    }
    return readUtf8(base(), _offset);
}*/


bool string::skipChar(uint32_t& offset, char ch) const {
    auto s = start();
    if (s[offset] == ch) {
        offset++;
        return true;
    }
    return false;
}

void string::skipSpacesAndTabs(uint32_t& offset) const {
    auto s = start();
    while (offset < _cb) {
        char ch=s[offset];
        if (ch==' ' || ch=='\t') {
            offset++;
        } else {
            break;
        }
    }
}

void string::skipWhitespace(uint32_t& offset) const {
    auto s = start();
    while (offset < _cb) {
        char ch=s[offset];
        if (ch==' ' || ch=='\r' || ch=='\n' || ch=='\t') {
            offset++;
        } else {
            break;
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
string string::readToken(uint32_t& offset) const {
    if (offset<_cb) {
        char32_t ch = peekChar(offset);
        if (isDigit(ch)) return readNumber(offset);
        else if (isIdentifierChar(ch)) return readIdentifier(offset);
        else if (isPunctuatorChar(ch)) {
            // wtf is this code doing?
            assert(0);
            readChar(offset);
            string str = "";
            str.append(ch);
            return str;
        } else {
            app->warn("Invalid char '%c'", ch);
        }
    }
    return "";
}


string string::readIdentifier(uint32_t& o) const {
    auto start = o;
    while (o < _cb) {
        char32_t ch = readChar(o);
        if (!isIdentifierChar(ch)) {
            rewind(o);
            break;
        }
    }
    return string(*this, start, o);
}

string string::readNumber(uint32_t& o) const {
    auto start = o;
    bool seenDecimalPoint = false;
    while (o < _cb) {
        char32_t ch = readChar(o);
        if (isDigit(ch)) {
            continue;
        }
        if (ch=='.') {
            if (!seenDecimalPoint) {
                seenDecimalPoint = true;
                continue;
            }
        }
        rewind(o);
        break;
    }
    return string(*this, start, o);
}

string string::readToEndOfLine(uint32_t& o) const {
    auto start = o;
    while (o < _cb) {
        char32_t ch = readChar(o);
        if (ch=='\r' || ch=='\n') {
            rewind(o);
            break;
        }
    }
    return string(*this, start, _offset);
}

string string::readUpTo(uint32_t& offset, const string& str) const {
    auto o = offset;
    offset += findStr(start()+o, _cb-o, str.start(), str._cb);
    return substr(o, offset);
}
string string::readUpTo(uint32_t& offset, char ch) const {
    auto o = offset;
    offset += findStr(start()+o, _cb-o, &ch, 1);
    return substr(o, offset);
}

string string::readUpToOneOf(uint32_t& offset, const string& str) const {
    auto s = offset;
    bool found = false;
    while (offset<_cb && !found) {
        auto o = offset;
        char32_t ch = readChar(offset);
        uint32_t oo = 0;
        while (oo<str._cb){
            if (str.readChar(oo) == ch) {
                offset = o;
                found = true;
                break;
            }
        }
    }
    return substr(s, offset);
}

bool string::isOneOf(const vector<string>& vec) const {
    for (auto& e : vec) {
        if (*this == e) {
            return true;
        }
    }
    return false;
}
