//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


#ifndef ANDROID
void oak::log_v(const char* prefix, char const* fmt, va_list& args) {
    printf(prefix);
    vprintf(fmt, args);
    printf("\n");
}
#endif
