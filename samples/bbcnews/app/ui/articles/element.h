//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#import <oaknut.h>


class BNElement : public Object {
public:
    
    int charIndex;
    float normalizedLeft; // 0.0 = left edge, 0.5f = middle, etc
    float normalizedWidth;   // 0.5 = half width, 1.0 = full width
    float marginTop;
    float marginBottom;
    float marginLeft;
    float marginRight;

    virtual bool isElementText() { return false; }
    virtual bool isElementMedia() { return false; }
};
