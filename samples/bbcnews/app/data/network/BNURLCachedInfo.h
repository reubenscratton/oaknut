//
//  BNURLCache.h
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

@import Foundation;


@interface BNURLCachedInfo : NSObject <NSCoding>

@property (nonatomic) NSURL* url;
@property (nonatomic) NSTimeInterval expiryTime;
@property (nonatomic) NSString* lastModified;
@property (nonatomic) NSString* etag;
@property (nonatomic) NSArray* children;

+ (BNURLCachedInfo*)cachedResponseForURL:(NSURL*)url;

- (id)initWithURL:(NSURL*)url;
- (void)loadCachedObjectWithBlock:(void (^)(id<NSCoding> cachedObject))completionBlock priority:(NSInteger)priority andFlags:(NSUInteger)flags;
- (void)updateCachedObject:(id<NSCoding>)cachedObject withHeaders:(NSDictionary*)headers andTTL:(NSInteger)ttl;
- (void)cancelLoadRequest;
						
@end
	
