//
//  BNCellByline.m
//  BBCNews
//
//  Copyright (c) 2014 British Broadcasting Corporation. All rights reserved.
//

#import "BNStyles.h"
#import "BNItem.h"
#import "BNCell.h"
#import "BNLabelInfo.h"
#import "BNPerson.h"
#import "BNByline.h"
#import "BNImageView.h"


@interface BNCellByline : BNCell

@property (nonatomic) BNLabelInfo* name;
@property (nonatomic) BNLabelInfo* function;
@property (nonatomic) BNImage* thumbnail;

@end



@implementation BNCellByline

- (id)initWithRelationship:(BNRelationship*)relationship andModule:(BNCellsModule*)module {
	self = [super initWithModule:module];
	if (self) {
		
		// NB: Bylines have horrific logic. The relationship provided to this function is not
		// necessarily the one we need. The first thing have have to do is fish all BNByline
		// objects and BNPerson objects out of the containing item.
		NSArray *bylines = [relationship.parentObject findChildrenByPrimaryTypes:@[BNModelType.byline] andSecondaryTypes:@[BNModelType.byline] andFormats:nil];
		NSArray *people = [relationship.parentObject findChildrenByPrimaryTypes:@[BNModelType.person] andSecondaryTypes:@[BNModelType.byline] andFormats:nil];
		
		BNPerson *person =  nil;
		
		if (bylines.count > 0 ){
			if([people count] == 1){
				person = [people objectAtIndex:0];
			}else{
				person = [bylines objectAtIndex:0];
			}
		}
		
		if([people count] > 0){
			if([people count] ==1){
				person = [people objectAtIndex:0];
			}else{
				NSMutableString *s = [NSMutableString new];
				for(int i =0; i< [people count]; i++){
					if(i != 0){
						[s appendString:@", "];
					}
					BNPerson *p = [people objectAtIndex:i];
					if(p.name){
						[s appendString:p.name];
					}
				}
				person.name = [NSString stringWithString:s];
			}
		}
		
		NSString* bylineText = nil;
		if (person.name) {
			self.name = [[BNLabelInfo alloc] initWithString:person.name attributes:attrsBylineName numLines:1];
			bylineText = person.function;
			if(person.thumbnailUrl){
				NSDictionary *dict = @{@"width":@"50", @"height":@"50" };
				BNImage *thumbImage = [[BNImage alloc] initFromJsonDictionary:dict];
				NSString *path = [[NSURL URLWithString:person.thumbnailUrl] path];
				thumbImage.modelId = [NSString stringWithFormat:@"/mcs%@",path];
				self.thumbnail = thumbImage;
			}
		}
		
		// Allow the 'byline' object title, if present, to override the person's 'function'.
		if (bylines.count) {
			BNByline* byline = bylines.firstObject;
			bylineText = byline.title;
		}
		
		self.function = [[BNLabelInfo alloc] initWithString:bylineText attributes:attrsBylineFunction numLines:0];


	}
	return self;
}


- (void)measureForContainingRect:(CGRect)rect {
	rect = UIEdgeInsetsInsetRect(rect, self.module.textPadding);
	CGPoint pt = CGPointMake(self.module.textPadding.left, self.module.textPadding.top);
	if (self.thumbnail) {
		pt.x += bylineThumbnailSize + 12;
	}
	[self.name measureForWidth:rect.size.width offset:pt];
	pt.y += self.name.bounds.size.height;
	[self.function measureForWidth:rect.size.width offset:pt];
	pt.y += self.function.bounds.size.height;
	rect.size.height = pt.y;
	if (self.thumbnail) {
		rect.size.height = MAX(bylineThumbnailSize, rect.size.height);
	}
	rect = UIEdgeInsetsUninsetRect(rect, self.module.textPadding);
	self.frameSize = rect.size;
}

- (void)createView:(UIView *)superview {
	[super createView:superview];
	[self.name createLabel:self.view];
	[self.function createLabel:self.view];
	if (self.thumbnail) {
		BNImageView* imageView = [[BNImageView alloc] initWithFrame:CGRectMake(self.module.textPadding.left, self.module.textPadding.top, bylineThumbnailSize, bylineThumbnailSize)];
		imageView.backgroundColor = [UIColor lightGrayColor];
		[imageView setBNImage:self.thumbnail];
		[self.view addSubview:imageView];
		imageView.layer.masksToBounds = YES;
		imageView.layer.cornerRadius = bylineThumbnailSize/2;
	}
}

- (void)deleteView {
	[self.name removeLabel];
	[self.function removeLabel];
	[super deleteView];
}

#pragma mark - BNCellAccessibilityDelegate

- (BOOL)isAccessibilityElement {
	return YES;
}

- (NSString *)accessibilityLabel {
	return [NSString stringWithFormat:@"%@, %@", self.name.string, self.function.string];
}

@end
