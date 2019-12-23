//
//  BNURLRequestManager.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//
#if 0
#import "BNURLRequestManager.h"
#import "BNURLCachedInfo.h"
#import "BNOfflineManager.h"

/**
 
 BNURLRequestManager is our download manager. Clients do not instantiate
 requests directly, they must call requestURL() and unrequestURL() on the
 shared manager object with a non-nil delegate.
 
 When a URL is requested for the first time, a BNURLRequest instance
 is created for it. When a URL is unrequested by its last delegate,
 the BNURLRequest is removed from the manager and forgotten about.
 As long as a BNURLRequest has at least 1 delegate it is considered 'extant'
 and will be automatically updated at regular intervals.
 
 Each BNURLRequest has one of three states:
 
	kStatusQueued: the request is on one of the run queues.
	kStatusRunning: the request is running in the background right now.
	kStatusPending: the request is inactive and awaiting auto-update.
 
 When a URL is requested and the BNURLRequest already exists, it is moved to its run queue
 unless it is already queued or running.
 
 When a BNURLRequest completes, either successfully or unsuccessfully, it is moved to pending.
 
 When the network becomes unavailable, running BNURLRequests are moved back to the heads of their
 run queues.
 
 The offline manager only needs to keep BNURLRequests for
 
 The heart of download management is 3 job queues: high medium and low.
 The high priority queue exists for items that are on the screen right now:
 obviously the article you’re reading and any visible images are more important
 than everything else. The medium queue is for 'adjacent prefetch', i.e. the
 content immediately left and right of whatever’s on the screen (anticipating
 a horizontal swipe) and for images just below the screen (anticipating the user
 scrolling down). The low queue is for 'general prefetch', i.e. anticipating
 what the user is likely to view, especially when offline (on the tube etc).
 
 The downloader allows 4 slots for concurrent downloads. When a slot becomes
 available, it is filled with the next highest priority job. One slot is permanently
 reserved for the high priority queue, i.e. it cannot service medium and low jobs,
 similarly two other slots cannot service low priority jobs. This arrangement
 prevents ‘priority inversion’, i.e. higher priority jobs being unable to run
 because all slots are busy servicing lower priority work.
 
 
 */

@interface BNURLRequestManager () {
	BNURLRequest* running[4];			// Requests that are running right now.
	NSMutableArray* queues[3];			// Queues of requests that are waiting for a run slot
	NSMutableSet* inactiveRequests; 	// Set of extant requests not currently queued
}

@property (nonatomic) Reachability* reachability;
@property (nonatomic) BOOL fetchInProgress;
@property (nonatomic, copy) void (^backgroundFetchCompletionHandler)(UIBackgroundFetchResult);
@property (nonatomic) NSInteger offlineDownloadCount;
@property (nonatomic) BOOL offlineDownloadsAllowed;

@end

NetworkStatus internetStatus = NotReachable;

@implementation BNURLRequestManager

static BNURLRequestManager *s_sharedInstance;

+ (void)staticInit {
	s_sharedInstance = [BNURLRequestManager new];
}
+ (instancetype)sharedInstance {
	NSAssert(s_sharedInstance, @"staticInit error");
	return s_sharedInstance;
}

- (id)init {
	self = [super init];
	if (self) {
		
		// Create queues
		queues[0] = [NSMutableArray new];
		queues[1] = [NSMutableArray new];
		queues[2] = [NSMutableArray new];
		inactiveRequests = [NSMutableSet new];
			
		// Monitor internet connection status
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onNetworkStatusChanged:) name:kReachabilityChangedNotification object:nil];
		self.reachability = [Reachability reachabilityForInternetConnection];
		internetStatus = [self.reachability currentReachabilityStatus];
		[self.reachability startNotifier];
		
		// Listen for foreground/background events
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onAppEnteringForeground:) name:UIApplicationWillEnterForegroundNotification object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onAppEnteredBackground:) name:UIApplicationDidEnterBackgroundNotification object:nil];
		
		[self onAppEnteringForeground:nil];


	}
	return self;
}




- (void)requestURL:(NSURL*)url delegate:(id<BNURLRequestDelegate>)delegate flags:(NSInteger)flags priority:(NSInteger)priority ttl:(NSInteger)ttl creatorBlock:(BNURLRequest*(^)())creatorBlock {
	
	// Find existing request
	BNURLRequest* req = [self findExistingRequest:url];
	if (req) {
		
		// Reprioritization
		if (priority<req.priority) {
			if (req.status == kStatusQueued) {
				[queues[req.priorityForRunQueues] removeObject:req];
			}
			req.priority = priority;
			if (req.status == kStatusQueued) {
				[queues[req.priorityForRunQueues] addObject:req];
			}
		}
		
		// Apply new flags
		if (flags & BNURLRequestFlagForceUpdate) {
			req.flags |= BNURLRequestFlagForceUpdate;
		}
		if (!(flags & BNURLRequestFlagNoUpdateRamCache)) {
			if (req.flags & BNURLRequestFlagNoUpdateRamCache) {
				req.flags &= ~BNURLRequestFlagNoUpdateRamCache;
			}
		}

		// Add new delegate to existing request
		if (![req.delegates containsObject:delegate]) {
			[req.delegates addObject:delegate];
		}
		
	} else {
		
		// Create new request
		req = creatorBlock();
		req.status = kStatusCreated;
		req.priority = priority;
		req.flags = flags;
		req.ttl = ttl;
		req.delegates = [NSMutableArray new];
		[req.delegates addObject:delegate];
		req.cacheInfo = [BNURLCachedInfo cachedResponseForURL:url];
	}
	
	// If not force-updating, and cached URL contents exist, fetch the cached data immediately for this delegate
	if (!(flags & BNURLRequestFlagForceUpdate)) {
		if (req.cacheInfo) {
			[req.cacheInfo loadCachedObjectWithBlock:^(id<NSCoding> cachedObj) {
				[delegate onRequestLoadedObject:req object:cachedObj isCacheData:YES];
			} priority:priority andFlags:flags];
		}
	}

	// Run request
	[self moveRequestToRunQueue:req];

}



- (void)unrequestURL:(NSURL *)url delegate:(id<BNURLRequestDelegate>)delegate {
	NSAssert([NSThread isMainThread], @"Wrong thread");
	BNURLRequest* req = [self findExistingRequest:url];
	if (req) {
		[req.delegates removeObject:delegate];
		if (req.delegates.count == 0) {

			[req cancel];
			
			if (req.status == kStatusQueued) {
				NSMutableArray* queue = queues[req.priorityForRunQueues];
				[queue removeObject:req];
			} else if (req.status == kStatusRunning) {
				[self removeFromRunningList:req];
			}
			if (req.status == kStatusInactive) {
				[inactiveRequests removeObject:req];
			}
			req.status = kStatusDead;
		} else {
			// TODO: reprioritize
		}
	}
}

- (void)moveRequestToRunQueue:(BNURLRequest*)req {
	
	if (req.status == kStatusRunning || req.status == kStatusQueued) {
		return;
	}
	
	// If it's an offline download then check offline is allowed right now
	if (req.priority >= BNDownloadPriorityOffline) {
		if (!self.offlineDownloadsAllowed) {
			return;
		}
	}

	// No need to update yet?
	if (0 == (req.flags & BNURLRequestFlagForceUpdate)) {
		if (req.cacheInfo) {
			NSTimeInterval now = [[NSDate date] timeIntervalSinceReferenceDate];
			BOOL hasExpired = req.cacheInfo.expiryTime <= now;
			if (!hasExpired) {
				[self moveRequestToInactive:req];
				return;
			}
		}
	}
	
	
	// If currently inactive, remove from inactive set
	if (req.status == kStatusInactive) {
		[inactiveRequests removeObject:req];
	}
	[queues[req.priorityForRunQueues] insertObject:req atIndex:0];
	req.status = kStatusQueued;
	[self drain];
}

- (void)moveRequestToInactive:(BNURLRequest*)req {
	if (req.status == kStatusInactive) {
		return;
	}
	if (req.delegates.count) {
		req.status = kStatusInactive;
		[inactiveRequests addObject:req];
	} else {
		req.status = kStatusDead;
	}
}

- (void)removeFromRunningList:(BNURLRequest*)req {
	for (int i=0 ; i<4 ; i++) {
		if (running[i] == req) {
			running[i] = nil;
			[self drain];
			break;
		}
	}
}
- (BNURLRequest*)findExistingRequest:(NSURL*)url {
	
	// Check running requests first
	for (int i=0 ; i<4 ; i++) {
		if ([running[i].URL isEqual:url]) {
			return running[i];
		}
	}
	
	// Check network queues
	for (NSInteger pri=0 ; pri<3 ; pri++) {
		NSMutableArray* queue = queues[pri];
		for (BNURLRequest* queuedReq in queue) {
			if ([queuedReq.URL isEqual:url]) {
				return queuedReq;
			}
		}
	}
	
	// Check pending
	for (BNURLRequest* req in inactiveRequests) {
		if ([req.URL isEqual:url]) {
			return req;
		}
	}
	
	return nil;
}


- (BNURLRequest*)dequeueFirstJobWithMinPriority:(NSInteger)minPriority {
	BNURLRequest* req = nil;
	for (NSInteger i=minPriority ; i>=0 ; i--) {
		NSMutableArray* queue = queues[i];
		if (queue.count) {
			req = queue.firstObject;
			
			if (req.priority >= BNDownloadPriorityOffline) {
				if ([BNOfflineManager budgetExceeded]) {
					BNLog(@"Download budget limit hit");
					continue;
				}
			}

			[queue removeObjectAtIndex:0];
			req.status = kStatusRunning;
			return req;
		}
	}
	return nil;
}

- (void)drain {
	NSAssert([NSThread isMainThread], @"wrong thread");
	if (internetStatus == NotReachable) {
		return;
	}
	
	// Slot 0 can only be given high priority jobs
	if (!running[0]) {
		running[0] = [self dequeueFirstJobWithMinPriority:BNDownloadPriorityHigh];
		if (running[0]) {
			[running[0] start];
		}
	}
	
	// Slot 1 can have high or medium
	if (!running[1]) {
		running[1] = [self dequeueFirstJobWithMinPriority:BNDownloadPriorityMedium];
		if (running[1]) {
			[running[1] start];
		}
	}
	
	// Slots 2 and 3 take anything
	if (!running[2]) {
		running[2] = [self dequeueFirstJobWithMinPriority:BNDownloadPriorityLow];
		if (running[2]) {
			[running[2] start];
		}
	}
	if (!running[3]) {
		running[3] = [self dequeueFirstJobWithMinPriority:BNDownloadPriorityLow];
		if (running[3]) {
			[running[3] start];
		}
	}

#ifdef DEBUG
	char ach[32];
	char* p = ach;
	for (int i=0 ; i<4 ; i++) {
		BNURLRequest* req = running[i];
		if (!req) {
			*p++ = '-';
		} else {
			if (req.priority == BNDownloadPriorityHigh) {
				*p++ = 'H';
			} else if (req.priority == BNDownloadPriorityMedium) {
				*p++ = 'M';
			} else if (req.priority == BNDownloadPriorityLow) {
				*p++ = 'L';
			} else {
				*p++ = '?';
			}
		}
	}
	*p++ = '\0';

	//NSLog(@"%s  H:%@ M:%@ L:%@  ina:%@", ach, @(queues[0].count), @(queues[1].count), @(queues[2].count), @(inactiveRequests.count));
#endif
	
}


- (void)moveAllInactiveRequestsToRunQueues {	// Move everything that's inactive to the queues
	NSSet* inactive = inactiveRequests.copy;
	for (BNURLRequest* req in inactive) {
		[self moveRequestToRunQueue:req];
	}
}

- (void)rescheduleAutorefresh:(BOOL)wantAutorefresh {
	[NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(autorefresh) object:nil];
	if (wantAutorefresh) {
		[self performSelector:@selector(autorefresh) withObject:nil afterDelay:60];
	}
}

- (void)autorefresh {
	BNLog(@"*** Autorefresh: moving %lu jobs to run queue", (unsigned long)inactiveRequests.count);
	self.offlineDownloadsAllowed = YES;
	[self moveAllInactiveRequestsToRunQueues];
	[self rescheduleAutorefresh:YES];
}


- (void)onAppEnteringForeground:(NSNotification*)notification {
	[self rescheduleAutorefresh:YES];
	self.offlineDownloadsAllowed = YES;
	[self drain];
}
- (void)onAppEnteredBackground:(NSNotification*)notification {
	
	// 1. Turn off autorefresh
	[self rescheduleAutorefresh:NO];

	// 2. Cancel all running downloads and put them back on their queues. Downloads will be resumed once
	// the app is in foreground, or a background fetch is started.
	[self cancelAndRequeueRunningDownloads];

	// 3. Disable offline downloads
	self.offlineDownloadsAllowed = NO;

}

- (void)performBackgroundFetchWithCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler {
	[self stopBackgroundFetch];
	
	// Store handler
	self.backgroundFetchCompletionHandler = completionHandler;
	
	// Schedule a callback so we don't overrun
	[self performSelector:@selector(stopBackgroundFetch) withObject:nil afterDelay:28];
	
	// Flag that offline downloads are allowed
	BNLog(@"** Background fetch started **");
	self.offlineDownloadsAllowed = YES;
	self.fetchInProgress = YES;
	self.offlineDownloadCount = 0;
	[self moveAllInactiveRequestsToRunQueues];
}



- (void)stopBackgroundFetch {
	[NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(stopBackgroundFetch) object:nil];
	
	// Existing handler set? Shouldn't happen but just in case we call it to avoid an OS penalty
	if (self.fetchInProgress) {
		BNLog(@"** Background fetch finished ** %d", (int)self.offlineDownloadCount);
		if (self.backgroundFetchCompletionHandler) {
			self.backgroundFetchCompletionHandler(self.offlineDownloadCount ? UIBackgroundFetchResultNewData : UIBackgroundFetchResultNoData);
		}
		self.fetchInProgress = NO;
		self.offlineDownloadsAllowed = NO;
		[BNOfflineManager saveBudgets];

	}

}

- (void)cancelAndRequeueRunningDownloads {
	for (int i=0 ; i<4 ; i++) {
		BNURLRequest* req = running[i];
		if (req) {
			[req cancel];
			[queues[req.priorityForRunQueues] insertObject:req atIndex:0];
			req.status = kStatusQueued;
			running[i] = nil;
		}
	}
}

/**
 * Internet Connection Status Monitoring
 */

- (void)onNetworkStatusChanged:(NSNotification*)notification {
	// called after network status changes
	NSAssert([NSThread isMainThread], @"wrong thread");
	
	// Get new status and paranoidly guard against non-changes
	NetworkStatus newStatus = [self.reachability currentReachabilityStatus];
	if (newStatus == internetStatus) {
		return;
	}
	internetStatus = newStatus;
	
	// Whatever the change, cancel running jobs and move them back to the head of
	// their respective network queues
	[self cancelAndRequeueRunningDownloads];
	
	// Update status to queued or waiting for network
	BOOL noNetwork = (internetStatus == NotReachable);
	/*for (int i=0 ; i<3 ; i++) {
		for (BNURLRequest* req in queues[i]) {
			req.status = noNetwork ? kStatusWaitingForNetwork : kStatusQueuedToRun;
		}
	}*/
	
	
	// Unless there's no internet, drain the network queues
	if (noNetwork) {
		BNLog(@"No internet connection.");
	} else {
		BNLog(@"Internet connection is %@.", (internetStatus==ReachableViaWiFi)?@"wifi":@"cell");
		// defer to give network time to properly establish before starting downloads
		[self performSelector:@selector(onNetworkConnected) withObject:nil afterDelay:5.f];
	}
}

- (void)onNetworkConnected {
	[self drain];
}



@end


#endif
