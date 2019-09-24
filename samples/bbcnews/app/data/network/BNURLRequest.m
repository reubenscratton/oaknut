//
//  BNURLRequest.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//
#import "BBCNEndpoint.h"
#import "BNEnvironment.h"

#import "BNURLRequest.h"
#import "BNURLRequestManager.h"
#import "BNURLCachedInfo.h"
#import "BNDeviceSpec.h"
#import "BNOfflineManager.h"
#import <libkern/OSAtomic.h>
#import "BNPolicyManager.h"
#import "BNPolicy.h"

 
static int32_t s_numDownloads;
static NSURLSession* s_session;

@implementation BNURLRequest


- (void)cancel {
	if (self.dataTask) {
		[self.dataTask cancel];
		self.dataTask = nil;
	}
}
	



+ (void)updateNetworkIndicator:(NSNumber*)flag {
	[UIApplication sharedApplication].networkActivityIndicatorVisible = [flag boolValue];
}

- (void)start {
	
	
	//check policy file first before making a request
	NSError *error;
	if([[BNPolicyManager sharedInstance] policy].killSwitch && [self.URL.absoluteString rangeOfString:[[[BNPolicyManager sharedInstance] policy].endpointPolicy getHREF]].location == NSNotFound){
			NSDictionary *userInfo = @{
								   NSLocalizedDescriptionKey:@"We apologise, the app cannot currently update the content. Please try later.",
								   NSLocalizedFailureReasonErrorKey: @"Kill switch active.",
								   NSLocalizedRecoverySuggestionErrorKey: @"Try again later",
								   };
		
		error = [NSError errorWithDomain:NSURLErrorDomain code:NSURLErrorResourceUnavailable userInfo:userInfo];
	}
	
	if([self.URL.absoluteString rangeOfString:@"BNFLAGPOLEDOWN"].location != NSNotFound){
		NSDictionary *userInfo = @{
								   NSLocalizedDescriptionKey:@"We apologise, the app cannot currently update the content. Please try later.",
								   NSLocalizedFailureReasonErrorKey: @"Flagpole was down.",
								   NSLocalizedRecoverySuggestionErrorKey: @"Try again later",
								   };
		error = [NSError errorWithDomain:NSURLErrorDomain code:NSURLErrorResourceUnavailable userInfo:userInfo];
	}
	

	if(error){
		[self onCompleteWithObject:nil error:error isCacheData:NO];
		/*[[BNURLRequestManager sharedManager] moveRequestToPending:self];
		NSArray* delegates = self.delegates.copy;
		// Fire error callback
		for (id<BNURLRequestDelegate> delegate in delegates) {
			if ([((NSObject*)delegate) respondsToSelector:@selector(onRequestError:error:httpStatus:)]) {
				[delegate onRequestError:self error:error httpStatus:503];
			}
		}*/
		return;
	}
	
	
	
	// Do not use NSURLCache
	self.cachePolicy = NSURLRequestReloadIgnoringCacheData;
	
	// Set conditional-get headers
	if (self.cacheInfo.lastModified) {
		[self setValue:self.cacheInfo.lastModified forHTTPHeaderField:@"If-Modified-Since"];
	}
	if (self.cacheInfo.etag) {
		[self setValue:self.cacheInfo.etag forHTTPHeaderField:@"If-None-Match"];
	}

	// Set devicespec headers
	[[BNDeviceSpec sharedInstance] applyToURLRequest:self];

	// If this is the first network request AND the network activity indicator is not already visible
	// then schedule to show the indicator in 2 seconds' time.
	if (1 == OSAtomicIncrement32(&s_numDownloads)) {
		if (![UIApplication sharedApplication].networkActivityIndicatorVisible) {
			dispatch_async(dispatch_get_main_queue(), ^{
				[BNURLRequest performSelector:@selector(updateNetworkIndicator:) withObject:@(YES) afterDelay:2];
			});
		}
	}
	
	// Run background task
    self.dataTask = [s_session dataTaskWithRequest:self completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
		
		// If this was the last network request then cancel any pending calls to show the network activity indicator
		// and also schedule a call to turn it off in 1 seconds' time.
		if (0 == OSAtomicDecrement32(&s_numDownloads)) {
			dispatch_async(dispatch_get_main_queue(), ^{
				[NSObject cancelPreviousPerformRequestsWithTarget:[BNURLRequest class] selector:@selector(updateNetworkIndicator:) object:@(YES)];
				if ([UIApplication sharedApplication].networkActivityIndicatorVisible) {
					[BNURLRequest performSelector:@selector(updateNetworkIndicator:) withObject:@(NO) afterDelay:1];
				}
			});
		}

		// Store response
		self.response = (NSHTTPURLResponse*)response;
		id<NSCoding> responseObject = nil;
		
#ifndef DEBUG
		@try {
#endif

		// HTTP 202 Accepted => back-off and re-request up to 4 times
		if (self.response.statusCode == 202) {
			NSLog(@"HTTP 202 for %@ - retry count: %@", self.URL.resourceSpecifier, @(self.retryCount));
			self.retryCount++;
			if (self.retryCount < 4) {
				NSTimeInterval maxAge = 3; // self.response.maxAge
				dispatch_async(dispatch_get_main_queue(), ^{
					[self performSelector:@selector(start) withObject:nil afterDelay:maxAge];
				});
				return;
			}
			
			// Too many 202s => Fail with an appropriate error
			error = [NSError errorWithDomain:NSURLErrorDomain code:NSURLErrorBadServerResponse userInfo:nil];
		}
		
		// General HTTP errors. It's not obvious to me that the NSError will definitely be non-nil in all
		// HTTP error cases, so we paranoidly create one, just in case.
		if (self.response.statusCode != 200  && self.response.statusCode != 304) {
			if (response) {
				NSLog(@"HTTP %@ for %@", @(self.response.statusCode), self.URL);
			}
			if (!error) {
				error = [NSError errorWithDomain:NSURLErrorDomain code:NSURLErrorBadServerResponse userInfo:nil];
			}
		}
		
		// Process a definite response here on the background thread
		if (data.length && !error) {
			responseObject = [self processResponseDataInBackground:data error:&error];
		}
			
			
		// EXPERIMENT: If the response object is image data then make a deep copy of it to
		// try to avoid a rare-ish crash during image decode.
		//
		// (see https://rink.hockeyapp->net/manage/apps/87483/app_versions/1/crash_reasons/26930003)
		//
		// *** NB: IF THIS CODE DOES NOT DEFINITELY FIX THE CRASH IT SHOULD BE REMOVED! ***
		//
		if ([((id<NSObject>)responseObject) isKindOfClass:[NSData class]]) {
			if ([self.URL.resourceSpecifier rangeOfString:@"/img/"].location != NSNotFound) {
				NSData* origData = (NSData*)responseObject;
				responseObject = [NSData dataWithBytes:origData.bytes length:origData.length];
			}
		}
		
		// Update cache info. Note that for HTTP 304 the response object is nil, but the cache must handle
		// it anyway so it can update the entity's expiry time.
		if (!error) {
			if (!self.cacheInfo) {
				self.cacheInfo = [[BNURLCachedInfo alloc] initWithURL:self.URL];
			}
			[self.cacheInfo updateCachedObject:responseObject withHeaders:self.response.allHeaderFields andTTL:self.ttl];
			[self updateCacheInfoChildren:responseObject];
		}
			
#ifndef DEBUG
		}
		@catch (NSException* ex) {
			error = [NSError errorWithDomain:NSURLErrorDomain code:NSURLErrorUnknown userInfo:@{@"exception":ex}];
		}
#endif
		// Notify clients and clean up
		dispatch_async(dispatch_get_main_queue(), ^{
			
			if (self.priority >= BNDownloadPriorityOffline) {
				[BNOfflineManager deductFromBudget:data.length + self.response.allHeaderFields.count * 50];
			}
			
			// Unset force-update flag
			self.flags &= ~BNURLRequestFlagForceUpdate;

			self.dataTask = nil;
			self.response = nil;
			
			// If we've downloaded a new object, mark that we've done so that any pending cache loads dont overwrite it
			if (responseObject) {
				self.hasDownloadedNewData = YES;
			}
			
			[[BNURLRequestManager sharedInstance] removeFromRunningList:self];
			
			// Error -1003 observed when network lost, handle it by immediately requeueing the request
			if (error && error.code == -1003) {
				[[BNURLRequestManager sharedInstance] moveRequestToRunQueue:self];
				return;
			}

			[[BNURLRequestManager sharedInstance] moveRequestToInactive:self];
			//self.status = kStatusComplete;

#ifndef DEBUG
			@try {
#endif
			[self onCompleteWithObject:responseObject error:error isCacheData:NO];
#ifndef DEBUG
			}
			@catch (NSException* ex) {
				//NSError* error = [NSError errorWithDomain:NSURLErrorDomain code:NSURLErrorUnknown userInfo:@{@"exception":ex}];
			}
#endif

		});
		
	}];
	//NSLog(@"Networking started for %@", self.URL.absoluteString);
    [self.dataTask resume];
}

- (void)onCompleteWithObject:(id<NSCoding>)object error:(NSError*)error isCacheData:(BOOL)isCacheData  {
	NSAssert([NSThread isMainThread], @"Wrong thread");
	NSArray* delegates = self.delegates.copy;
	
	// Fire error callback, unless request was cancelled which is not, imho, an error condition
	if (error && error.code != -999) {
		for (id<BNURLRequestDelegate> delegate in delegates) {
			if ([((NSObject*)delegate) respondsToSelector:@selector(onRequestError:error:httpStatus:)]) {
				[delegate onRequestError:self error:error httpStatus:self.response.statusCode];
			}
		}
	}

	// Fire object-loaded callback
	if (object) {
		for (id<BNURLRequestDelegate> delegate in delegates) {
			[delegate onRequestLoadedObject:self object:object isCacheData:isCacheData];
		}
	}
}

- (id)processResponseDataInBackground:(NSData*)data error:(NSError*__autoreleasing*)error {
	return data;
}


- (void)updateCacheInfoChildren:(id<NSCoding>)object {
	self.cacheInfo.children = nil;
}


- (void)setStatus:(BNURLRequestStatus)status {
	if (status == _status) {
		return;
	}
	_status = status;
	
	// Tell delegates status changed
	NSArray* delegates = self.delegates.copy;
	for (id<BNURLRequestDelegate> delegate in delegates) {
		if ([((NSObject*)delegate) respondsToSelector:@selector(onRequestStatusChanged:)]) {
			[delegate onRequestStatusChanged:self];
		}
	}
}

- (NSInteger)priorityForRunQueues {
	return MIN(self.priority, BNDownloadPriorityLow);
}


@end
