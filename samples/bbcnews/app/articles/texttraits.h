//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>

class BNTextTrait : public Object {
public:
    enum Type {
        Bold,
        Italic,
        Forecolor,
        FontScale,
        BaselineOffset,
        Link,
    } _type;
    uint32_t _start;
    uint32_t _end;

    COLOR _color;
    float _scale;
    float _distance; // gets scaled by font lineheight
// link
    string _platform;
    string _url;

    BNTextTrait(Type type, uint32_t start);
    BNTextTrait(Type type, uint32_t start, uint32_t end);

    virtual void applyToString(AttributedString* string);

};


//extern NSString* BNLinkAttributeName;


