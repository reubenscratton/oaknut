//
//  emBeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#include "diskcontrols.h"

/*
extern NSString* s_docsDir;
NSString* kNotificationDiskControlsChanged = @"kNotificationDiskControlsChanged";
NSString* s_dir;
NSString* s_filepath;
NSMutableDictionary* s_jsonObj;





@interface DiskControlsRequest : SSJSONRequest
@end

@implementation DiskControlsRequest

- (id)processResponseDataInBackground:(NSData *)data error:(NSError *__autoreleasing *)error {
	NSDictionary* json = [super processResponseDataInBackground:data error:error];
	if (json) {
		return [[DiskControls alloc] initWithJson:json];
	}
	return nil;
}

@end
*/


class DiskControlsManager {
public:

	DiskControlsManager();
};

DiskControlsManager::DiskControlsManager() {
/*
	NSAssert(s_docsDir, @"Oops");
	s_dir = [s_docsDir stringByAppendingString:@"/Controls"];
	s_filepath = [s_dir stringByAppendingString:@"/controls.dat"];
	
	// Load stored data
	NSError* error = nil;
	NSData* data = [NSData dataWithContentsOfFile:s_filepath];
	if (data && data.length>0) {
		s_jsonObj = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers|NSJSONReadingMutableLeaves error:&error];
	}
	if (!s_jsonObj) {
		s_jsonObj = [NSMutableDictionary new];
	}*/
}


//void DiskControlsManager::onUrlRequestLoad(URLData* data) {
	//[self notifyDiskControlsLoaded:(DiskControls*)object diskId:(NSString*)request.context];
//}
/*+ (void)notifyDiskControlsLoaded:(DiskControls*)diskControls diskId:(NSString*)diskId {
	[[NSNotificationCenter defaultCenter] postNotificationName:kNotificationDiskControlsChanged object:diskId userInfo:@{@"diskControls":diskControls}];
}*/


static DiskControlsManager s_manager;







DiskControls::DiskControls() {
}

void DiskControls::fromVariant(const variant& v) {
	_controllers = v.arrayVal<Controller>("layouts");
}
void DiskControls::toVariant(variant& v) {
    variant array;
    for (auto& e : _controllers) {
        variant ev;
        e->toVariant(ev);
        array.appendVal(ev);
    }
    v["layouts"] = array;
}

Controller* DiskControls::initialController() {
	if (_controllers.size() > 0) {
		return _controllers[0];
	}
	return nullptr;
}

Controller* DiskControls::controllerById(const string& controllerId) {
	/*if ([controllerId isKindOfClass:[NSNumber class]]) {
		NSNumber* controllerIndex = (NSNumber*)controllerId;
		if (controllerIndex.integerValue < self.controllers.count) {
			return self.controllers[controllerIndex.integerValue];
		}
	}*/
	return NULL;
}



void DiskControls::loadDiskControlsForDiskId(const string& diskId) {
	/*
		// Use user-defined version if it exists
	NSDictionary* json = s_jsonObj[diskId];
	if (json) {
		DiskControls* diskControls = [[DiskControls alloc] initWithJson:json];
		[self notifyDiskControlsLoaded:diskControls diskId:diskId];
		return;
	}
	
    NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"http://www.ibeeb.co.uk/games/%@/ibeeb_controls.json", [diskId urlEncode]]];

    // If there's no controls in the cache, instantiate a copy of the defaults
    if (![SSURLCachedInfo cachedResponseForURL:url]) {
        NSData* defaultData = [NSData dataWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"default_controllers.json" ofType:nil]];
        json = [NSJSONSerialization JSONObjectWithData:defaultData options:NSJSONReadingMutableContainers error:nil];
        DiskControls* defaultControls = [[DiskControls alloc] initWithJson:json];
		[self notifyDiskControlsLoaded:defaultControls diskId:diskId];
    }
	
	[[SSURLRequestManager sharedInstance] requestURL:url delegate:(id<SSURLRequestDelegate>)self flags:SSURLRequestFlagCacheNeverExpires priority:SSDownloadPriorityHigh ttl:INT32_MAX requestClass:[DiskControlsRequest class] context:diskId];
*/
}
void DiskControls::setDiskControls(DiskControls* diskControls, const string& diskId) {
}
bool DiskControls::hasCustomControlsForDiskId(const string& diskId) {
	return false;// s_jsonObj[diskId] != nil;
}
void DiskControls::resetControlsForDiskId(const string& diskId) {
/*
	[s_jsonObj removeObjectForKey:diskId];
	[self saveCustomControls];
	[self loadDiskControlsForDiskId:diskId];
*/
}


/*
- (id)initWithCoder:(NSCoder*)decoder {
	return [self initWithJson:[decoder decodeObjectForKey:@"json"]];
}

- (void)encodeWithCoder:(NSCoder*)coder {
	[coder encodeObject:[self toJson] forKey:@"json"];
}

- (instancetype)copy {
	return [[DiskControls alloc] initWithJson:[self toJson]];
}

+ (void)setDiskControls:(DiskControls*)diskControls forDiskId:(NSString*)diskId {
	s_jsonObj[diskId] = [diskControls toJson];
	[self saveCustomControls];
	[self notifyDiskControlsLoaded:diskControls diskId:diskId];
}

+ (void)saveCustomControls {
	NSError* error = nil;
	NSData* data = [NSJSONSerialization dataWithJSONObject:s_jsonObj options:NSJSONWritingPrettyPrinted error:&error];
	if (error == nil) {
		[[NSFileManager defaultManager] createDirectoryAtPath:s_dir
								  withIntermediateDirectories:YES
												   attributes:nil
														error:nil];
		[data writeToFile:s_filepath atomically:YES];
	}
}
*/
