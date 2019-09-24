//
//  BNURLRequestManager.h
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

@import Foundation;
@import UIKit;
#import "Reachability.h"
#import "BNURLRequest.h"

extern NetworkStatus internetStatus;

@protocol BNURLRequestDelegate
- (void)onRequestLoadedObject:(BNURLRequest*)request object:(id)object isCacheData:(BOOL)isCacheData;
- (void)onRequestError:(BNURLRequest*)request error:(NSError*)error httpStatus:(NSInteger)httpStatus;

@optional
- (void)onRequestStatusChanged:(BNURLRequest*)request;

@end


@interface BNURLRequestManager : NSObject

+ (instancetype)sharedInstance;
- (void)requestURL:(NSURL*)url delegate:(id<BNURLRequestDelegate>)delegate flags:(NSInteger)flags priority:(NSInteger)priority ttl:(NSInteger)ttl creatorBlock:(BNURLRequest*(^)())creatorBlock;
- (void)unrequestURL:(NSURL*)url delegate:(id<BNURLRequestDelegate>)delegate;

// Only for use of BNURLRequest
- (void)removeFromRunningList:(BNURLRequest*)req;
- (void)moveRequestToRunQueue:(BNURLRequest*)req;
- (void)moveRequestToInactive:(BNURLRequest*)req;

// Only for use of BNAppDelegate
- (void)performBackgroundFetchWithCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler;


@end
