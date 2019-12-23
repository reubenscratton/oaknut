#if 0
//
//  BNElementAdvert.m
//  BBCNews
//
//  Copyright (c) 2015 BBC News. All rights reserved.
//

#import "BNElementAdvert.h"
#import "BNAdvertView.h"

@interface BNElementAdvert ()

@property (nonatomic) BNAdvertView* advertView;

@end


@implementation BNElementAdvert

- (id)initWithType:(NSString*)type {
	self = [super init];
	if (self) {
		self.advertView = [[BNAdvertView alloc] initWithType:type];
		self.advertView.showDividers = YES;
		self.normalizedWidth = 1;

	}
	return self;
}


- (void)measureForContainingRect:(CGRect)rect {
	[self.advertView layoutWithContainingRect:rect];
}

- (CGPoint)contentFrameOrigin {
	return self.advertView.frame.origin;
}
- (void)setContentFrameOrigin:(CGPoint)contentFrameOrigin {
	CGRect frame = self.advertView.frame;
	frame.origin = contentFrameOrigin;
	self.advertView.frame = frame;
}

- (CGRect)contentFrame {
	return self.advertView.frame;
}


- (void)updateSubviews:(UIView*)superview {
	if (CGRectIntersectsRect(self.frame, superview.bounds)) {
		if (!self.advertView.superview) {
			[self.advertView createSubviews:superview];
		}
	} else {
		if (self.advertView.superview) {
			[self.advertView removeSubviews];
		}
	}
}

- (UIView*)view {
	return self.advertView;
}

- (void)onTextKitDealloc {
	[self.advertView removeSubviews];
#ifndef UK
	self.advertView.adView = nil;
	self.advertView.advertisementLabel = nil;
#endif
	self.advertView = nil;
}

@end
#endif
