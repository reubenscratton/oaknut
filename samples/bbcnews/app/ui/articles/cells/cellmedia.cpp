//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "cellmedia.h"
/*#import "BNStyles.h"
#import "BNImage.h"
#import "BNAV.h"
#import "BNRelationship.h"
#import "BNVideo.h"
#import "BNLabelinfo.h"
#import "BNVideoView.h"
#import "BNNavigationController.h"
#import "BNVideoView.h"
#import "BNAManager.h"
#import "BNURLRequestManager.h"
*/


DECLARE_DYNCREATE(BNCellMedia, BNCellsModule*);

BNCellMedia::BNCellMedia(BNCellsModule* module) : BNCellContent(module) {
    _parallax = module->_json.boolVal("parallax");
    _imageAspect = module->_json.floatVal("imageAspect");
    
    _imageView = new BNImageView();
    addSubview(_imageView);
    
    if (!module->_json.boolVal("hideCaption")) {
        _caption = new BNLabel();
        _caption->setLayoutSize(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
        _caption->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Bottom());
        //    _caption.label.backgroundColor = [self.module.json[@"inverseColorScheme"] boolValue] ?  [UIColor colorWithWhite:32/255.f alpha:1] : [UIColor colorWithWhite:247/255.f alpha:1];
        addSubview(_caption);
    }
    /*
    if (self.media) {
        // Create the video overlay
        if (!self.videoView) {
            self.videoView = [BNVideoView createForMedia:self.media];
        }
        self.videoView.frame = self.imageView.bounds;
        [self.imageView addSubview:self.videoView];
    }*/

}

void BNCellMedia::setRelationship(BNRelationship* rel) {
    setMediaObject(rel->_childObject, (BNItem*)rel->_parentObject);
    _usesScrollviewOffset = YES;
    _secondaryType = rel->_secondaryType;
}

void BNCellMedia::setMediaObject(BNBaseModel* mediaObject, BNItem* containingItem) {
    _containingItem = containingItem;

    string captionText;
    if (mediaObject->isImage()) {
        _image = (BNImage*)mediaObject;
        _caption->setText(_image->_caption);
    } else if (mediaObject->isAV()) {
        _media = (BNAV*)mediaObject;
        _image = _media->posterImage();
        _caption->setText(_media->_caption);
    }
    
    if (_image) {
        float actualImageAspect = _image->_height / (float)_image->_width;
        if (_imageAspect > 0.f) {
            _imageAspect = MIN(actualImageAspect, _imageAspect);
            _imageView->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Aspect(_imageAspect));
        } else {
            _imageView->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Aspect(actualImageAspect));
        }

        _imageView->setBNImage(_image);
    }
    //if (self.imageAspect > 0.f) {
    //    self.imageView.contentMode = UIViewContentModeScaleAspectFit;
    //}
    
    
}

/*
void BNCellMedia::adviseScrollviewOffset(float offset) {
	if (_parallax && !_videoView.player) {
		float dy = MAX(offset, 0); // cos parallax in overscroll looks bad
		dy *= .7;
		// CATransform3D transform = CATransform3DMakeTranslation(0, dy, 0);
		// self.view.layer.transform = transform;
	}
}*/




/*
- (BOOL)shouldAutoPlay {
	
	if (self.media.guidance) {
		return NO;
	}
	
	NSInteger autoPlaySetting = [[[NSUserDefaults standardUserDefaults] valueForKey:@"autoPlaySetting"] integerValue];
	
	if (autoPlaySetting == 0) { // never auto-play
		return NO;
	}
	
	if (autoPlaySetting == 1) { // only on WiFi
		return internetStatus == ReachableViaWiFi;
	}
	
	if (autoPlaySetting == 2) { // on radio.
		return internetStatus == ReachableViaWWAN || ReachableViaWiFi;
	}
	
	return NO;

}

- (void)onIsInitialContent {
	
	if ([self shouldAutoPlay]) {
		[self.videoView onPlayButtonPressed:nil];
	}
}

- (void)deleteView {
	[self.caption removeLabel];
	if (self.videoView) {
		[self.videoView removeFromSuperview];
	}
	[self.imageView removeFromSuperview];
	self.imageView = nil;
	[super deleteView];
}

- (void)onTapped {
	if (self.videoView) {
		[self.videoView onPlayButtonPressed:nil];
	} else {
		BNNavigationController* navController = [BNNavigationController get];
		NSArray* images = nil;
		if ([self.secondaryType isEqualToString:BNRelationshipType.placementPhotoGallery]) {
			images = self.containingItem.photoGalleryImages;
		} else {
			images = self.containingItem.itemPictureGalleryImages;
		}
		[navController openPhotoGallery:images withCurrentImage:self.image];
	}
	
	[[BNAManager sharedInstance] setPageLoadInteraction:BNALoadInteractionValueTap referringObject:BNAReferringObjectValueFromRelatedStoryLink];
	[[BNAManager sharedInstance] userActionEventOfType:BNAActionTypeClick name:BNAActionNameRelatedTopicLink labels:nil];

}


- (void)setIsOnScreen:(BOOL)isOnScreen {
	self.videoView.isOnScreen = isOnScreen;
	[super setIsOnScreen:isOnScreen];
}

- (void)onSuperviewDidAppear:(BOOL)viewControllerIsMovingToParent {
	[self.videoView onSuperviewDidAppear:viewControllerIsMovingToParent];
}

- (void)onSuperviewDidDisappear:(BOOL)viewControllerIsMovingFromParent {
	[self.videoView onSuperviewDidDisappear:viewControllerIsMovingFromParent];
}

*/
