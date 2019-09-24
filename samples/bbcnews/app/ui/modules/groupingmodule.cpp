//
//  BNGroupingModule.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNGroupingModule.h"
#import "BNMyNewsCollection.h"

@interface BNGroupingModule ()

@property (nonatomic) NSArray* originalModules;

@end


@implementation BNGroupingModule

- (id)initFromJson:(NSDictionary *)json {
	self = [super initFromJson:json];
	if (self) {
		self.originalModules = self.modules;
	}
	return self;
}

- (id)copyWithZone:(NSZone *)zone {
	BNGroupingModule* copy = [super copyWithZone:zone];
	copy.originalModules = self.originalModules;
	return copy;
}

- (void)updateLayoutWithContentObject:(BNContent*)contentObject {
	BNMyNewsCollection* collectionGroup = (BNMyNewsCollection*)contentObject;
	NSMutableArray* newModules = [NSMutableArray new];
	int index = 0;
	for (BNContent* collection in collectionGroup.collectionsDeduped) {
		for (BNModule* submodule in self.originalModules) {
			NSArray* groupOffsets = submodule.json[@"groupOffsets"];
			BOOL includeModule = YES;
			if (groupOffsets) {
				includeModule = NO;
				for (NSNumber* groupOffset in groupOffsets) {
					if (groupOffset.intValue == index) {
						includeModule = YES;
						break;
					}
				}
			}
			if (includeModule) {
				BNModule* clonedSubmodule = [submodule copy];
				clonedSubmodule.container = self;
				[clonedSubmodule updateLayoutWithContentObject:collection];
				[newModules addObject:clonedSubmodule];
			}
		}
		index++;
	}
	self.modules = newModules;
}

- (void)layoutWithContainingRect:(CGRect)bounds {
	self.frame = [self.container layoutModules:self.modules withContainingRect:bounds];
}

- (void)updateSubviews:(UIView *)superview {
	[self.container updateSubviewsForModules:self.modules inSuperview:superview];
}

- (CGRect)boundsAfter:(CGRect)frame {
	return [self.container boundsAfter:frame];
}

@end
