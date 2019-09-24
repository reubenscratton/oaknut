//
//  BNMyNewsCollection.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNMyNewsCollection.h"
#import "BNCollections.h"
#import "BNContentRequest.h"
#import "BNRelationship.h"
#import "BNItem.h"
#import "BNContentRequest.h"
#import "BNContentStub.h"

@interface BNMyNewsCollection () <BNURLRequestDelegate>

@property (nonatomic) NSMutableArray *delegates;
@property (nonatomic) NSMutableDictionary *contentObjects;

@property (nonatomic) NSDate *timeOfLastDedupe;
@property (nonatomic) dispatch_queue_t dedupeQueue;

@end

@implementation BNMyNewsCollection

static BNMyNewsCollection *s_sharedInstance;

+ (void)staticInit {
	s_sharedInstance = [BNMyNewsCollection new];
}
+ (instancetype)sharedInstance {
	NSAssert(s_sharedInstance, @"staticInit error");
	return s_sharedInstance;
}

- (id)init {
	self = [super initWithId:modelIdMyNews];
	if (self) {
		self.dedupeQueue = dispatch_queue_create("bbc.news.mynews.dedupe.queue", NULL);
		self.delegates = [NSMutableArray new];
		
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onFollowStateChanged:) name:BNNotificationFollowStateChanged object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onFollowedTopicReordered:) name:BNNotificationFollowedTopicReordered object:nil];
		
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onFollowedTopicChanged:) name:BNNotificationFollowedTopicChanged object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onFollowedTopicDeleted:) name:BNNotificationFollowedTopicDeleted object:nil];

		self.timeOfLastDedupe = [NSDate distantPast];
		[self reload];
	}
	return self;
}

- (void)addDelegate:(id<BNURLRequestDelegate>)delegate {
	[self.delegates addObject:delegate];
	[delegate onRequestLoadedObject:nil object:self isCacheData:NO];
}
- (void)removeDelegate:(id<BNURLRequestDelegate>)delegate {
	[self.delegates removeObject:delegate];
}

- (void)reload {
	NSArray* stubs = [BNCollections followedCollections];
	self.contentObjects = [NSMutableDictionary new];
	for (BNContentStub* stub in stubs) {
		[BNContentRequest request:stub.modelId delegate:self flags:0 priority:BNDownloadPriorityMedium];
	}
	[self notify];
}

- (void)onFollowStateChanged:(NSNotification*)notification {
	BNContentStub* stub = notification.userInfo[@"stub"];
	if ([BNCollections isFollowed:stub]) {
		[BNContentRequest request:stub.modelId delegate:self flags:0 priority:BNDownloadPriorityMedium];
	} else {
		[BNContentRequest unrequest:stub.modelId delegate:self];
		[self.contentObjects removeObjectForKey:stub.modelId];
		[self notify];
	}
}

- (void)onFollowedTopicReordered:(NSNotification*)notification {
	[self reload];
}

- (void)onFollowedTopicChanged:(NSNotification*)notification {
	[self reload];
}
- (void)onFollowedTopicDeleted:(NSNotification*)notification {
	[self reload];
}


- (void)notify {
	NSTimeInterval timeSinceLastDedupe = [[NSDate date] timeIntervalSinceDate:self.timeOfLastDedupe];
	if (timeSinceLastDedupe > 1) {
		[self doDedupe];
	} else {
		[NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(doDedupe) object:nil];
		[self performSelector:@selector(doDedupe) withObject:nil afterDelay:1.f];
	}
}

- (void)doDedupe {
	self.timeOfLastDedupe =  [NSDate date];
	self.lastUpdated = self.timeOfLastDedupe.timeIntervalSince1970;
	NSArray* collections = [self.contentObjects allValues];
	NSArray* followedCollections = [BNCollections followedCollections].copy;
    dispatch_async(self.dedupeQueue, ^() {
#ifndef DEBUG
		@try {
#endif
			[self dedupe:collections followedCollections:followedCollections];
#ifndef DEBUG
		}
		@catch (NSException* ex) {
			NSLog(@"Exception deduping My News: %@", ex);
		}
#endif
    });
}

- (void)dedupe:(NSArray*)collections followedCollections:(NSArray*)followedCollections {
    
    NSMutableDictionary *dedupedCollectionDict = [NSMutableDictionary new];
    NSMutableSet *setOfAllChildObjects = [NSMutableSet new];
    
    // Iterate over the content we've successfully fetched
    for (BNCollection *collection in collections) {
		if (![collection isKindOfClass:[BNCollection class]]) {
			continue;
		}
		
		// Get the set of unique child relationships, i.e. those that aren't in a previously considered collection
        NSMutableArray *dedupedRelationships = [NSMutableArray new];
        for (BNRelationship *childRelationship in collection.childRelationships) {
            BNBaseModel *childObject = childRelationship.childObject;
            if (![setOfAllChildObjects containsObject:childObject]) {
				[setOfAllChildObjects addObject:childRelationship.childObject];
				[dedupedRelationships addObject:childRelationship];
            }
        }
		
		// Create a copy of the collection that only contains the unique relationships
        BNCollection *dedupedCollection = [[BNCollection alloc] initWithTypeAndId:BNModelType.collection modelId:collection.modelId];
		dedupedCollection.name = collection.name;
		dedupedCollection.format = collection.format;
        dedupedCollection.childRelationships = dedupedRelationships;
        [dedupedCollectionDict setObject:dedupedCollection forKey:collection.modelId];
    }
	
	// Order the deduped collections w.r.t. the followed collections ordering
	NSMutableArray* dedupedOrdered = [[NSMutableArray alloc] initWithCapacity:dedupedCollectionDict.count];
	for (BNContentStub* stub in [BNCollections followedCollections]) {
		BNCollection* collection = dedupedCollectionDict[stub.modelId];
		if (collection) {
			[dedupedOrdered addObject:collection];
		}
	}
    self.collectionsDeduped = dedupedOrdered;
	
	// Notify UI on main thread
    dispatch_async(dispatch_get_main_queue(), ^() {
        for (id<BNURLRequestDelegate> delegate in self.delegates) {
			[delegate onRequestLoadedObject:nil object:self isCacheData:NO];
        }
    });
}

- (NSMutableArray*)findRelationshipsByPrimaryTypes:(NSArray*)primaryTypes andSecondaryTypes:(NSArray*)secondaryTypes andFormats:(NSArray*)formats {
	NSMutableArray* matches = [[NSMutableArray alloc] init];
	for (BNContent* content in self.collectionsDeduped) {
		NSArray* submatches = [content findRelationshipsByPrimaryTypes:primaryTypes andSecondaryTypes:secondaryTypes andFormats:formats];
		[matches addObjectsFromArray:submatches];
	}
	return matches.mutableCopy;
}

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}


- (void)onRequestLoadedObject:(BNURLRequest*)request object:(id)object isCacheData:(BOOL)isCacheData {
	if ([object isKindOfClass:[BNContent class]]) {
		BNContent* content = (BNContent*)object;
		if (content.modelId) {
			[self.contentObjects setObject:content forKey:content.modelId];
			[self notify];
		}
	}
}

-(void)onRequestError:(BNURLRequest *)request error:(NSError *)error httpStatus:(NSInteger)httpStatus {
	
}


- (void)update:(BOOL)force {}

- (BOOL)isEmpty {
	//return self.collectionsDeduped.count==0;
	return [BNCollections followedCollections].count == 0;
}

@end
