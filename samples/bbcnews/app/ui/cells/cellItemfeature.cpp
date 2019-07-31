//
//  BNCellItemFeature.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNCellItem.h"
#import "BNStyles.h"

@interface BNCellItemFeature : BNCellItem

@property (nonatomic) CGRect accentFrame;
@property (nonatomic) UIColor* featureColor;
@property (nonatomic) UIView* accentView;
@property (nonatomic) NSString* imagePos;
@end


@implementation BNCellItemFeature


- (id)initWithRelationship:(BNRelationship*)relationship andModule:(BNCellsModule*)module {
	self = [super initWithRelationship:relationship andModule:module];
	if (self) {
		
		if (module.cellsPerRow > 1) {
			self.headlineAttrs = self.inverseColorScheme ? attrsFeatureHeadlineInv : attrsFeatureHeadline;
		} else {
			self.headlineAttrs = self.inverseColorScheme ? attrsFullWidthFeatureHeadlineInv : attrsFullWidthFeatureHeadline;
		}
		
		self.textAreaInsets = textInsetsFeature;
		self.showSummary = module.summaries;
		
		self.showMediaGlyphInHeadline = YES;
		self.hideTimestamp = YES;
		self.imagePos = module.json[@"imagePosition"];

		// TODO: Magazine will eventually have it's own relationship secondaryType. When it does, replace this.
		if ([self.item.modelId hasPrefix:modelIdMagazine]) {
			self.featureColor = [UIColor featureColorMagazine];
		} else if ([self.item.site isEqualToString:BNContentSite.sport]) {
			self.featureColor = [UIColor featureColorSport];
		} else if ([relationship.secondaryType isEqualToString:BNRelationshipType.groupFeatureAnalysis]) {
			self.featureColor = [UIColor featureColorAnalysis];
		} else if ([relationship.secondaryType isEqualToString:BNRelationshipType.groupAlsoInTheNews]) {
			self.featureColor = [UIColor bbcNewsLiveRed];
		} else {
			self.featureColor = [UIColor featureColorFeatures];
		}

	}
	return self;
}

- (void)postInit {
	[super postInit];
	if (IS_IPHONE) {
	//self.summary.numLines = 3;
	}
}



- (void)measureForContainingRect:(CGRect)rect {

	rect.origin = CGPointZero;
	CGFloat imageWidth = rect.size.width;
	CGFloat imageHeight = 0;
	CGRect textArea = rect;
	CGFloat minimumHeight = 0.f;
	if ([self.imagePos isEqualToString:@"top"]) {
		imageHeight = (imageWidth * self.imageAspect);
		textArea.origin.y += imageHeight;
	} else if ([self.imagePos isEqualToString:@"left"] || [self.imagePos isEqualToString:@"right"]) {
		imageWidth *= self.module.imageWidthSpec;
		imageHeight = (imageWidth * self.imageAspect);
		textArea.size.width = rect.size.width - imageWidth;
		minimumHeight = imageHeight;
		if ([self.imagePos isEqualToString:@"left"]) {
			textArea.origin.x += imageWidth;
		} else {
			self.imageOrigin = CGPointMake(rect.size.width - imageWidth, 0);
		}
	}
	self.imageSize = CGSizeMake(imageWidth, imageHeight);
	
	self.accentFrame = CGRectMake(textArea.origin.x, textArea.origin.y, textArea.size.width, featureAccentStripHeight);
	//textArea.origin.y += featureAccentStripHeight;
	
	textArea = UIEdgeInsetsInsetRect(textArea, self.textAreaInsets);
	
	if (self.item.homedCollection && !self.hideTopics) {
		[self.topic measureForWidth:textArea.size.width offset:textArea.origin];
		textArea.origin.y = CGRectGetMaxY(self.topic.bounds) + 4;
	} else {
		textArea.origin.y = textArea.origin.y + 8;
	}
	
	[self.headline measureForWidth:textArea.size.width offset:textArea.origin];
	textArea.origin.y = CGRectGetMaxY(self.headline.bounds) + 2;
	
	[self.summary measureForWidth:textArea.size.width offset:textArea.origin];
	
	CGFloat textBottom;
 
	if(!self.showSummary){
		textBottom = CGRectGetMaxY(self.headline.bounds) + self.textAreaInsets.bottom;
	} else {
		textBottom = CGRectGetMaxY(self.summary.bounds) + self.textAreaInsets.bottom;
	}
	
	CGFloat frameHeight = MAX(minimumHeight, textBottom);
	
	self.frameSize = CGSizeMake(rect.size.width, frameHeight);

}

- (void)createView:(UIView*)superview {
	[super createView:superview];
	self.accentView = [[UIView alloc] initWithFrame:self.accentFrame];
	self.accentView.backgroundColor = self.featureColor;
	[self.view addSubview:self.accentView];
}

- (void)deleteView {
	[self.accentView removeFromSuperview];
	self.accentView = nil;
	[super deleteView];
}

@end


@interface BNCellItemFeatureLarge : BNCellItemFeature

@property (nonatomic) UIView* textFrameBackgroundView;

@end


#define FEATURE_LARGE_ALPHA 0.9f

@implementation BNCellItemFeatureLarge

- (id)initWithRelationship:(BNRelationship*)relationship andModule:(BNCellsModule*)module {
	self = [super initWithRelationship:relationship andModule:module];
	if (self) {
		self.imagePos = @"top";
	}
	return self;
}

- (void)measureForContainingRect:(CGRect)rect {
	CGFloat origWidth = rect.size.width;
	rect.size.width = ((rect.size.width) / 2) - 8;
	[super measureForContainingRect:rect];
	rect.size.width = origWidth;
	self.imageSize = CGSizeMake(rect.size.width, rect.size.width*9.f/16.f);
	self.frameSize = self.imageSize;
	CGFloat dy = self.accentFrame.origin.y - 180; // TODO: move this constant into layout
	[self adjustTextFrame: dy];
}

- (void)adjustTextFrame:(CGFloat)dy {
	CGPoint delta = CGPointMake(0, dy);
	[self.summary adjustFrame:delta];
	[self.headline adjustFrame:delta];
	[self.topic adjustFrame:delta];
	CGRect accentFrame = self.accentFrame;
	accentFrame.origin.y += delta.y;
	self.accentFrame = accentFrame;
}

- (void)createView:(UIView*)superview {
	[super createView:superview];
	CGRect textFrame = self.accentFrame;
	textFrame.size.height = self.frame.size.height - textFrame.origin.y;
	self.textFrameBackgroundView = [[UIView alloc] initWithFrame:textFrame];
	self.textFrameBackgroundView.backgroundColor = self.headline.label.backgroundColor;
	self.textFrameBackgroundView.alpha = FEATURE_LARGE_ALPHA;
	self.headline.label.backgroundColor = nil;
	self.headline.label.opaque = NO;
	self.summary.label.backgroundColor = nil;
	self.summary.label.opaque = NO;
	
	[self.view insertSubview:self.textFrameBackgroundView aboveSubview:self.imageView];
}

- (void)deleteView {
	[super deleteView];
	[self.textFrameBackgroundView removeFromSuperview];
	self.textFrameBackgroundView = nil;
}

- (void)extendToHeight:(CGFloat)height {
	//CGFloat dy = height - self.frameSize.height;
	[super extendToHeight:height];
	//self.imageSize = CGSizeMake(self.imageSize.width, height);
	//[self adjustTextFrame:dy];
}

@end