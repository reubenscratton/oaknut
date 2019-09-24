//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "durationlabel.h"
#include "../model/_module.h"
//#import "NSDate+Formatter.h"




BNDurationLabel::BNDurationLabel(BNItem* item, const string& styleName) : BNLabel() {
    applyStyle(styleName);
    
    BNAV* av = NULL;
    auto avs = item->primaryAVs();
    if (avs.size()) {
        av = (BNAV*)avs[0];
    }
	bool isAudio = false;
	string timeLabel = " ";
	
	if (av) {
		isAudio = av->isAudio();
		_isLive = av->_isLive;
        timeLabel = "TODO";//[NSDate durationStringFromTimeIntervalNumber:av.duration];
	} else {
		isAudio = item->isAudio();
		_isLive = false;
	}

	if (!_isLive) {
        setText(timeLabel);
        //mediaGlyph:isAudio?kAudio:kVideo];
    }
	
    if (item->_modelId == BNModelIdLive){
		_isLive = false;
	}
}

/*
void BNDurationLabelInfo::measureForWidth(float width, POINT offset) {
	if (_isLive) {
        //_bounds = {offset.x, offset.y, (float)s_liveIcon->_width, (float)s_liveIcon->_height};
        _bounds = {offset.x, offset.y, 32.f,32.f}; // TODO
        return;
	}
    BNLabelInfo::measureForWidth(width, offset);
}


void BNDurationLabelInfo::createLabel(View* superview) {
	if (_isLive) {
        _imageView = new ImageView();
        _imageView->setRect(_bounds);
        _imageView->setImageAsset("images/live.png");
		superview->addSubview(_imageView);
		return;
	}
	
    BNLabelInfo::createLabel(superview);
	_label->setBackgroundColor(app->getStyleColor("color.contentBackgroundInv"));
}

void BNDurationLabelInfo::removeLabel() {
    BNLabelInfo::removeLabel();
    _imageView->removeFromParent();
    _imageView = NULL;
}

*/
