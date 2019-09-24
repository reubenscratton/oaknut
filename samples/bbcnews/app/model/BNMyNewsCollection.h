//
//  BNMyNewsCollection.h
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNCollection.h"

@protocol BNURLRequestDelegate;

@interface BNMyNewsCollection : BNCollection

@property (nonatomic) NSArray* collectionsDeduped;

+ (instancetype)sharedInstance;
- (id)init;
- (void)addDelegate:(id<BNURLRequestDelegate>)delegate;
- (void)removeDelegate:(id<BNURLRequestDelegate>)delegate;
- (void)update:(BOOL)force;
- (void)dedupe:(NSArray *)collections followedCollections:(NSArray *)followedCollections;

@end
