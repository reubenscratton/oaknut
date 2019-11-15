//
//  BNCellArticleText.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNCellArticletext.h"
#import "BNItem.h"
#import "BNElementText.h"
#import "BNStyles.h"
#import "BNCellsModuleArticleBody.h"


@implementation BNCellArticleText


- (id)initWithItem:(BNItem*)item andModule:(BNCellsModule *)module {
	self = [super initWithModule:module];
	if (self) {
		self.textView = [[BNTextView alloc] initWithFrame:CGRectZero];
		self.textView.backgroundColor = item.isMediaItem ? [UIColor contentBackgroundInvColor] : [UIColor contentBackgroundColor];
		self.textView.tintColor = [UIColor bbcNewsLiveRed];
		BNCellsModuleArticleBody* moduleArticleBody = (BNCellsModuleArticleBody*)module;
		self.textView.item = item;
		[self.textView setAttributedText:moduleArticleBody.attributedText andNonTextElements:moduleArticleBody.nontextElements];
		self.usesScrollviewOffset = YES;
	}
    return self;

}

- (void)measureForContainingRect:(CGRect)rect {
	self.frameSize = [self.textView sizeThatFits:rect.size];
	self.frameOrigin = rect.origin;
}

- (void)createView:(UIView *)superview {
	[super createView:superview];
	self.textView.layer.transform = CATransform3DIdentity;
	self.textView.contentOffset = CGPointZero;
	self.textView.frame = self.view.bounds;
	[self.view addSubview:self.textView];
}

- (void)deleteView {
	[self.textView removeFromSuperview];
	[super deleteView];
}


- (void)adviseScrollviewOffset:(CGFloat)offset {

	// Displace the apparent position of our UITextView so it perfectly eclipses the visible portion
	// of the cell (i.e. self). The cell size is the same as the article size (i.e. potentially very
	// tall) but the UITextView is only as large as the visible region of the cell. This is the heart
	// of our workaround of UITextViews 8K height limit (see http://stackoverflow.com/a/24726113/440602)
	offset -= (self.frameOrigin.y + navbarHeight);
	if (offset<0) {
		offset=0;
	}
	CATransform3D transform = CATransform3DMakeTranslation(0, offset, 0);
	self.textView.layer.transform = transform;
	self.textView.contentOffset = CGPointMake(0,offset);
	//NSLog(@"tv scroll %f", offset);
}


