//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "cellitem.h"
#include "../../common/durationlabel.h"

class BNCellItemDigest : public BNCellItem {
public:
    
    BNDurationLabel* _duration;



    BNCellItemDigest(BNCellsModule* module) : BNCellItem(module, BNCellStyle::Digest) {
	}

    
    void setItem(BNItem* item) override {
        BNCellItem::setItem(item);
        //if (!self.headline.numLines) {
        //	self.headline.numLines = IS_IPAD?2:1;
        //}
        if (_item->isMediaItem()) {
            _duration = new BNDurationLabel(_item, "durationTiny");
            _duration->setLayoutSize(MEASURESPEC::Match(_imageView), MEASURESPEC::Wrap());
            _duration->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Bottom());
            // triggers mad bug _imageView->addSubview(_duration);
        }
        

    }
/*
    void measureForContainingRect(const RECT& containingRect) override {
        RECT rect = containingRect;
        _frame.origin = rect.origin;
        rect.origin = {0,0};
        rect = rect.copyWithInsets(_module->_cellPadding);
        _imageOrigin = {_module->_cellPadding.left, _module->_cellPadding.top};
        
        RECT textFrame = rect;
        textFrame.origin.y += _module->_textPadding.top;
        textFrame.origin.x = _imageOrigin.x+_imageSize.width + _module->_textPadding.left;
        textFrame.size.width = rect.right() - textFrame.origin.x;
        //textFrame = UIEdgeInsetsInsetRect(textFrame, self.textAreaInsets);
        _headline->measureForWidth(textFrame.size.width, textFrame.origin);
        float bottom = _imageOrigin.y + _imageSize.height;
        rect.size.height = bottom - rect.origin.y;
        rect = rect.copyWithUninsets(_module->_cellPadding);
        _frame.size = rect.size;
    }*/


};

DECLARE_DYNCREATE(BNCellItemDigest, BNCellsModule*);


