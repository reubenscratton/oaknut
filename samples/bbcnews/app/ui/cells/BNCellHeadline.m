//
//  BNCellHeadline.m
//  BBCNews
//
//  Copyright (c) 2014 British Broadcasting Corporation. All rights reserved.
//

#import "BNStyles.h"
#import "BNItem.h"
#import "BNCell.h"
#import "BNLabelInfo.h"

@interface BNCellHeadline : BNCell

@property (nonatomic) BNLabelInfo* labelInfo;
//@property (nonatomic) BNItem* item;

@end


@implementation BNCellHeadline


- (id)initWithItem:(BNItem*)item andModule:(BNCellsModule *)module {
	self = [super initWithModule:module];
	if (self) {
		BOOL invColor = item.isMediaItem || [module.json[@"inverseColorScheme"] boolValue];
		self.labelInfo = [[BNLabelInfo alloc] initWithString:item.name attributes:invColor ? attrsH1Inv : attrsH1 numLines:0];
		self.labelInfo.useDynamicText = YES;
	}
	return self;
}

- (void)measureForContainingRect:(CGRect)rect {
	rect = UIEdgeInsetsInsetRect(rect, self.module.textPadding);
	[self.labelInfo measureForWidth:rect.size.width offset:CGPointMake(self.module.textPadding.left, self.module.textPadding.top)];
	rect.size.height = self.labelInfo.bounds.size.height;
	rect = UIEdgeInsetsUninsetRect(rect, self.module.textPadding);
	self.frameSize = rect.size;
}

- (void)createView:(UIView *)superview {
	[super createView:superview];
	[self.labelInfo createLabel:self.view];
}

- (void)deleteView {
	[self.labelInfo removeLabel];
	[super deleteView];
}

#pragma mark - BNCellAccessibilityDelegate

- (NSString *)accessibilityIdentifier {
    return @"bn-cell-headline";
}

- (BOOL)isAccessibilityElement {
    return YES;
}

- (NSString *)accessibilityLabel {
	return self.labelInfo.string;
}

- (UIAccessibilityTraits)accessibilityTraits {
	return UIAccessibilityTraitHeader;
}


@end
