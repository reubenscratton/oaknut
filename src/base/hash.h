//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


// CityHash - a fast general-purpose hash from Google, see https://github.com/google/cityhash
uint32_t CityHash32(const char *s, uint32_t len);
uint64_t CityHash64(const char *s, uint32_t len);

typedef struct sha1_t {
    uint8_t bytes[20];
    
    bool operator<(const struct sha1_t &s1) const;
    bool operator==(const struct sha1_t &s1) const;
} sha1_t;
sha1_t sha1(const string& str);
