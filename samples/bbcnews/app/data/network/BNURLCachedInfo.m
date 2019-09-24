//
//  BNURLCache.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNURLCachedInfo.h"
#import "BNStyles.h"
#import "BNURLRequest.h"
#import "BNBlockOperation.h"

static NSString* s_cacheRoot;
static NSString* s_indexFilePath;
static NSMutableDictionary* s_indexDictionary;
static NSMutableArray* s_indexMRU;
static NSDateFormatter* s_dateFormatter;
static NSOperationQueue* s_storageQueue;
static BOOL s_indexSaveScheduled;
static NSCache* s_ramCache;

// IMPORTANT: Increment this if any aspect of cache serialization changes
static NSInteger CURRENT_CACHE_VERSION = 9;

#define MAX_STORAGE_ITEMS 50000		// TODO: This should be linked to followed collection count

NSString* encodeString(NSString* s) {
	return (NSString*)CFBridgingRelease(CFURLCreateStringByAddingPercentEscapes(NULL,
		(CFStringRef)s,	NULL, (CFStringRef)@"!*'\"();:@&=+$,/?%#[]% ",
		CFStringConvertNSStringEncodingToEncoding(NSUTF8StringEncoding)));
}


@implementation BNURLCachedInfo

+ (void)staticInit {
	
	// TODO: May change this to ISO 8601.
	s_dateFormatter = [[NSDateFormatter alloc] init];
	[s_dateFormatter setDateFormat:@"EEE, dd MMM yyyy HH:mm:ss zzz"];
	
	NSString* appSupportDir = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) lastObject];
	s_cacheRoot = [appSupportDir stringByAppendingString:@"/urlcache/"];
	s_indexFilePath = [s_cacheRoot stringByAppendingString:@"index.dat"];
	
	// If upgrading cache from previous version, destroy it
	NSInteger previousCacheVersion = [[NSUserDefaults standardUserDefaults] integerForKey:@"cacheVersion"];
	if (previousCacheVersion != CURRENT_CACHE_VERSION) {
		NSLog(@"** Cache upgrade. Destroying old cache. **");
		[[NSFileManager defaultManager] removeItemAtPath:s_indexFilePath error:nil];
		[[NSUserDefaults standardUserDefaults] setInteger:CURRENT_CACHE_VERSION forKey:@"cacheVersion"];
		[[NSUserDefaults standardUserDefaults] synchronize];
	}
	
	// Deserialize index here on the main thread. This feels gnarly but we don't really
	// want to let the main thread run until the cache is in place.
	NSData *indexFileData = [NSData dataWithContentsOfFile:s_indexFilePath];
	s_indexMRU = [NSKeyedUnarchiver unarchiveObjectWithData:indexFileData];
	if (!s_indexMRU) {
		s_indexMRU = [NSMutableArray new];
	}
	s_indexDictionary = [NSMutableDictionary new];
	for (BNURLCachedInfo* cachedInfo in s_indexMRU) {
		s_indexDictionary[cachedInfo.url] = cachedInfo;
	}
	
	s_storageQueue = [[NSOperationQueue alloc] init];
	s_storageQueue.maxConcurrentOperationCount = 1;
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appWillResignActive:) name:UIApplicationWillResignActiveNotification object:nil];
	
	// Set up RAM object cache
	s_ramCache = [NSCache new];
	s_ramCache.totalCostLimit = (IS_IPAD?32:24) * 1024*1024;
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(memoryWarning:)
			name:UIApplicationDidReceiveMemoryWarningNotification object:nil];
	

}

+(void)memoryWarning:(NSNotification*)notification {
	[s_ramCache removeAllObjects];
}

+ (BNURLCachedInfo*)cachedResponseForURL:(NSURL*)url {
	NSAssert(s_indexDictionary, @"argh!");
	BNURLCachedInfo* info = s_indexDictionary[url];
	if (info) { // Cache hit, so move to top of the MRU
		@synchronized (s_indexMRU) {
			[s_indexMRU removeObject:info]; // argh, bet this is slow
			[s_indexMRU addObject:info];
		}
		[BNURLCachedInfo scheduleIndexSave];
	}
	return info;
}

- (void)evict {
	NSOperation* op = [BNBlockOperation blockOperationWithBlock:^{
		NSString* objectPath = [self cachedObjectPath];
		NSError* error = nil;
		[[NSFileManager defaultManager] removeItemAtPath:objectPath error:&error];
		BNLog(@"Evicting %@", self.url.resourceSpecifier);
		if (error) {
			BNLog(@"Error deleting cached object: %@", error);
		}
	}];
	op.queuePriority = NSOperationQueuePriorityVeryLow;
	[s_storageQueue addOperation:op];
}

- (id)initWithURL:(NSURL*)url {
	self = [super init];
	if (self) {
		self.url = url;
		s_indexDictionary[url] = self;
		@synchronized (s_indexMRU) {
			[s_indexMRU addObject:self];
		
			while (s_indexMRU.count > MAX_STORAGE_ITEMS) {
				BNURLCachedInfo* oldestItem = [s_indexMRU objectAtIndex:0];
				[s_indexMRU removeObjectAtIndex:0];
				[s_indexDictionary removeObjectForKey:oldestItem.url];
				[oldestItem evict];
			}
		}
		[BNURLCachedInfo scheduleIndexSave];
	}
	return self;
}

- (id)initWithCoder:(NSCoder*)decoder {
	self = [super init];
	if (self) {
		self.url = [decoder decodeObject];
		self.expiryTime = [[decoder decodeObject] doubleValue];
		self.lastModified = [decoder decodeObject];
		self.etag = [decoder decodeObject];
		self.children = [decoder decodeObject];
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder*)coder {
	[coder encodeObject:self.url];
	[coder encodeObject:@(self.expiryTime)];
	[coder encodeObject:self.lastModified];
	[coder encodeObject:self.etag];
	[coder encodeObject:self.children];
}

- (NSString*)cachedObjectDir {
	return [s_cacheRoot stringByAppendingString:[NSString stringWithFormat:@"%@/%02d", self.url.host, (unsigned int)(self.url.path.hash % 100)]];
}
- (NSString*)cachedObjectPath {
	NSString* pathq = [self.url.path substringFromIndex:1];
	if (self.url.query) {
		pathq = [NSString stringWithFormat:@"%@_%@", pathq, self.url.query];
	}
	return [NSString stringWithFormat:@"%@/%@", [self cachedObjectDir], encodeString(pathq)];
}

- (void)loadCachedObjectWithBlock:(void (^)(id<NSCoding> cachedObject))completionBlock priority:(NSInteger)priority andFlags:(NSUInteger)flags {
	NSAssert([NSThread isMainThread], @"Wrong thread");
	
	// Check ram cache
	id<NSCoding> cachedObject = [s_ramCache objectForKey:self.url];
	if (cachedObject) {
		//NSLog(@"RAM Cache hit for %@", self.url.resourceSpecifier);
		completionBlock(cachedObject);
		return;
	}
	
	NSOperation* op = [BNBlockOperation blockOperationWithBlock:^{
		id<NSCoding> object = nil;
		@try {
			object = [NSKeyedUnarchiver unarchiveObjectWithFile:[self cachedObjectPath]];
		} @catch (NSException* ex) {
			NSLog(@"Warning: Exception while unarchiving object at %@: %@", [self cachedObjectPath], ex);
			NSError* error = nil;
			[[NSFileManager defaultManager] removeItemAtPath:[self cachedObjectPath] error:&error];
			//if (!error) {
				dispatch_async(dispatch_get_main_queue(), ^() {
					BNURLCachedInfo* info = self;
					[s_indexMRU removeObject:info];
					[s_indexDictionary removeObjectForKey:info.url];
				});
			//}
		}
		
		// Put in RAM cache unless this is a background prefetch (cos we don't want to fill RAM cache
		// with stuff that's relatively unlikely to be viewed soon).
		if (object && (0 == (flags & BNURLRequestFlagNoUpdateRamCache))) {
			[self updateRamCacheWithObject:object];
		}
		
		//NSLog(@"Cache load of %@ for URL %@ at pri %d", ((id<NSObject>)object).class, self.url, priority);
		dispatch_async(dispatch_get_main_queue(), ^() {
			completionBlock(object);
		});
	}];
	op.queuePriority = NSOperationQueuePriorityHigh - priority;
	[s_storageQueue addOperation:op];
}

- (void)cancelLoadRequest {
	// todo
}

- (void)updateRamCacheWithObject:(id<NSCoding>)object {
	NSUInteger cost = 50*1024; // a guess for typical average runtime cost of a BNContent instance
	if ([((id<NSObject>)object) isKindOfClass:[NSData class]]) {
		cost = ((NSData*)object).length;
	}
	[s_ramCache setObject:object forKey:self.url cost:cost];
}

- (void)updateCachedObject:(id<NSCoding>)cachedObject withHeaders:(NSDictionary*)headers andTTL:(NSInteger)ttl {
	
	// Work out the expiry date from the response headers
	self.expiryTime = 0;
	self.lastModified = headers[@"Last-Modified"];
	self.etag = headers[@"Etag"];
	NSString* dateStr = headers[@"Date"];

	if(dateStr && ttl > 1){
		NSDate *date = [s_dateFormatter dateFromString:dateStr];
		self.expiryTime = [[date dateByAddingTimeInterval:ttl] timeIntervalSinceReferenceDate];
	}
	else{
		NSString* cacheControl = [headers[@"Cache-Control"] lowercaseString];
		if (cacheControl && dateStr) {
			NSDate* date = [s_dateFormatter dateFromString:dateStr];
			NSRange range = [cacheControl rangeOfString:@"max-age="];
			if (range.location != NSNotFound) {
				cacheControl = [cacheControl substringFromIndex:range.location+range.length];
				NSInteger maxAge = [cacheControl intValue];
				self.expiryTime = [[date dateByAddingTimeInterval:maxAge] timeIntervalSinceReferenceDate];
			}
		}
	}
	
	// Write the processed object (if there is one) to storage
	if (cachedObject) {
		
		[self updateRamCacheWithObject:cachedObject];
		
		__block id<NSCoding> cachedObjectForBlock = cachedObject;
		
		NSOperation* op = [BNBlockOperation blockOperationWithBlock:^{
			NSError* error = nil;
			[[NSFileManager defaultManager] createDirectoryAtPath:[self cachedObjectDir]
									  withIntermediateDirectories:YES
													   attributes:nil
															error:&error];
			//NSLog(@"Storing %@ to %@", self.url, filepath);
			[NSKeyedArchiver archiveRootObject:cachedObjectForBlock toFile:[self cachedObjectPath]];
			// TODO: handle error
		}];
		op.queuePriority = NSOperationQueuePriorityLow;
		[s_storageQueue addOperation:op];
	}
	
	// Schedule an index-save
	[BNURLCachedInfo scheduleIndexSave];
}

+ (void)scheduleIndexSave {
	if (![NSThread isMainThread]) {
		dispatch_async(dispatch_get_main_queue(), ^{
			[self scheduleIndexSave];
		});
		return;
	}
	if (!s_indexSaveScheduled) {
		s_indexSaveScheduled = YES;
		[[BNURLCachedInfo class] performSelector:@selector(saveIndex) withObject:nil afterDelay:30.f];
	}
}

+ (void)appWillResignActive:(NSNotification*)notification {
	if (s_indexSaveScheduled) {
		[NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(saveIndex) object:nil];
		[self saveIndex];
	}
}

+ (void)saveIndex {
	NSOperation* op = [BNBlockOperation blockOperationWithBlock:^{
		NSLog(@"*** Saving BNUrlCache index ***");
		@synchronized(s_indexMRU) {
			[NSKeyedArchiver archiveRootObject:s_indexMRU toFile:s_indexFilePath];
		}
	}];
	op.queuePriority = NSOperationQueuePriorityVeryLow;
	[s_storageQueue addOperation:op];
	s_indexSaveScheduled = NO;
}

@end


