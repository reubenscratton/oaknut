//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

App app;

float App::dp(float dp) {
    return dp*_window->_scale;
}
float App::idp(float pix) {
    return pix/_window->_scale;
}

