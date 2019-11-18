//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



void test_string() {
    
    string foobar("foosbar");
    
    // UTF-8 Encoding
    assert(""_S.length() == 0);
    assert("a"_S.length() == 1);
    assert("\u0081"_S.length() == 1);
    assert("\u0081"_S.lengthInBytes() == 2);
    assert("\u1a00"_S.length() == 1);
    assert("\u1a00"_S.lengthInBytes() == 3);
    assert("😀"_S.lengthInBytes() == 4);
    assert("\u0081\u1a00"_S.length() == 2);
    assert("\u0081\u1a00"_S.lengthInBytes() == 5);
    assert("a\u0081\u1a00😀"_S.lengthInBytes() == 10);

    // Character access
    //auto ptr = string("ABCDEF").begin();
    //assert(*ptr++ == 'A');
    //assert(*(ptr+2) == 'D');
    assert("ABCDEF"_S.charAt(-1) == 'F');
    assert("ABCDEF"_S.substr(3).charAt(1) == 'E');
    assert("ABCDEF"_S.substr(100) == "");
    assert("a\u0081bc"_S.charAt(-1) == 'c');
    
    // Substring modification
    string substr = "ABC"_S.substr(1, 2);
    assert(substr.length()==1);
    substr.append("C"_S);
    assert(substr == "BC"_S);

    // Find
    string foo = "Hello world"_S;
    assert(6 == foo.find("w"));
    assert(2 == foo.find("l"));
    assert(3 == foo.find("l", 1, 3));
    assert(foo.end() == foo.find("x"));
    //assert(-1 == string("").find('A'));
    
    // Split
    auto sr = "Hello world"_S.split(" ");
    assert(sr[0]=="Hello"_S);
    assert(sr[1]=="world"_S);
    assert(1 == "abcdefghijklmnopqrstuvwxyz"_S.split(""_S).size());
    assert(1 == string(" ").split(" ").size());
    assert(2 == string("  ").split(" ").size());
    assert(3 == string("   ").split(" ").size());
    

}
