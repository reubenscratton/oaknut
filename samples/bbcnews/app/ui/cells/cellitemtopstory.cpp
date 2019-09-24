//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "cellitemtopstory.h"
#include "../../app.h"
//#import "BNLiveEvent.h"
//#import "BNLayoutsGroupManager.h"
//#import "NSDate+Formatter.h"


DECLARE_DYNCREATE(BNCellItemTopStory, BNCellsModule*);

BNCellItemTopStory::BNCellItemTopStory(BNCellsModule* module) : BNCellItem(module) {
    auto textInsetsStyle = app->getStyle("text-insets");
    if (module->_H == 1) {
        _headline->applyStyle("H1");
        _textAreaInsets = textInsetsStyle->edgeInsetsVal("top-story");
        _orientation = kPortrait;
    } else if (module->_H == 2) {
         _headline->applyStyle("H2");
        _textAreaInsets = textInsetsStyle->edgeInsetsVal("secondary-top-story");
        _orientation = kLandscape;
    } else {
         _headline->applyStyle("H3");
        _textAreaInsets = textInsetsStyle->edgeInsetsVal("secondary-top-story");
        _orientation = kLandscape;
    }
    
    if (module->_textPadding != EDGEINSETS_Zero) {
        _textAreaInsets = module->_textPadding;
    }
    string orientation = module->_json.stringVal("orientation");
    if (orientation == "portrait") {
        _orientation = kPortrait;
    } else if (orientation == "landscape") {
        _orientation = kLandscape;
    } else if (orientation == "landscapeWithFullWidthHeadline") {
        _orientation = kLandscapeWithFullWidthHeadline;
    }
    
    setPadding(_module->_cellPadding);

    if (((BBCNewsApp*)app)->_compactMode && _summary) {
        _hideTimestamp = true;
        _hideTopics = true;
    }
    if (_module->_H != 1) {
        _headline->setMaxLines(_summary ? 0 : 3);
    }
}


void BNCellItemTopStory::setItem(BNItem* item) {
    BNCellItem::setItem(item);
	BOOL isLiveEvent = _item->isLiveEvent();
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

    
    
    if (_orientation == kLandscapeWithFullWidthHeadline) {
        setPadding(_textAreaInsets);
        _headline->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
        /*_imageOrigin.x = _textAreaInsets.left;
        _imageOrigin.y = _textAreaInsets.top + _headline->_bounds.size.height + _textAreaInsets.bottom;
        
        // Image fills left half (actually slightly less than half)
        float imageWidth = 0;
        if (_module->_imageWidthSpec <= 1) {
            imageWidth = (rect.size.width * _module->_imageWidthSpec) - _textAreaInsets.left/2;
        } else {
            imageWidth = _module->_imageWidthSpec;
        }
        float imageHeight = ceilf(imageWidth * 9.f/16.f);
        _imageSize = {imageWidth, imageHeight};
        
        
        float textLeft = _imageOrigin.x + imageWidth + _textAreaInsets.left;
        float textWidth = rect.size.width - textLeft;
        _topic->measureForWidth(textWidth, {0,0});
        RECT topicRect = _topic->_bounds;
        float bottom = _imageOrigin.y + _imageSize.height;
        if (_showSummary) {
            _summary->measureForWidth(textWidth, {textLeft, _imageOrigin.y - 6});
            bottom = MAX(bottom, _summary->_bounds.bottom() + (topicRect.size.height-6));
        }
        topicRect.origin = {textLeft - 5, bottom - (topicRect.size.height-6)};
        _topic->_bounds = topicRect;
        rect.size.height = bottom;
        rect = rect.copyWithUninsets(_textAreaInsets);
        _frame.size = rect.size;*/
    } else if (_orientation == kLandscape) {

        
        
        //_imageOrigin = {_module->_cellPadding.left, _module->_cellPadding.top};
        if (app->_defaultDisplay->sizeClass() == Display::Tablet) {
            if (_module->_imageWidthSpec <= 1) {
                _imageView->setLayoutSize({MEASURESPEC::TypeRelative,NULL,_module->_imageWidthSpec,  - _textAreaInsets.left/4}, MEASURESPEC::Aspect(9.f/16.f));
            } else {
                _imageView->setLayoutSize({MEASURESPEC::TypeRelative,NULL,_module->_imageWidthSpec,0}, MEASURESPEC::Aspect(9.f/16.f));
            }
            _imageView->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Aspect(9.f/16.f));

        } else {
            _imageView->setLayoutSize(MEASURESPEC::Abs(app->dp(144)), MEASURESPEC::Abs(app->dp(81)));
        }
        
        _textFrame->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Match(_imageView));
        _textFrame->setLayoutOrigin(ALIGNSPEC::ToRightOf(_imageView, 0),ALIGNSPEC::Top());

        // Image fills left half (actually slightly less than half)
        
        
        // Headline first
        _headline->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
        _headline->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Top());
        
        // Summary beneath headline
        /*if (_showSummary) {
            _summary->measureForWidth(textFrame.size.width, {textFrame.origin.x, bottom});
            bottom = _summary->_bounds.bottom();
        }*/
        
        // Topic at bottom
        if (_topic) {
            _topic->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Bottom());
        }
        /*
        // If bottom of topic is higher than bottom of image, shift topic down so they're aligned
        float dy = _imageSize.height - bottom;
        if (dy > 0) {
            _topic->_bounds.origin.y += dy;
            bottom += dy;
        } else {
            bottom += dy;
            
            // Force the summary to ellipsize rather than extend below the image
            if (_showSummary) {
                _summary->_bounds.size.height += dy;
            }
        }
        
        // Overall rect is original rect with image height
        rect.size.height = bottom;
        _frame.size = rect.size;*/
    } else {

        _imageView->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Aspect(9.f/16.f));
        _textFrame->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
        _textFrame->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Below(_imageView, 0));
        
        _headline->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Top());
        if (_summary) {
            _summary->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Below(_headline, 0));
        }
        if (_topic) {
            _topic->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Below(_headline, 0));
        }
        
    }
//    if (_isDummy) {
//        _dummyLabel->measureForWidth(rect.size.width, {0,0});
//        _dummyLabel->_bounds.origin.y = (rect.size.height - _dummyLabel->_bounds.size.height) / 2;
//    }
    
    Label* tinyTimestampLabel = NULL;
    if (_module->_tinyTimestamps && !_isDummy && !_hideTimestamp) {
        AttributedString age = "TODO"; //[NSDate tinyTimestampStringFromTimeStamp:_item->_lastUpdated);
        if (age.length()) {
            tinyTimestampLabel = new Label();
            age.applyStyle(app->getStyle("tinyTimestamps"));
            tinyTimestampLabel->setText(age);
            tinyTimestampLabel->sizeToFit();
            RECT frame = tinyTimestampLabel->getRect();
            frame.origin.x = (_frame.size.width - frame.size.width) - 8;
            frame.origin.y = (_frame.size.height - frame.size.height) - 4;
            tinyTimestampLabel->setRect(frame);
            
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


