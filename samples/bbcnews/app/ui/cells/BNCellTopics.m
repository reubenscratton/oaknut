//
//  BNCellTopics.m
//  BBCNews
//
//  Copyright (c) 2014 British Broadcasting Corporation. All rights reserved.
//

#import "BNStyles.h"
#import "BNItem.h"
#import "BNCell.h"
#import "BNTopicLabelInfo.h"

@interface BNCellTopics : BNCell

@property (nonatomic) BNTopicLabelInfo* labelInfo;

@end


@implementation BNCellTopics


- (id)initWithItem:(BNItem*)item andModule:(BNCellsModule*)module {
	self = [super initWithModule:module];
	if (self) {
		self.labelInfo = [[BNTopicLabelInfo alloc] initWithItem:item maxTopics:module.maxTopics inverseColorScheme:item.isMediaItem hideTimestamp:NO displayingIn:(BNCollection*)self.module.contentObject isLongTimestamp:[module.json[@"long_timestamp"] boolValue]];
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

- (BOOL)isAccessibilityElement
{
	return YES;
}

- (NSString *)accessibilityLabel {
	NSMutableString* s = self.labelInfo.string.mutableCopy;
	[s replaceOccurrencesOfString:@"|" withString:@"." options:NSLiteralSearch range:NSMakeRange(0, s.length)];
	return s;
}


@end
