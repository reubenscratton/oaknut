//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "urlrequest.h"
#include "../../policy/policy.h"
/*#import "BNContent.h"
#import "BNMyNewsCollection.h"
#import "BNCollections.h"
#import "BNItem.h"
#import "BNURLCachedInfo.h"
#import "BNContentView.h"
#import "BBCNEndpoint.h"
*/


string BNURLRequest::urlForModelId(const string& modelId) {
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

BNURLRequest* BNURLRequest::requestContent(const string& modelId, int flags, int priority) {
	
	// Special case for my news
	if (modelId.hasPrefix(BNModelIdMyNews)) {
        assert(0); // not sure what to do here
		//[[BNMyNewsCollection sharedInstance] addDelegate:delegate];
		//return;
	}
	
    return new BNURLRequest(urlForModelId(modelId), flags, priority);
}

static void fixRedundantJson(variant&json) {
    if (json.isString()) {
        string& str = json.stringRef();
        str.hadPrefix("bbc.mobile.news");
    } else if (json.isCompound()) {
        for (auto& e: json.compoundRef()) {
            fixRedundantJson(e.second);
        }
    } else if (json.isArray()) {
        for (auto& e : json.arrayRef()) {
            fixRedundantJson(e);
        }
    }
}


BNURLRequest::BNURLRequest(const string& url, int flags, int priority) {
	
	// Use normal content TTL for everything except LEP commentaries, which should update every minute
    int ttl = BNPolicy::current()->_endpointContent->_ttl;
	if (url.contains("/commentary/")) {
		ttl = 60;
	}
	
    _req = URLRequest::get(url);
    _req->setHeader("User-Agent", BNPolicy::current()->userAgent());
    
    // Special background processing used to trim the BBC's highly redundant JSON and to convert into a model obj
    _req->customDecoder = [=] (URLResponse* response) -> bool {
        auto& json = response->decoded.json;
        json = variant::parse(response->data.toString(), PARSEFLAG_JSON);
        if (json.isError()) {
            json.clear();
            return false;
        }
        if (json.hasVal("type")) {
            fixRedundantJson(json);
            BNBaseModel* modelObj = BNBaseModel::createModelObjectFromJson(json);
            Task::postToMainThread([=]() {
                modelObj->retain();
                if (!_cancelled) {
                    onHandleContent(modelObj);
                }
                modelObj->release();
            });
        }
            
        return true; // i.e. no default processing
    };

}

void BNURLRequest::cancel() {

    /*if ([modelId hasPrefix:modelIdMyNews]) {
		[[BNMyNewsCollection sharedInstance] removeDelegate:delegate];
		return;
	}*/
    _cancelled = true;
    if (_req) {
        _req->cancel();
        _req = NULL;
    }
}

/*
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





