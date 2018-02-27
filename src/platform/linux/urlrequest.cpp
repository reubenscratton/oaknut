//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_LINUX

#include "oaknut.h"
#include <gio/gio.h>

void URLRequest::nativeStart() {
    retain(); // ensure request is alive until it completes, can't count on anything else to keep us alive
    oakLog("todo! URLRequest::nativeStart");
}

void URLRequest::nativeStop() {
    oakLog("todo! URLRequest::nativeStop");
}


#endif