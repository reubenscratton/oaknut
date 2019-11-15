//
//  BNImageLinkModule.m
//  BBCNews
//
//  Copyright (c) 2015 BBC News. All rights reserved.
//

#import "BNImageLinkModule.h"
#import "BNStyles.h"
#import "BNPolicy.h"

#import "BNNavigationController.h"
#import "BNTabBarController.h"
#import "BBCNURLHandler.h"

static int s_sizes[5] = {320,375,414,768,1024};
static int s_blurbWidths[5] = {223,222,288,554,740};
static int s_blurbTops[5] = {122,144,146,156,170};
static int s_blurbHeights[5] = {80,80,62,48,48};


@interface BNImageLinkView : UIImageView

@property (nonatomic, strong) NSString *link;
@property (nonatomic) UIView* highlightView;
@property (nonatomic) UILabel* actionLabel;

@end

@implementation BNImageLinkView

- (id)initWithFrame:(CGRect)frame andImageIndex:(int)imageIndex forLink:(NSString *)link withBlurbText:(NSString *)blurb andActionText:(NSString *)action {
	self = [super initWithFrame:frame];
	if (self) {
		
		self.link = link;
		
		// Add the blurb label
		CGRect labelFrame = CGRectMake(0, s_blurbTops[imageIndex], s_blurbWidths[imageIndex], s_blurbHeights[imageIndex]);
		UILabel* blurbLabel = [[UILabel alloc] initWithFrame:labelFrame];
		//blurbLabel.backgroundColor = [UIColor colorWithRed:0.5f green:0.5f blue:0 alpha:0.25f];
		blurbLabel.textAlignment = NSTextAlignmentCenter;
		blurbLabel.numberOfLines = 0;
		blurbLabel.attributedText = [BNStyles attributedString:blurb attrs:attrsImageLinkBlurbLabel];
		[self addSubview:blurbLabel];
		
		// Add the action label
		labelFrame.origin.x = s_blurbWidths[imageIndex];
		labelFrame.size.width = frame.size.width - s_blurbWidths[imageIndex];
		self.actionLabel = [[UILabel alloc] initWithFrame:labelFrame];
		self.actionLabel.textAlignment = NSTextAlignmentCenter;
		self.actionLabel.attributedText = [BNStyles attributedString:action attrs:attrsImageLinkActionLabel];
		[self addSubview:self.actionLabel];
		if(self.link){
			self.userInteractionEnabled  = YES;
		}
	}
	return self;
}

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (UIAccessibilityTraits)accessibilityTraits
{
	return UIAccessibilityTraitButton;
}


- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	
	if(self.link){
		self.highlightView = [[UIView alloc] initWithFrame:self.bounds];
		self.highlightView.backgroundColor = [UIColor colorWithWhite:0.25f alpha:0.25f];
		[self addSubview:self.highlightView];
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	if(self.link){
		[self launchLink];
	}
	[self.highlightView removeFromSuperview];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	[self.highlightView removeFromSuperview];
}

- (void)didMoveToWindow {
	[super didMoveToWindow];
	
}

- (void)launchLink {
	
	NSURL* url = [NSURL URLWithString:self.link];
	BNNavigationController* navController = [BNNavigationController get];
	[navController openURL:url withinCollection:nil andTitle:nil andFlags:0];
}


@end


@interface BNImageLinkModule()

@property (nonatomic) int imageIndex;
@property (nonatomic) UIImage* image;
@property (nonatomic) UIImageView* imageView;

@property (nonatomic, strong) NSString *link, *imagePrefixName, *altText, *actionText, *callToActionText;
//@property (nonatomic) BOOL resetNav;


@end

@implementation BNImageLinkModule


- (id)initFromJson:(NSDictionary *)json {
	self = [super initFromJson:json];
	if (self) {
		self.link = json[@"contentLink"];
		self.imagePrefixName = json[@"imagePrefix"];
		self.altText = json[@"altText"];
		self.actionText = json[@"actionText"];
		self.callToActionText = json[@"callToActionText"];
		//self.resetNav = [json[@"resetNav"] boolValue];
	}
	return self;
}

- (id)copyWithZone:(NSZone *)zone {
	BNImageLinkModule* copy = [super copyWithZone:zone];
	copy.link = self.link;
	copy.imagePrefixName = self.imagePrefixName;
	copy.altText = self.altText;
	copy.actionText = self.actionText;
	copy.callToActionText = self.callToActionText;
	//copy.resetNav = self.resetNav;
	return copy;
}



- (void)layoutWithContainingRect:(CGRect)bounds {
	if (!self.image) {
		for (self.imageIndex=1 ; self.imageIndex<5 ; self.imageIndex++) {
			if (s_sizes[self.imageIndex] > bounds.size.width) {
				break;
			}
		}
		self.imageIndex--;
		

		
		self.image = [UIImage imageNamed:[NSString stringWithFormat:@"%@_%dpt",self.imagePrefixName , s_sizes[self.imageIndex]]];
	}
	bounds = UIEdgeInsetsInsetRect(bounds, self.padding);
	CGFloat imageAspect = self.image.size.width / self.image.size.height;
	self.frame = CGRectMake(bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.width / imageAspect);
}




- (void)updateSubviews:(UIView *)superview {
	if (CGRectIntersectsRect(self.frame, superview.bounds)) {
		if (!self.imageView) {
			self.imageView = [[BNImageLinkView alloc] initWithFrame:self.frame andImageIndex:self.imageIndex forLink:self.link withBlurbText:self.callToActionText andActionText:self.actionText];
			self.imageView.image = self.image;
			if (self.altText.length) {
				self.imageView.isAccessibilityElement = YES;
				self.imageView.accessibilityLabel = self.altText;
			} else {
				self.imageView.isAccessibilityElement = NO;
			}
			[superview addSubview:self.imageView];
		}
	} else {
		if (self.imageView) {
			[self.imageView removeFromSuperview];
			self.imageView = nil;
		}
	}
}

@end




