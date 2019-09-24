//
//  BNRedButtonModule.m
//  BBCNews
//
//  Copyright (c) 2015 BBC News. All rights reserved.
//

#import "BNRedButtonModule.h"
#import "BNStyles.h"
#import "BNFollowScreenViewController.h"

@interface BNRedButtonModule ()

@property (nonatomic) NSString* title;
@property (nonatomic) NSString* selector;
@property (nonatomic) UIButton* button;

@end


@implementation BNRedButtonModule

- (id)initFromJson:(NSDictionary *)json {
	self = [super initFromJson:json];
	if (self) {
		self.title = json[@"title"];
		self.selector = json[@"selector"];
	}
	return self;
}

- (id)copyWithZone:(NSZone *)zone {
	BNRedButtonModule* copy = [super copyWithZone:zone];
	copy.title = self.title;
	copy.selector= self.selector;
	return copy;
}

- (void)layoutWithContainingRect:(CGRect)bounds {
	bounds = UIEdgeInsetsInsetRect(bounds, self.padding);
	self.button = [UIButton buttonWithType:UIButtonTypeSystem];
	[self.button addTarget:self action:NSSelectorFromString(self.selector) forControlEvents:UIControlEventTouchUpInside];
	self.button.backgroundColor = [UIColor bbcNewsLiveRed];
	[self.button setAttributedTitle:[[NSAttributedString alloc] initWithString:self.title attributes:attrsRedButtonTitles] forState:UIControlStateNormal];
	[self.button sizeToFit];
	bounds.size.height = self.button.frame.size.height;
	self.button.frame = bounds;
	self.frame = UIEdgeInsetsUninsetRect(bounds, self.padding);
}


- (void)updateSubviews:(UIView *)superview {
	if (CGRectIntersectsRect(self.frame, superview.bounds)) {
		[superview addSubview:self.button];
	} else {
		if (self.button) {
			[self.button removeFromSuperview];
		}
	}
}

/**
 Button Actions
 */
- (void)onAddMoreTopicsPressed:(id)sender {
	[BNFollowScreenViewController showModal:0 fromButton:nil];
}
- (void)onManageTopicsPressed:(id)sender {
	[BNFollowScreenViewController showModal:1 fromButton:nil];
	
}


@end
