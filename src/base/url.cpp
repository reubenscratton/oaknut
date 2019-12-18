//
// Copyright © 2019 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

url::url(const string& str) {
    uint32_t start = 0;
    uint32_t pos = str.find(":");
    if (pos < str.lengthInBytes()) {
        scheme = str.substr(start, pos);
        start = pos+1;
    }
    // Host[+port] specified?
    if (str.skipString(start, "//")) {
        host = str.readUpToOneOf(start, ":/");
        pos = host.find(":");
        if (pos < host.lengthInBytes()) {
            port = host.substr(pos+1).asInt();
            host = host.substr(0, pos);
        }
    }
    pos = str.find("?", 1, start);
    //uint32_t pathEnd = pos;
    if (pos < str.lengthInBytes()) {
        path = str.substr(start, pos);
        string query = str.substr(pos+1);
        auto pairs = query.split("&");
        for (auto pair : pairs) {
            auto bits = pair.split("=");
            auto key = bits[0].urlDecode();
            string value;
            if (bits.size() > 1) {
                value = bits[1].urlDecode();
            } else {
                value = "";
            }
            queryparams[key] = value;
        }
    } else {
        path = str.substr(start);
    }
}
