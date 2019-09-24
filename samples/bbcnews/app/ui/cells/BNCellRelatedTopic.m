//
//  BNCellRelatedTopic.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNCellContent.h"
#import "BNLabelInfo.h"
#import "BNRelationship.h"
#import "BNStyles.h"
#import "BNNavigationController.h"
#import "BNAManager.h"


@interface BNCellRelatedTopic : BNCellContent

@property (nonatomic) BNCollection* collection;
@property (nonatomic) BNLabelInfo* topicName;

@end

@implementation BNCellRelatedTopic


- (id)initWithRelationship:(BNRelationship*)relationship andModule:(BNCellsModule*)module {
	self = [super initWithModule:module];
	if (self) {
		self.collection = (BNCollection*)relationship.childObject;
		self.textAreaInsets = textInsetsRelatedTopic;
		BNItem* parentItem = (BNItem*)relationship.parentObject;
		self.inverseColorScheme = parentItem.isMediaItem;
	}
	return self;
}

- (void)postInit {
	[super postInit];
	self.topicName = [[BNLabelInfo alloc] initWithString:self.collection.name attributes:self.inverseColorScheme ? attrsRelatedTopicInv : attrsRelatedTopic numLines:1];
}

- (void)measureForContainingRect:(CGRect)rect {
	rect = UIEdgeInsetsInsetRect(rect, self.textAreaInsets);
	[self.topicName measureForWidth:rect.size.width offset:CGPointMake(self.textAreaInsets.left, self.textAreaInsets.top)];
	rect.size.height = self.topicName.bounds.size.height;
	rect = UIEdgeInsetsUninsetRect(rect, self.textAreaInsets);
	self.frameSize = rect.size;
}


- (void)createView:(UIView*)superview {
	[super createView:superview];
	[self.topicName createLabel:self.view];
	
	UIView* divider = [[UIView alloc] initWithFrame:CGRectMake(0,0,self.view.frame.size.width,1)];
	divider.autoresizingMask = UIViewAutoresizingFlexibleWidth;
	divider.backgroundColor = self.inverseColorScheme ? [UIColor thinDividerInvColor] : [UIColor thinDividerColor];
	[self.view addSubview:divider];
	[self.view setAccessibilityTraits:UIAccessibilityTraitButton];
}

- (void)deleteView {
	[self.topicName removeLabel];
	[super deleteView];
}

- (void)onTapped {
	BNNavigationController* navController = [BNNavigationController get];
	[navController openItem:self.collection withinCollection:nil andTitle:nil];
	
	
	[[BNAManager sharedInstance] setPageLoadInteraction:BNALoadInteractionValueTap referringObject:BNAReferringObjectValueFromRelatedTopicLink];
	[[BNAManager sharedInstance] userActionEventOfType:BNAActionTypeClick name:BNAActionNameRelatedTopicLink labels:nil];

}

#pragma mark - BNCellAccessibilityDelegate

- (NSString *)accessibilityLabel
{
	return self.topicName.string;
}

- (UIAccessibilityTraits)accessibilityTraits
{
	return UIAccessibilityTraitButton;
}

@end
