//
//  BNModuleTitleDigest.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNModuleTitleDigest.h"
#import "BNStyles.h"

@interface BNModuleTitleDigest ()
@property (nonatomic) CGRect labelFrame;
@property (nonatomic) UIView* labelHolderView;
@end


@implementation BNModuleTitleDigest

- (id)initFromJson:(NSDictionary *)json {
	self = [super initFromJson:json];
	if (self) {
		// Use a different default style
		if (self.attrs == attrsTitleModule) {
			self.attrs = attrsDigestTitles;
		}
	
		// Append some spaces to the label text to work around UIButton ignoring of spacing in NSParagraphStyle
		self.text = [NSString stringWithFormat:@"%@  ", self.text];
	}
	return self;
}

- (BOOL)showBackgroundForLinks {
	return NO;
}

- (void)layoutWithContainingRect:(CGRect)bounds {
	[super layoutWithContainingRect:bounds];
	self.labelFrame = self.labelInfo.bounds;
	CGRect rect = self.labelInfo.bounds;
	rect.origin.y = self.textPadding.top;//-= self.frame.origin.y;
	rect.origin.x = self.textPadding.left;//-= self.frame.origin.x;
	self.labelInfo.bounds = rect;
}


- (void)createLabelView:(UIView*)superview {
	self.labelHolderView = [[UIView alloc] initWithFrame:self.labelFrame];
	self.labelHolderView.backgroundColor = [UIColor contentBackgroundColor];
	[superview addSubview:self.labelHolderView];
	[super createLabelView:self.labelHolderView];
}

- (void)removeLabelView {
	[super removeLabelView];
	[self.labelHolderView removeFromSuperview];
	self.labelHolderView = nil;
}


@end
