//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



void test_string() {

    // UTF-8 Encoding
    assert(string("").length() == 0);
    assert(string("a").length() == 1);
    assert(string("\u0081").length() == 1);
    assert(string("\u0081").lengthInBytes() == 2);
    assert(string("\u1a00").length() == 1);
    assert(string("\u1a00").lengthInBytes() == 3);
    assert(string("😀").lengthInBytes() == 4);
    assert(string("\u0081\u1a00").length() == 2);
    assert(string("\u0081\u1a00").lengthInBytes() == 5);
    assert(string("a\u0081\u1a00😀").lengthInBytes() == 10);

    // Character access
    //auto ptr = string("ABCDEF").begin();
    //assert(*ptr++ == 'A');
    //assert(*(ptr+2) == 'D');
    assert(string("ABCDEF").charAt(-1) == 'F');
    assert(string("ABCDEF").substr(3).charAt(1) == 'E');
    assert(string("ABCDEF").substr(100) == "");
    assert(string("a\u0081bc").charAt(-1) == 'c');
    
    // Substring modification
    string substr = string("ABC").substr(1, 2);
    assert(substr.length()==1);
    substr.append("C");
    assert(substr == "BC");

    // Find
    string foo = "Hello world";
    assert(6 == foo.find("w"));
    assert(2 == foo.find("l"));
    assert(3 == foo.find("l", 3));
    assert(foo.end() == foo.find("x"));
    //assert(-1 == string("").find('A'));
    
    // Split
    auto sr = string("Hello world").split(" ");
    assert(sr[0]=="Hello");
    assert(sr[1]=="world");
    assert(1 == string("abcdefghijklmnopqrstuvwxyz").split("").size());
    assert(1 == string(" ").split(" ").size());
    assert(2 == string("  ").split(" ").size());
    assert(4 == string("   ").split(" ").size());
    

}
