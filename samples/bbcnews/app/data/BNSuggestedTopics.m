//
//  BNSuggestedTopics.m
//  BBCNews
//
//  Copyright (c) 2015 BBC News. All rights reserved.
//

#import "BNSuggestedTopics.h"
#import "BNContentRequest.h"
#import "BNSuggestionsGroup.h"
#import "BNContentStub.h"
#import "BNCollections.h"


@interface BNSuggestedTopicsManager : NSObject <BNURLRequestDelegate>

+ (instancetype)sharedInstance;

@property (nonatomic) BNSuggestedTopics *suggestedTopics;

@end

@implementation BNSuggestedTopics

+ (instancetype)currentData {
	return [[BNSuggestedTopicsManager sharedInstance] suggestedTopics];
}

- (id)initWithJson:(NSDictionary*)json {
	self = [super init];
	if (self) {
		self.groups = [NSMutableArray new];
		NSArray* jsections = json[@"sections"];
		for (NSDictionary* jsection in jsections) {
			BNSuggestionsGroup* group = [BNSuggestionsGroup new];
			group.title = jsection[@"name"];
			NSArray* jtopics = jsection[@"topics"];
			for (NSDictionary* jtopic in jtopics) {
				BNContentStub* stub = [BNContentStub contentStubWithID:jtopic[@"contentId"] name:jtopic[@"name"]];
				[group addSuggestion:stub];
			}
			[self.groups addObject:group];
		}
	}
	return self;
}

@end





@implementation BNSuggestedTopicsManager

static BNSuggestedTopicsManager *s_sharedInstance;

+ (void)staticInit {
	s_sharedInstance = [BNSuggestedTopicsManager new];
}
+ (instancetype)sharedInstance {
	NSAssert(s_sharedInstance, @"staticInit error");
	return s_sharedInstance;
}


- (id)init {
	self = [super init];
	if (self) {
		
		// Load the default file
		NSString *jsonPath = [[NSBundle mainBundle] pathForResource:@"followtopics" ofType:@"json"];
		NSData *indexesData = [NSData dataWithContentsOfFile:jsonPath];
		NSDictionary* json = [NSJSONSerialization JSONObjectWithData:indexesData options:NSJSONReadingAllowFragments error:nil];
		[self importJson:json];

		// Request static file updates
		[BNContentRequest request:@"/static/followtopics" delegate:self flags:0 priority:BNDownloadPriorityLow];
	}
	return self;
}

- (void)importJson:(NSDictionary*)json {

	NSArray* topicOverrides = json[@"topic_overrides"];
	if (topicOverrides) {
		[BNCollections applyTopicOverrides:topicOverrides];
	}
	
	self.suggestedTopics = [[BNSuggestedTopics alloc] initWithJson:json];

}


-(void)onRequestStatusChanged:(BNURLRequest *)request{
	
}

-(void)onRequestLoadedObject:(BNURLRequest *)request object:(id)object isCacheData:(BOOL)isCacheData{
	NSDictionary* json = (NSDictionary*)object;
	[self importJson:json];
}

-(void)onRequestError:(BNURLRequest *)request error:(NSError *)error httpStatus:(NSInteger)httpStatus{
	
}


@end