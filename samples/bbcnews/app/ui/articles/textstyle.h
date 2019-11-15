//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>

class BNTextStyle : public Object {
public:
    
    Font* _baseFont;
    float _lineHeight;
    EDGEINSETS _margins;

    float topAdjustment();
    float bottomAdjustment();

//    + (instancetype)styleWithFontName:(NSString *)fontName size:(CGFloat)size lineHeight:(CGFloat)lineHeight traits:(UIFontDescriptorSymbolicTraits)traits margins:(UIEdgeInsets)margins;

};


