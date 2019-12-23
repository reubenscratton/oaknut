#if 0
//
//  BNCellItemMostPopular.m
//  BBCNews
//
//  Copyright (c) 2014 British Broadcasting Corporation. All rights reserved.
//

#import "BNCellItemMostPopular.h"
#import "BNLayoutsGroupManager.h"
#import "BNStyles.h"


@implementation BNCellItemMostPopular

- (void)createView:(UIView*)superview {
	[super createView:superview];
	float cornerSize = cornerLabelSize;
	if ([BNLayoutsGroupManager sharedInstance].compactMode) {
		cornerSize *= .66f;
	}
	self.cornerLabel = [[BNCornerLabel alloc] initWithFrame:CGRectMake(0,0,cornerSize,cornerSize)];
	self.cornerLabel.attributedText = [[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@"%@", @(self.cellIndex+1)] attributes:attrsCornerLabels];
	[self.view addSubview:self.cornerLabel];
}

- (void)deleteView {
	[super deleteView];
	[self.cornerLabel removeFromSuperview];
	self.cornerLabel = nil;
}


-(NSString *)accessibilityLabel{

	NSMutableString* label = [NSMutableString stringWithFormat:@"%lu, %@",self.cellIndex+1,self.headlineText.mutableCopy];
	
	if (self.item.homedCollection) {
		
		[label appendString:@". "];
		[label appendString:self.item.homedCollection.name];
	}
	
	if (self.item.isMediaItem) {
		if (self.item.isVideo) {
			[label insertString:@". " atIndex:0];
			[label insertString:@"Video" atIndex:0];
		}
		
		if (self.item.isAudio) {
			[label insertString:@". " atIndex:0];
			[label insertString:@"Audio" atIndex:0];
		}
	}
	
	if (self.showSummary && self.summaryText) {
		[label appendString:@". "];
		[label appendString:self.summaryText];
	}
	
	return label;
}

@end

#endif



