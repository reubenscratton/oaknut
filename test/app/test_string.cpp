//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



void test_string() {
    
    string foo = "Hello world";
    assert(6 == foo.find("w"));
    assert(2 == foo.find("l"));
    assert(3 == foo.find("l", 3));
    assert(-1 == foo.find('x'));
    assert(-1 == string("").find('A'));
    auto sr = foo.split(" ");
    assert(sr[0]=="Hello");
    assert(sr[1]=="world");
    assert(1 == string("abcdefghijklmnopqrstuvwxyz").split("").size());
    assert(2 == string(" ").split(" ").size());
    assert(3 == string("  ").split(" ").size());
    assert(4 == string("   ").split(" ").size());
    

}
