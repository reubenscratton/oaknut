//
//  BNStaticTitleModuleTopic.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.

#import "BNContentTitleModule.h"
#import "BNStyles.h"
#import "BBCNFeatureController.h"
#import "BNNavigationController.h"


@implementation BNContentTitleModule

- (void)updateLayoutWithContentObject:(BNContent*)contentObject {
	self.contentId = contentObject.modelId;
	self.text = [NSString stringWithFormat:@"%@ ", contentObject.name];
	self.labelInfo = nil;
}

- (BOOL)showBackgroundForLinks {
	return YES;
}

- (void)onTitleTapped:(id)sender {

	if ([BBCNFeatureController tabBarEnabled]) {
		[[BNNavigationController get] showFollowedTopicsRibbonVC:self.contentId];
	} else {
		[super onTitleTapped:sender];
	}
}

@end