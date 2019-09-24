//
//  BNCellMedia.m
//  BBCNews
//
//  Copyright (c) 2014 British Broadcasting Corporation. All rights reserved.
//

#import "BNCellMedia.h"
#import "BNImageView.h"
#import "BNStyles.h"
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

@interface BNCellMedia ()


@property (nonatomic) BNImage* image;
@property (nonatomic) CGFloat imageAspect;
@property (nonatomic) BNItem* containingItem;
@property (nonatomic) BNAV* media;
@property (nonatomic) BNVideoView* videoView;
@property (nonatomic) BNLabelInfo* caption;
@property (nonatomic) BOOL parallax;
@property (nonatomic) NSString* secondaryType;
@property (nonatomic) BNImageView* imageView;

@end

@implementation BNCellMedia


- (void)adviseScrollviewOffset:(CGFloat)offset {
	if (self.parallax && !self.videoView.player) {
		CGFloat dy = MAX(offset, 0); // cos parallax in overscroll looks bad
		dy *= .7;
		CATransform3D transform = CATransform3DMakeTranslation(0, dy, 0);
		self.view.layer.transform = transform;
		//self.playButton.alpha = 1.0f - dy/200.0f;
	}
}


- (id)initWithRelationship:(BNRelationship*)relationship andModule:(BNCellsModule*)module {
	self = [super initWithModule:module];
	if (self) {
		[self setMediaObject:relationship.childObject andContainingItem:(BNItem*)relationship.parentObject];
		self.usesScrollviewOffset = YES;
		self.parallax = [module.json[@"parallax"] boolValue];
		self.secondaryType = relationship.secondaryType;
		self.imageAspect = [module.json[@"imageAspect"] floatValue];
	}
	return self;
}


- (void)setMediaObject:(BNBaseModel*)mediaObject andContainingItem:(BNItem*)containingItem {
	self.containingItem = containingItem;

	NSString* captionText = nil;
	if ([mediaObject isKindOfClass:[BNImage class]]) {
		self.image = (BNImage*)mediaObject;
		captionText = self.image.caption;
	} else if ([mediaObject isKindOfClass:[BNAV class]]) {
		self.media = (BNAV*)mediaObject;
		self.image = self.media.posterImage;
		captionText = self.media.caption;
	}
	
	if (captionText && ![self.module.json[@"hideCaption"] boolValue]) {
		self.caption = [[BNLabelInfo alloc] initWithString:captionText  attributes:attrsImageCaptions numLines:0];
	}
	
}

- (void)measureForContainingRect:(CGRect)rect {
	CGFloat height = 0.f;
	if (self.image != nil) {
		if (self.imageAspect > 0.f) {
			CGFloat actualImageAspect = self.image.height / (CGFloat)self.image.width;
			self.imageAspect = MIN(actualImageAspect, self.imageAspect);
			self.imageSize = CGSizeMake(rect.size.width, rect.size.width * self.imageAspect);
		} else {
			self.imageSize = [BNImageView expectedRenderSize:self.image forContainingSize:rect.size];
		}
		height = self.imageSize.height;
		
		if (self.caption != nil) {
			[self.caption measureForWidth:rect.size.width offset:CGPointMake(0,height)];
			height += self.caption.bounds.size.height;
		}
	}
	self.frameOrigin = rect.origin;
	self.frameSize = CGSizeMake(rect.size.width, height);
}

- (CGRect)bounds {
	return CGRectMake(0,0,self.frameSize.width, self.frameSize.height);
}

- (void)createView:(UIView *)superview {
	[super createView:superview];
	
	self.imageView = [[BNImageView alloc] initWithFrame:CGRectMake(0,0, self.imageSize.width, self.imageSize.height)];
	if (self.imageAspect > 0.f) {
		self.imageView.contentMode = UIViewContentModeScaleAspectFit;
	}
	[self.imageView setBNImage:self.image];
	self.imageView.userInteractionEnabled = NO;
	self.imageView.autoresizesSubviews = YES;
	[self.view addSubview:self.imageView];
	
	[self.caption createLabel:self.view];
	self.caption.label.backgroundColor = [self.module.json[@"inverseColorScheme"] boolValue] ?  [UIColor colorWithWhite:32/255.f alpha:1] : [UIColor colorWithWhite:247/255.f alpha:1];
	[self.view setIsAccessibilityElement: YES];
	
	if (self.media) {
		// Create the video overlay
		if (!self.videoView) {
			self.videoView = [BNVideoView createForMedia:self.media];
		}
		self.videoView.frame = self.imageView.bounds;
		[self.imageView addSubview:self.videoView];
		self.imageView.userInteractionEnabled = YES;
		self.imageView.isAccessibilityElement = YES;
		self.imageView.accessibilityLabel = self.media.isAudio ?
			NSLocalizedString(@"Audio player", nil):
			NSLocalizedString(@"Video player", nil);
		self.imageView.accessibilityTraits = UIAccessibilityTraitNone;
		self.caption.label.accessibilityLabel = self.accessibilityLabel;
	}
}

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



#pragma mark - UIAccessibility

- (BOOL)isAccessibilityElement {
	return self.videoView == nil && (self.caption != nil || (self.image != nil && self.image.altText != nil));
}

- (NSString *)accessibilityLabel {
	NSString *description = self.caption != nil ?  self.caption.string : self.image.altText;
	return [NSString stringWithFormat:@"Image, %@", description];
}

- (NSArray *)accessibleElements {
	if (self.videoView == nil) {
		return @[];
	}
	if (self.caption.label) {
		return @[self.videoView, self.imageView, self.caption.label];
	}
	return @[self.videoView, self.imageView];
}

@end
