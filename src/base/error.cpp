//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

error::error() : _code(0) {
}
error::error(int code, string msg/*=""*/) : _code(code), _msg(msg) {
}
error::error(const string& msg) : _code(-1), _msg(msg) {
}

error error::fromErrno() {
    int e = errno;
    if (!e) {
        return none();
    }
    return error(e, string(strerror(e), -1));
}

error error::none() {
    return error(0);
}
