//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


// Copyright (c) 2011 Google, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


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



uint32 oak::CityHash32(const char *s, uint32 len) {
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

uint64 oak::CityHash64(const char *s, uint32 len) {
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





/*
SHA-1 in C
By Steve Reid <steve@edmweb.com>
100% Public Domain
*/

typedef struct {
    u_int32_t state[5];
    u_int32_t count[2];
    unsigned char buffer[64];
} SHA1_CTX;


/* #define LITTLE_ENDIAN * This should be #define'd already, if true. */
/* #define SHA1HANDSOFF * Copies data before messing with it. */

#define SHA1HANDSOFF

#ifndef BYTE_ORDER
#if (BSD >= 199103)
# include <machine/endian.h>
#else
#if defined(linux) || defined(__linux__)
# include <endian.h>
#else
#define    LITTLE_ENDIAN    1234    /* least-significant byte first (vax, pc) */
#define    BIG_ENDIAN    4321    /* most-significant byte first (IBM, net) */
#define    PDP_ENDIAN    3412    /* LSB first in word, MSW first in long (pdp)*/

#if defined(vax) || defined(ns32000) || defined(sun386) || defined(__i386__) || \
    defined(MIPSEL) || defined(_MIPSEL) || defined(BIT_ZERO_ON_RIGHT) || \
    defined(__alpha__) || defined(__alpha)
#define BYTE_ORDER    LITTLE_ENDIAN
#endif

#if defined(sel) || defined(pyr) || defined(mc68000) || defined(sparc) || \
    defined(is68k) || defined(tahoe) || defined(ibm032) || defined(ibm370) || \
    defined(MIPSEB) || defined(_MIPSEB) || defined(_IBMR2) || defined(DGUX) ||\
    defined(apollo) || defined(__convex__) || defined(_CRAY) || \
    defined(__hppa) || defined(__hp9000) || \
    defined(__hp9000s300) || defined(__hp9000s700) || \
    defined (BIT_ZERO_ON_LEFT) || defined(m68k) || defined(__sparc)
#define BYTE_ORDER    BIG_ENDIAN
#endif
#endif /* linux */
#endif /* BSD */
#endif /* BYTE_ORDER */

#if defined(__BYTE_ORDER) && !defined(BYTE_ORDER)
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#define BYTE_ORDER LITTLE_ENDIAN
#else
#define BYTE_ORDER BIG_ENDIAN
#endif
#endif

#if !defined(BYTE_ORDER) || \
    (BYTE_ORDER != BIG_ENDIAN && BYTE_ORDER != LITTLE_ENDIAN && \
    BYTE_ORDER != PDP_ENDIAN)
    /* you must determine what the correct bit order is for
     * your compiler - the next line is an intentional error
     * which will force your compiles to bomb until you fix
     * the above macros.
     */
#error "Undefined or invalid BYTE_ORDER"
#endif

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
#if BYTE_ORDER == LITTLE_ENDIAN
#define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
    |(rol(block->l[i],8)&0x00FF00FF))
#elif BYTE_ORDER == BIG_ENDIAN
#define blk0(i) block->l[i]
#else
#error "Endianness not defined!"
#endif
#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);


/* Hash a single 512-bit block. This is the core of the algorithm. */

static void SHA1Transform(u_int32_t state[5], const unsigned char buffer[64]) {
u_int32_t a, b, c, d, e;
typedef union {
    unsigned char c[64];
    u_int32_t l[16];
} CHAR64LONG16;
#ifdef SHA1HANDSOFF
CHAR64LONG16 block[1];  /* use array to appear as a pointer */
    memcpy(block, buffer, 64);
#else
    /* The following had better never be used because it causes the
     * pointer-to-const buffer to be cast into a pointer to non-const.
     * And the result is written through.  I threw a "const" in, hoping
     * this will cause a diagnostic.
     */
CHAR64LONG16* block = (const CHAR64LONG16*)buffer;
#endif
    /* Copy context->state[] to working vars */
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    /* 4 rounds of 20 operations each. Loop unrolled. */
    R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
    R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
    R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
    R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
    R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
    R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
    R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
    R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
    R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
    R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
    R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
    R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
    R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
    R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
    R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
    R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
    R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
    R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
    R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
    R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
    /* Add the working vars back into context.state[] */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    /* Wipe variables */
    a = b = c = d = e = 0;
#ifdef SHA1HANDSOFF
    memset(block, '\0', sizeof(block));
#endif
}


/* SHA1Init - Initialize new context */

static void SHA1Init(SHA1_CTX* context) {
    /* SHA1 initialization constants */
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}


/* Run your data through this. */

static void SHA1Update(SHA1_CTX* context, const unsigned char* data, u_int32_t len) {
u_int32_t i;
u_int32_t j;

    j = context->count[0];
    if ((context->count[0] += len << 3) < j)
    context->count[1]++;
    context->count[1] += (len>>29);
    j = (j >> 3) & 63;
    if ((j + len) > 63) {
        memcpy(&context->buffer[j], data, (i = 64-j));
        SHA1Transform(context->state, context->buffer);
        for ( ; i + 63 < len; i += 64) {
            SHA1Transform(context->state, &data[i]);
        }
        j = 0;
    }
    else i = 0;
    memcpy(&context->buffer[j], &data[i], len - i);
}


/* Add padding and return the message digest. */

static void SHA1Final(sha1_t& digest, SHA1_CTX* context) {
unsigned i;
unsigned char finalcount[8];
unsigned char c;

#if 0    /* untested "improvement" by DHR */
    /* Convert context->count to a sequence of bytes
     * in finalcount.  Second element first, but
     * big-endian order within element.
     * But we do it all backwards.
     */
    unsigned char *fcp = &finalcount[8];

    for (i = 0; i < 2; i++)
    {
    u_int32_t t = context->count[i];
    int j;

    for (j = 0; j < 4; t >>= 8, j++)
        *--fcp = (unsigned char) t
    }
#else
    for (i = 0; i < 8; i++) {
        finalcount[i] = (unsigned char)((context->count[(i >= 4 ? 0 : 1)]
         >> ((3-(i & 3)) * 8) ) & 255);  /* Endian independent */
    }
#endif
    c = 0200;
    SHA1Update(context, &c, 1);
    while ((context->count[0] & 504) != 448) {
    c = 0000;
        SHA1Update(context, &c, 1);
    }
    SHA1Update(context, finalcount, 8);  /* Should cause a SHA1Transform() */
    for (i = 0; i < 20; i++) {
        digest.bytes[i] = (unsigned char)
         ((context->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
    }
    /* Wipe variables */
    memset(context, '\0', sizeof(*context));
    memset(&finalcount, '\0', sizeof(finalcount));
}
/* ================ end of sha1.c ================ */

bool sha1_t::operator<(const struct sha1_t &s1) const {
    return memcmp(bytes, s1.bytes, sizeof(bytes)) < 0;
}
bool sha1_t::operator==(const struct sha1_t &s1) const {
    return memcmp(bytes, s1.bytes, sizeof(bytes)) == 0;
}


sha1_t oak::sha1(const string& str) {
    sha1_t hash;
    SHA1_CTX ctx;
    SHA1Init(&ctx);
    SHA1Update(&ctx, (uint8_t*)str.start(), str.lengthInBytes());
    SHA1Final(hash, &ctx);
    return hash;
}

