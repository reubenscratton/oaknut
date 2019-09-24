//
//  BNCellItemRelatedStory.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNCellItemTopStory.h"
#import "BNAManager.h"


@interface BNCellItemRelatedStory : BNCellItemTopStory

@end
@implementation BNCellItemRelatedStory

- (id)initWithRelationship:(BNRelationship*)relationship andModule:(BNCellsModule*)module {
	self = [super initWithRelationship:relationship andModule:module];
	if (self) {
		BNItem* parentItem = (BNItem*)relationship.parentObject;
		self.inverseColorScheme = parentItem.isMediaItem;
		self.orientation = kLandscape;
		self.headlineAttrs = self.inverseColorScheme ? attrsH3Inv : attrsH3;
	}
	return self;
}

- (void)measureForContainingRect:(CGRect)rect {
	[super measureForContainingRect:rect];
	
	// Add a vertical margin to top and bottom
	CGFloat vmargin = IS_IPAD ? 16 : 12;
	self.imageOrigin = CGPointMake(self.imageOrigin.x, self.imageOrigin.y+vmargin);
	self.headline.bounds = CGRectOffset(self.headline.bounds, 0, vmargin);
	self.topic.bounds = CGRectOffset(self.topic.bounds, 0, vmargin);
	self.frameSize = CGSizeMake(self.frameSize.width, self.frameSize.height+vmargin*2);
}


- (void)createView:(UIView*)superview {
	[super createView:superview];
	UIView* divider = [[UIView alloc] initWithFrame:CGRectMake(0,0,self.frameSize.width,1)];
	divider.autoresizingMask = UIViewAutoresizingFlexibleWidth;
	divider.backgroundColor = self.inverseColorScheme ? [UIColor thinDividerInvColor] : [UIColor thinDividerColor];
	[self.view addSubview:divider];
}

- (void)onTapped
{
	[super onTapped];
	//echo needs view topic first
	[[BNAManager sharedInstance] setPageLoadInteraction:BNALoadInteractionValueTap referringObject:BNAReferringObjectValueFromRelatedStoryLink];
	
	[[BNAManager sharedInstance] userActionEventOfType:BNAActionTypeClick name:BNAActionNameRelatedStoryLink labels:nil];

}

#pragma mark - BNCellAccessibilityDelegate

- (UIAccessibilityTraits)accessibilityTraits {
	return UIAccessibilityTraitButton;
}

@end