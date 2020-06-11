//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "cellitemtopstory.h"
//#import "BNLiveEvent.h"
//#import "BNLayoutsGroupManager.h"
//#import "NSDate+Formatter.h"


DECLARE_DYNCREATE(BNCellItemTopStory, BNCellsModule*);

BNCellItemTopStory::BNCellItemTopStory(BNCellsModule* module) : BNCellItem(module, BNCellStyle::TopStory) {
}


void BNCellItemTopStory::setItem(BNItem* item) {
    BNCellItem::setItem(item);
	bool isLiveEvent = _item->isLiveEvent();
    if (isLiveEvent) {
        _hideTimestamp = true;
    }
	if (_item->isMediaItem() || isLiveEvent) {
		_duration = new BNDurationLabel(_item, "durationSmall");
		if (isLiveEvent) {
			_duration->_isLive = true;
		}
        _imageView->addSubview(_duration);
	}

//    if (_isDummy) {
//        _dummyLabel->measureForWidth(rect.size.width, {0,0});
//        _dummyLabel->_bounds.origin.y = (rect.size.height - _dummyLabel->_bounds.size.height) / 2;
//    }
    
    Label* tinyTimestampLabel = NULL;
    if (_module->_tinyTimestamps && !_isDummy && !_hideTimestamp) {
        attributed_string age = "TODO"; //[NSDate tinyTimestampStringFromTimeStamp:_item->_lastUpdated);
        if (age.length()) {
            tinyTimestampLabel = new Label();
            age.applyStyle(app->getStyle("tinyTimestamps"));
            tinyTimestampLabel->setText(age);
            tinyTimestampLabel->sizeToFit();
            RECT frame = tinyTimestampLabel->getRect();
            frame.origin.x = (_frame.size.width - frame.size.width) - 8;
            frame.origin.y = (_frame.size.height - frame.size.height) - 4;
            tinyTimestampLabel->setLayoutRect(frame);
            
            _headline->_bottomRightExclusionSize = frame.size;
        }
    }
    if (tinyTimestampLabel) {
        addSubview(tinyTimestampLabel);
    }
}


/*
void BNCellItemTopStory::extendToHeight(float height) {
	float dy = height - _frame.size.height;
	if (dy>0) {
        BNCellItem::extendToHeight(height);
        _topic->adjustFrame({0, dy});
	}
}
*/


//- (BOOL)isAccessibilityElement {
//	return YES;
//}


