//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "../../policy/policy.h"
#include "contentrequest.h"
/*#import "BNContent.h"
#import "BNMyNewsCollection.h"
#import "BNCollections.h"
#import "BNItem.h"
#import "BNURLCachedInfo.h"
#import "BNContentView.h"
#import "BBCNEndpoint.h"
*/


string BNContentRequest::urlForModelId(const string& modelId) {
	if (!modelId.length()) {
		return "";
	}
	
	// Get suitable endpoint
    string endpoint = BNPolicy::current()->_endpointContent->getHREF();
	if (modelId.hasPrefix("?")) {
		endpoint = BNPolicy::current()->_endpointPolicy->getHREF();
	}

	if (modelId == BNModelIdMostRead || modelId == BNModelIdMostWatched) {
        return endpoint + BNModelIdMostPopular;
	}
	
	return endpoint + modelId;
	
}

BNContentRequest* BNContentRequest::request(const string& modelId, int flags, int priority) {
	
	// Special case for my news
	if (modelId.hasPrefix(BNModelIdMyNews)) {
        assert(0); // not sure what to do here
		//[[BNMyNewsCollection sharedInstance] addDelegate:delegate];
		//return;
	}
	
    return requestURL(urlForModelId(modelId), flags, priority);
}

BNContentRequest* BNContentRequest::requestURL(const string& url, int flags, int priority) {
	
	// Use normal content TTL for everything except LEP commentaries, which should update every minute
    int ttl = BNPolicy::current()->_endpointContent->_ttl;
	if (url.contains("/commentary/")) {
		ttl = 60;
	}
	
    auto req = URLRequest::get(url);
	// Lookup an existing request or create a new one. In the ideal caching situation this can
	// complete synchronously, i.e. delegate will run before this function returns.
	/*[[BNURLRequestManager sharedInstance] requestURL:url delegate:delegate flags:flags priority:priority ttl:ttl creatorBlock:^BNURLRequest *{
		BNContentRequest* req = [[BNContentRequest alloc] initWithURL:url];
		return req;
	}];*/
    return NULL;
}

/*+ (void)unrequest:(NSString*)modelId delegate:(id<BNURLRequestDelegate>)delegate {
	if ([modelId hasPrefix:modelIdMyNews]) {
		[[BNMyNewsCollection sharedInstance] removeDelegate:delegate];
		return;
	}
	[[BNURLRequestManager sharedInstance] unrequestURL:[self urlForModelId:modelId] delegate:delegate];
}


- (id)processResponseDataInBackground:(NSData *)data error:(NSError*__autoreleasing*)error {
	
	NSDictionary* json = [super processResponseDataInBackground:data error:error];
	if (json) {
		if (json[@"killSwitch"]) {
			return [[BNPolicy alloc] initWithJSONDictionary:json];
		}
		if (json[@"type"]) {
			return [BNBaseModel createModelObjectFromJsonDictionary:json];
		}
	}
	return json;
}



- (void)updateCacheInfoChildren:(id<NSCoding>)object {
	
	// Get items, sorted by date desc
	BNContent* content = (BNContent*)object;
	if (![content isKindOfClass:[BNCollection class]]) {
		return; // TODO: items...
	}
	NSArray* items = [content findChildrenByPrimaryTypes:@[BNModelType.item] andSecondaryTypes:nil andFormats:nil];
	 items = [items sortedArrayUsingComparator:^NSComparisonResult(BNItem* obj1, BNItem* obj2) {
	 return obj2.lastUpdated-obj1.lastUpdated;
	 }];
	
	// Update the collection's cache record's children to be all the modelIDs of the items in the collection
	NSMutableArray* childModelIds = [[NSMutableArray alloc] initWithCapacity:items.count];
	for (BNContent* item in items) {
		if (item.modelId) {
			[childModelIds addObject:item.modelId];
		}
	}
	self.cacheInfo.children = childModelIds;
}

*/





