//
//  BNSuggestedTopics.h
//  BBCNews
//
//  Copyright (c) 2015 BBC News. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BNSuggestedTopics : NSObject

@property (nonatomic) NSMutableArray* groups;

+ (instancetype)currentData;

@end
