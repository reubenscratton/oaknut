//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include "cell.h"

class BNCellContent : public BNCell {
public:
    BNCellContent(BNCellsModule* module);
    

    EDGEINSETS _textAreaInsets;
    bool _inverseColorScheme;

    virtual void onTapped();
    
    sp<View> _highlightView;
    
};
