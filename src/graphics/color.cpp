//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

COLOR COLOR::interpolate(COLOR start, COLOR end, float val) {
    uint8_t* c1 = (uint8_t*)&start;
    uint8_t* c2 = (uint8_t*)&end;
    uint8_t c3[4];
    c3[0] = c1[0] + (uint8_t)((c2[0]-c1[0]) * val);
    c3[1] = c1[1] + (uint8_t)((c2[1]-c1[1]) * val);
    c3[2] = c1[2] + (uint8_t)((c2[2]-c1[2]) * val);
    c3[3] = c1[3] + (uint8_t)((c2[3]-c1[3]) * val);
    return *(COLOR*)c3;
}
