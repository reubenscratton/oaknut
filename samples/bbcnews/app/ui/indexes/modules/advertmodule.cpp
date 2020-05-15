//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "advertmodule.h"
//#import "BNAdvertView.h"
//#import "BNStyles.h"


DECLARE_DYNCREATE(BNAdvertModule, const variant&);

BNAdvertModule::BNAdvertModule(const variant& json) : BNModule(json) {
    log_info("TODO: Finish BNAdvertModule");
    //if ([BNAdManager sharedInstance].bannerAdsEnabled) {
    //    self.parallax = [json[@"parallax"] boolValue];
    //}
}

BNAdvertModule::BNAdvertModule(BNAdvertModule* source) : BNModule(source) {
    _parallax = source->_parallax;
}

BNModule* BNAdvertModule::clone() {
    return new BNAdvertModule(this);
}

/*
- (void)updateLayoutWithContentObject:(BNContent*)contentObject {
	[super updateLayoutWithContentObject:contentObject];
	self.contentAllowsAdvertising = !contentObject.disallowAdvertising;
}

- (void)adviseScrollviewOffset:(CGFloat)offset {
	if (self.parallax) {
		CGFloat dy = MAX(offset, 0); // cos parallax in overscroll looks bad
		dy *= .7;
		CATransform3D transform = CATransform3DMakeTranslation(0, dy, 0);
		self.advertView.layer.transform = transform;
	}
}

- (void)layoutWithContainingRect:(CGRect)bounds {
	if ([BNAdManager sharedInstance].bannerAdsEnabled && self.contentAllowsAdvertising) {
		bounds = UIEdgeInsetsInsetRect(bounds, self.padding);
		if (!self.advertView) {
			self.advertView = [[BNAdvertView alloc] initWithType:self.json[@"type"]];
		}
		bounds = [self.advertView layoutWithContainingRect:bounds];
		self.frame = UIEdgeInsetsUninsetRect(bounds, self.padding);
	} else {
		self.frame = CGRectZero;
	}
}



- (void)updateSubviews:(UIView*)superview {
	
	// Adjust frame for when parallax effect is in use (i.e. so ad isn't removed while it's still visible)
	CGRect frame = self.frame;
	if (self.parallax) {
		frame.size.height *=3;
	}
	
	if (CGRectIntersectsRect(frame, superview.bounds)) {
		if (!self.advertView.superview) {
			[self.advertView createSubviews:superview];
		}
		
		if ([superview isKindOfClass:[UIScrollView class]]) {
			UIScrollView* scrollView = (UIScrollView*)superview;
			[self adviseScrollviewOffset:scrollView.contentOffset.y + scrollView.contentInset.top];
		}

	} else {
		[self removeAllViews];
	}
}

- (void)removeAllViews {
#ifndef UK
	if (self.advertView.adView) {
		[self.advertView removeSubviews];
	}
#endif
	[super removeAllViews];
}


 */
