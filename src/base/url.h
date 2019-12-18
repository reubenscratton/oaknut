//
// Copyright © 2019 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

struct url {
    string scheme;
    string host;
    int port;
    string path;
    map<string, string> queryparams;
    
    url(const string& str);
};
