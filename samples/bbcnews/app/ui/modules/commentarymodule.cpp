//
//  BNCommentaryModule.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNCommentaryModule.h"
#import "BNCommentaryItem.h"
#import "BNElementText.h"
#import "BNElementMedia.h"
#import "BNStyles.h"
#import "BNImage.h"
#import "BNTextTraits.h"
#import "BNLiveEvent.h"
#import "BNContentRequest.h"
#import "BNPerson.h"

@interface BNCommentaryModule () <BNURLRequestDelegate>

@property (nonatomic) BNLiveEvent *liveEvent;

@end

@implementation BNCommentaryModule

- (void)updateLayoutWithContentObject:(BNContent*)contentObject {
	[super updateLayoutWithContentObject:contentObject];
	if (self.liveEvent) {
		[BNContentRequest unrequest:self.liveEvent.commentaryId delegate:self];
	}
	self.liveEvent = (BNLiveEvent*)contentObject;
	[BNContentRequest request:self.liveEvent.commentaryId delegate:self flags:0 priority:BNDownloadPriorityHigh];
}

- (BNElementImage*)getBylineImage:(NSString*)imageFile size:(CGSize)size {
	BNElementImage* bylineImage = [[BNElementImage alloc] init];
	bylineImage.normalizedWidth = IS_IPAD?56:48;
	bylineImage.marginLeft = self.textPadding.left;
	bylineImage.marginRight = 0;
	bylineImage.marginTop = IS_IPAD?8:4;
	bylineImage.marginBottom = IS_IPAD?8:4;
	BNStaticImage* image = [BNStaticImage new];
	image.width = size.width;
	image.height = size.height;
	image.positionHint = @"abs";
	image.modelId = imageFile;
	bylineImage.media = image;
	return bylineImage;
}

- (void)onRequestLoadedObject:(BNURLRequest *)request object:(id)object isCacheData:(BOOL)isCacheData {
	
	BNItem* contentObject = (BNItem*)object;
	BNItem* uberItem = [[BNItem alloc] initWithId:contentObject.modelId];
	uberItem.elements = [NSMutableArray new];
	NSMutableArray* allChildRelationships = [NSMutableArray new];
	
	BNElementText* t = [[BNElementText alloc] initWithStyle:@"uitextview_bugfix"];
	t.text = [[NSMutableString alloc] initWithString:@" "];
	[uberItem.elements addObject:t];
	
	NSArray* people = [self.liveEvent findChildrenByPrimaryTypes:@[BNModelType.person] andSecondaryTypes:@[BNModelType.byline] andFormats:nil];
	
	BNElementText* heading = [[BNElementText alloc] initWithStyle:@"crosshead"];
	heading.text = [[NSMutableString alloc] initWithString:@"Live Reporting"];
	[uberItem.elements addObject:heading];
	
	if([people count] > 0){
		NSMutableString *peopleString = [[NSMutableString alloc] initWithString:@"By"];
		for(int i=0; i < [people count]; i++){
			BNItem *item = [people objectAtIndex:i];
			NSString *seperator = @" ";
			if(i > 0 && i == [people count] -1){
				seperator = @" and ";
			}else if (i > 0)
			{
				seperator = @", ";
			}
			
			[peopleString appendString:[NSString stringWithFormat:@"%@%@",seperator,item.name]];
		}
		BNElementText* peopleText = [[BNElementText alloc] initWithStyle:@"commentarySubheading"];
		peopleText.text = peopleString;
		[uberItem.elements addObject:peopleText];
		
	}
	
	BNElementText* timeNotice = [[BNElementText alloc] initWithStyle:@"normal"];
	timeNotice.text = [[NSMutableString alloc] initWithString:@"All times stated are UK Time"];
	[uberItem.elements addObject:timeNotice];
	
	
	NSArray* commentaryItems = [contentObject findChildrenByPrimaryTypes:@[BNModelType.commentary] andSecondaryTypes:nil andFormats:nil];
	for (BNCommentaryItem* commentary in commentaryItems) {
		
		// Create a separator image
		if (uberItem.elements.count == 0) {
			BNElementText* t = [[BNElementText alloc] initWithStyle:@"uitextview_bugfix"];
			t.text = [[NSMutableString alloc] initWithString:@" "];
			[uberItem.elements addObject:t];
		} else {
			BNElementImage* separatorImage = [BNElementImage new];
			separatorImage.normalizedWidth = 1.f;
			separatorImage.marginBottom = 8;
			BNImage* image = [BNImage new];
			image.width = 464;
			image.height = 2;
			image.positionHint = @"full-width";
			image.modelId = @"/mcs/media/images/74982000/jpg/_74982321_line976.jpg";
			separatorImage.media = image;
			[uberItem.elements addObject:separatorImage];
		}
		
		
		if (commentary.time) {
			BNElementText* time = [[BNElementText alloc] initWithStyle:@"commentaryTime"];
			time.text = commentary.time.mutableCopy;
			BNTextTraitForecolor* colorTime = [[BNTextTraitForecolor alloc] init];
			colorTime.forecolor = [UIColor darkGrayColor];
			colorTime.end = time.text.length;
			[time.traits addObject:colorTime];
			[uberItem.elements addObject:time];
		}
		
		NSArray *byline = [commentary findChildrenByPrimaryTypes:@[BNModelType.byline] andSecondaryTypes:@[BNModelType.byline] andFormats:nil];
		NSArray *people = [commentary findChildrenByPrimaryTypes:@[BNModelType.person] andSecondaryTypes:@[BNModelType.byline] andFormats:nil];
		
		BNPerson *person =  nil;
		
		if([byline count] > 0 ){
			if([people count] == 1){
				person = [people objectAtIndex:0];
			}else{
				person = [byline objectAtIndex:0];
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
		if (person.name) {
			commentary.heading = person.name;
			commentary.subHeading = person.function;
		}
	
		if([byline count] > 0 || [people count] > 0){
			if(person.thumbnailUrl){
				NSDictionary *dict = @{@"width":@"50", @"height":@"50" };
				BNImage *thumbImage = [[BNImage alloc] initFromJsonDictionary:dict];
				NSString *path = [[NSURL URLWithString:person.thumbnailUrl] path];
				thumbImage.modelId = [NSString stringWithFormat:@"/mcs%@",path];
				thumbImage.width = 64;
				thumbImage.height = 64;
				thumbImage.positionHint = @"abs";
				
				BNElementImage* bylineImage = [[BNElementImage alloc] init];
				bylineImage.marginLeft = self.textPadding.left;
				bylineImage.marginRight = 0;
				bylineImage.marginTop = IS_IPAD?8:4;
				bylineImage.marginBottom = IS_IPAD?8:4;
				bylineImage.media = thumbImage;
				bylineImage.normalizedWidth = IS_IPAD?56:48;
				bylineImage.disableForceFullWidth = YES;
				
				
				[uberItem.elements addObject:bylineImage];
			}
		}
		
		if(!person.thumbnailUrl)
		{
			if ([commentary.typeCode isEqualToString:BNCommentaryTypeCode.comment]) {
				[uberItem.elements addObject:[self getBylineImage:@"images/comment" size:CGSizeMake(64,64)]];
			} else if ([commentary.typeCode isEqualToString:BNCommentaryTypeCode.sms]) {
				[uberItem.elements addObject:[self getBylineImage:@"images/sms" size:CGSizeMake(64,64)]];
			} else if ([commentary.typeCode isEqualToString:BNCommentaryTypeCode.tweet]) {
				[uberItem.elements addObject:[self getBylineImage:@"images/twitter" size:CGSizeMake(64,53)]];
			} else if ([commentary.typeCode isEqualToString:BNCommentaryTypeCode.email]) {
				//[uberItem.elements addObject:[self getBylineImage:@"email.png"]];
			} else if ([commentary.typeCode isEqualToString:BNCommentaryTypeCode.blog]) {
				[uberItem.elements addObject:[self getBylineImage:@"images/blog" size:CGSizeMake(64,64)]];
			}
		}
	
		if (commentary.heading) {
			BNElementText* heading = [BNElementText new];
			heading.style = [BNStyles namedTextStyle:@"commentaryHeading"];
			heading.text = commentary.heading.mutableCopy;
			[uberItem.elements addObject:heading];
		}
		if (commentary.subHeading) {
			BNElementText* subHeading = [BNElementText new];
			subHeading.style = [BNStyles namedTextStyle:@"commentarySubheading"];
			subHeading.text = commentary.subHeading.mutableCopy;
			[uberItem.elements addObject:subHeading];
		}
		[uberItem.elements addObjectsFromArray:commentary.elements];
		[allChildRelationships addObjectsFromArray:commentary.childRelationships];
	}
	uberItem.childRelationships = allChildRelationships;
	[uberItem configureAfterParsing];
	
	[super updateLayoutWithContentObject:uberItem];

	[self invalidateSize];
}
- (void)onRequestError:(BNURLRequest *)request error:(NSError *)error httpStatus:(NSInteger)httpStatus {
}

- (void)layoutWithContainingRect:(CGRect)bounds {
	[super realLayoutWithContainingRect:bounds];
}


- (void)forceRefresh {
	if (self.liveEvent) {
		[BNContentRequest unrequest:self.liveEvent.commentaryId delegate:self];
		[BNContentRequest request:self.liveEvent.commentaryId delegate:self flags:BNURLRequestFlagForceUpdate priority:BNDownloadPriorityHigh];
	}
}

- (void)cleanup {
	[super cleanup];
	if (self.liveEvent) {
		[BNContentRequest unrequest:self.liveEvent.commentaryId delegate:self];
	}
}
@end
