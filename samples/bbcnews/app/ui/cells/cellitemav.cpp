//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "cellitemtopstory.h"


static const float ITEM_WIDTH_IPHONE = 145; // todo: move to styles dict
static const float ITEM_WIDTH_IPAD = 230;

class BNCellItemAV : public BNCellItemTopStory {
public:

    BNCellItemAV(BNCellsModule* module) : BNCellItemTopStory(module) {
        _widthMeasureSpec = MEASURESPEC::Abs(app->dp(ITEM_WIDTH_IPHONE));
    }
    
/*    void measureForContainingRect(const RECT& arect) override {
        RECT rect = arect;
        rect.size.width = app->_defaultDisplay->sizeClass() == Display::Tablet ?
            ITEM_WIDTH_IPAD : ITEM_WIDTH_IPHONE;
        BNCellItemTopStory::measureForContainingRect(rect);
    }*/

};

DECLARE_DYNCREATE(BNCellItemAV, BNCellsModule*);


