//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include "element.h"
#include "textstyle.h"
#include "texttraits.h"

//
// This is the base class for any article element that can be wholly
// displayed in an NSAttributedString. It must be initialized with
// one of the named text styles from BNElementFormatter.
//
// It supports bold, italic, and link traits automatically.
//


class BNElementText : public BNElement {
public:
    string _styleName;
    style* _style;
    string _text;
    vector<BNTextTrait> _traits;
    uint32_t headIndentNumChars;

    BNElementText(const string& style);
    void trimTrailingWhitespace();
    AttributedString toAttributedString(COLOR textColor, EDGEINSETS padding);

    bool isElementText() override { return true; }

};
