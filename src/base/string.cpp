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
    assert(byteOffset >=0 && byteOffset < _cb);
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
int32_t string::find(const char* s) const {
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

string operator+(const string& lhs, const string& rhs) {
    string r(lhs);
    r.append(rhs);
    return r;
}
string operator+(const string& lhs, const char* s) {
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
    string str(buff, size);
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

typedef uint8_t uint8;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef std::pair<uint64, uint64> uint128;


static uint64 UNALIGNED_LOAD64(const char *p) {
    uint64 result;
    memcpy(&result, p, sizeof(result));
    return result;
}

static uint32 UNALIGNED_LOAD32(const char *p) {
    uint32 result;
    memcpy(&result, p, sizeof(result));
    return result;
}

#if defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)
#elif defined(__OpenBSD__)
#include <sys/types.h>
#define bswap_32(x) swap32(x)
#define bswap_64(x) swap64(x)
#elif defined(__NetBSD__)
#include <sys/types.h>
#include <machine/bswap.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_32)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)
#endif
#else
#include <byteswap.h>
#endif

#ifdef WORDS_BIGENDIAN
#define uint32_in_expected_order(x) (bswap_32(x))
#define uint64_in_expected_order(x) (bswap_64(x))
#else
#define uint32_in_expected_order(x) (x)
#define uint64_in_expected_order(x) (x)
#endif

#if !defined(LIKELY)
#if HAVE_BUILTIN_EXPECT
#define LIKELY(x) (__builtin_expect(!!(x), 1))
#else
#define LIKELY(x) (x)
#endif
#endif

inline uint64 Uint128Low64(const uint128& x) { return x.first; }
inline uint64 Uint128High64(const uint128& x) { return x.second; }

static uint64 Fetch64(const char *p) {
    return uint64_in_expected_order(UNALIGNED_LOAD64(p));
}

static uint32 Fetch32(const char *p) {
    return uint32_in_expected_order(UNALIGNED_LOAD32(p));
}

// Some primes between 2^63 and 2^64 for various uses.
static const uint64 k0 = 0xc3a5c85c97cb3127ULL;
static const uint64 k1 = 0xb492b66fbe98f273ULL;
static const uint64 k2 = 0x9ae16a3b2f90404fULL;

// Magic numbers for 32-bit hashing.  Copied from Murmur3.
static const uint32 c1 = 0xcc9e2d51;
static const uint32 c2 = 0x1b873593;

// A 32-bit to 32-bit integer hash copied from Murmur3.
static uint32 fmix(uint32 h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

static uint32 Rotate32(uint32 val, int shift) {
    // Avoid shifting by 32: doing so yields an undefined result.
    return shift == 0 ? val : ((val >> shift) | (val << (32 - shift)));
}

#undef PERMUTE3
#define PERMUTE3(a, b, c) do { std::swap(a, b); std::swap(a, c); } while (0)

static uint32 Mur(uint32 a, uint32 h) {
    // Helper from Murmur3 for combining two 32-bit values.
    a *= c1;
    a = Rotate32(a, 17);
    a *= c2;
    h ^= a;
    h = Rotate32(h, 19);
    return h * 5 + 0xe6546b64;
}



uint32 CityHash32(const char *s, uint32 len) {
    if (len <= 24) {
        if (len <= 12) {
            if (len <= 4) {
                uint32 b = 0;
                uint32 c = 9;
                for (size_t i = 0; i < len; i++) {
                    signed char v = s[i];
                    b = b * c1 + v;
                    c ^= b;
                }
                return fmix(Mur(b, Mur(len, c)));
            } else {
                uint32 a = len, b = len * 5, c = 9, d = b;
                a += Fetch32(s);
                b += Fetch32(s + len - 4);
                c += Fetch32(s + ((len >> 1) & 4));
                return fmix(Mur(c, Mur(b, Mur(a, d))));
            }
        } else {
            uint32 a = Fetch32(s - 4 + (len >> 1));
            uint32 b = Fetch32(s + 4);
            uint32 c = Fetch32(s + len - 8);
            uint32 d = Fetch32(s + (len >> 1));
            uint32 e = Fetch32(s);
            uint32 f = Fetch32(s + len - 4);
            uint32 h = len;
            return fmix(Mur(f, Mur(e, Mur(d, Mur(c, Mur(b, Mur(a, h)))))));
        }
    }
    
    // len > 24
    uint32 h = len, g = c1 * len, f = g;
    uint32 a0 = Rotate32(Fetch32(s + len - 4) * c1, 17) * c2;
    uint32 a1 = Rotate32(Fetch32(s + len - 8) * c1, 17) * c2;
    uint32 a2 = Rotate32(Fetch32(s + len - 16) * c1, 17) * c2;
    uint32 a3 = Rotate32(Fetch32(s + len - 12) * c1, 17) * c2;
    uint32 a4 = Rotate32(Fetch32(s + len - 20) * c1, 17) * c2;
    h ^= a0;
    h = Rotate32(h, 19);
    h = h * 5 + 0xe6546b64;
    h ^= a2;
    h = Rotate32(h, 19);
    h = h * 5 + 0xe6546b64;
    g ^= a1;
    g = Rotate32(g, 19);
    g = g * 5 + 0xe6546b64;
    g ^= a3;
    g = Rotate32(g, 19);
    g = g * 5 + 0xe6546b64;
    f += a4;
    f = Rotate32(f, 19);
    f = f * 5 + 0xe6546b64;
    size_t iters = (len - 1) / 20;
    do {
        uint32 a0 = Rotate32(Fetch32(s) * c1, 17) * c2;
        uint32 a1 = Fetch32(s + 4);
        uint32 a2 = Rotate32(Fetch32(s + 8) * c1, 17) * c2;
        uint32 a3 = Rotate32(Fetch32(s + 12) * c1, 17) * c2;
        uint32 a4 = Fetch32(s + 16);
        h ^= a0;
        h = Rotate32(h, 18);
        h = h * 5 + 0xe6546b64;
        f += a1;
        f = Rotate32(f, 19);
        f = f * c1;
        g += a2;
        g = Rotate32(g, 18);
        g = g * 5 + 0xe6546b64;
        h ^= a3 + a1;
        h = Rotate32(h, 19);
        h = h * 5 + 0xe6546b64;
        g ^= a4;
        g = bswap_32(g) * 5;
        h += a4 * 5;
        h = bswap_32(h);
        f += a0;
        PERMUTE3(f, h, g);
        s += 20;
    } while (--iters != 0);
    g = Rotate32(g, 11) * c1;
    g = Rotate32(g, 17) * c1;
    f = Rotate32(f, 11) * c1;
    f = Rotate32(f, 17) * c1;
    h = Rotate32(h + g, 19);
    h = h * 5 + 0xe6546b64;
    h = Rotate32(h, 17) * c1;
    h = Rotate32(h + f, 19);
    h = h * 5 + 0xe6546b64;
    h = Rotate32(h, 17) * c1;
    return h;
}

// Bitwise right rotate.  Normally this will compile to a single
// instruction, especially if the shift is a manifest constant.
static uint64 Rotate(uint64 val, int shift) {
    // Avoid shifting by 64: doing so yields an undefined result.
    return shift == 0 ? val : ((val >> shift) | (val << (64 - shift)));
}

static uint64 ShiftMix(uint64 val) {
    return val ^ (val >> 47);
}

// Hash 128 input bits down to 64 bits of output.
// This is intended to be a reasonably good hash function.
static uint64 HashLen16(uint64 u, uint64 v) {
    uint128 x = uint128(u, v);
    // Murmur-inspired hashing.
    const uint64 kMul = 0x9ddfea08eb382d69ULL;
    uint64 a = (Uint128Low64(x) ^ Uint128High64(x)) * kMul;
    a ^= (a >> 47);
    uint64 b = (Uint128High64(x) ^ a) * kMul;
    b ^= (b >> 47);
    b *= kMul;
    return b;
}

static uint64 HashLen16(uint64 u, uint64 v, uint64 mul) {
    // Murmur-inspired hashing.
    uint64 a = (u ^ v) * mul;
    a ^= (a >> 47);
    uint64 b = (v ^ a) * mul;
    b ^= (b >> 47);
    b *= mul;
    return b;
}


// Return a 16-byte hash for 48 bytes.  Quick and dirty.
// Callers do best to use "random-looking" values for a and b.
static pair<uint64, uint64> WeakHashLen32WithSeeds(
                                                   uint64 w, uint64 x, uint64 y, uint64 z, uint64 a, uint64 b) {
    a += w;
    b = Rotate(b + a + z, 21);
    uint64 c = a;
    a += x;
    a += y;
    b += Rotate(a, 44);
    return make_pair(a + z, b + c);
}

// Return a 16-byte hash for s[0] ... s[31], a, and b.  Quick and dirty.
static pair<uint64, uint64> WeakHashLen32WithSeeds(
                                                   const char* s, uint64 a, uint64 b) {
    return WeakHashLen32WithSeeds(Fetch64(s),
                                  Fetch64(s + 8),
                                  Fetch64(s + 16),
                                  Fetch64(s + 24),
                                  a,
                                  b);
}

uint64 CityHash64(const char *s, uint32 len) {
    if (len <= 32) {
        if (len <= 16) {
            if (len >= 8) {
                uint64 mul = k2 + len * 2;
                uint64 a = Fetch64(s) + k2;
                uint64 b = Fetch64(s + len - 8);
                uint64 c = Rotate(b, 37) * mul + a;
                uint64 d = (Rotate(a, 25) + b) * mul;
                return HashLen16(c, d, mul);
            }
            if (len >= 4) {
                uint64 mul = k2 + len * 2;
                uint64 a = Fetch32(s);
                return HashLen16(len + (a << 3), Fetch32(s + len - 4), mul);
            }
            if (len > 0) {
                uint8 a = s[0];
                uint8 b = s[len >> 1];
                uint8 c = s[len - 1];
                uint32 y = static_cast<uint32>(a) + (static_cast<uint32>(b) << 8);
                uint32 z = len + (static_cast<uint32>(c) << 2);
                return ShiftMix(y * k2 ^ z * k0) * k2;
            }
            return k2;
        } else {
            uint64 mul = k2 + len * 2;
            uint64 a = Fetch64(s) * k1;
            uint64 b = Fetch64(s + 8);
            uint64 c = Fetch64(s + len - 8) * mul;
            uint64 d = Fetch64(s + len - 16) * k2;
            return HashLen16(Rotate(a + b, 43) + Rotate(c, 30) + d,
                             a + Rotate(b + k2, 18) + c, mul);
        }
    } else if (len <= 64) {
        uint64 mul = k2 + len * 2;
        uint64 a = Fetch64(s) * k2;
        uint64 b = Fetch64(s + 8);
        uint64 c = Fetch64(s + len - 24);
        uint64 d = Fetch64(s + len - 32);
        uint64 e = Fetch64(s + 16) * k2;
        uint64 f = Fetch64(s + 24) * 9;
        uint64 g = Fetch64(s + len - 8);
        uint64 h = Fetch64(s + len - 16) * mul;
        uint64 u = Rotate(a + g, 43) + (Rotate(b, 30) + c) * 9;
        uint64 v = ((a + g) ^ d) + f + 1;
        uint64 w = bswap_64((u + v) * mul) + h;
        uint64 x = Rotate(e + f, 42) + c;
        uint64 y = (bswap_64((v + w) * mul) + g) * mul;
        uint64 z = e + f + c;
        a = bswap_64((x + z) * mul + y) + b;
        b = ShiftMix((z + a) * mul + d + h) * mul;
        return b + x;
    }
    
    // For strings over 64 bytes we hash the end first, and then as we
    // loop we keep 56 bytes of state: v, w, x, y, and z.
    uint64 x = Fetch64(s + len - 40);
    uint64 y = Fetch64(s + len - 16) + Fetch64(s + len - 56);
    uint64 z = HashLen16(Fetch64(s + len - 48) + len, Fetch64(s + len - 24));
    pair<uint64, uint64> v = WeakHashLen32WithSeeds(s + len - 64, len, z);
    pair<uint64, uint64> w = WeakHashLen32WithSeeds(s + len - 32, y + k1, x);
    x = x * k1 + Fetch64(s);
    
    // Decrease len to the nearest multiple of 64, and operate on 64-byte chunks.
    len = (len - 1) & ~static_cast<size_t>(63);
    do {
        x = Rotate(x + y + v.first + Fetch64(s + 8), 37) * k1;
        y = Rotate(y + v.second + Fetch64(s + 48), 42) * k1;
        x ^= w.second;
        y += v.first + Fetch64(s + 40);
        z = Rotate(z + w.first, 33) * k1;
        v = WeakHashLen32WithSeeds(s, v.second * k1, x + w.first);
        w = WeakHashLen32WithSeeds(s + 32, z + w.second, y + Fetch64(s + 16));
        std::swap(z, x);
        s += 64;
        len -= 64;
    } while (len != 0);
    return HashLen16(HashLen16(v.first, w.first) + ShiftMix(y) * k1 + z,
                     HashLen16(v.second, w.second) + x);
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

