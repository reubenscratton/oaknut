//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "cellitemtopstory.h"


static const float ITEM_WIDTH_IPHONE = 145; // todo: move to styles dict

class BNCellItemAV : public BNCellItemTopStory {
public:

    BNCellItemAV(BNCellsModule* module) : BNCellItemTopStory(module) {
        _rightSpec = LAYOUTSPEC::Abs(app->dp(ITEM_WIDTH_IPHONE));
    }


};

DECLARE_DYNCREATE(BNCellItemAV, BNCellsModule*);


