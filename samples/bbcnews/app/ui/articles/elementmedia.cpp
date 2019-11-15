//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "elementmedia.h"



BNElementMedia::BNElementMedia(BNRelationship* relationship) {
    _media = relationship->_childObject;
    _mediaId = _media->_modelId;
}


void BNElementMedia::measureForContainingRect(const RECT& rect) {
    app->log("todo: BNElementMedia::measureForContainingRect");
	//[self.mediaCell measureForContainingRect:rect];
}

POINT BNElementMedia::getContentFrameOrigin() {
    app->log("todo");
    return POINT {0,0};
	//return _mediaCell->_frameOrigin;
}
void BNElementMedia::setContentFrameOrigin(POINT pt) {
    app->log("todo");
	//_mediaCell.frameOrigin = pt;
}
/*
- (CGRect)contentFrame {
	return self.mediaCell.frame;
}

- (BNCellMedia*)createMediaCell:(BNCellsModuleArticleBody*)module {
	self.mediaCell = [[BNCellMedia alloc] initWithModule:module];
	[self.mediaCell setMediaObject:self.media andContainingItem:module.item];
	[self.mediaCell postInit];
	return self.mediaCell;
}

- (void)updateSubviews:(UIView*)superview {
	// no need for anything here, views are supplied by BNCellsModule
}

- (UIView*)view {
	return self.mediaCell.view;
}

*/
/*

@implementation BNElementImage
- (BNImage*)image {
	return (BNImage*)self.media;
}
@end

@implementation BNElementAudio
- (BNAudio*)audio {
	return (BNAudio*)self.media;
}
@end

@implementation BNElementVideo
- (BNVideo*)video {
	return (BNVideo*)self.media;
}
@end

*/
