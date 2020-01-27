//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class error {
public:
    error();
    error(int code, string msg="");
    error(const string& msg);
    
    operator int() const { return _code; }
    
    int _code;
    string _msg;
    
    static error none();
    static error fromErrno();
};
