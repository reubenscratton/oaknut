//
//  BNJSONRequest.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNJSONRequest.h"

#ifdef DEBUG
//#define BAD_SERVER
#endif

#define BADNESS 50

@implementation BNJSONRequest



- (id)processResponseDataInBackground:(NSData*)data error:(NSError*__autoreleasing*)error {
	NSAssert(![NSThread isMainThread], @"Wrong thread");

	// Deserialize the JSON data into a NSDictionary
	NSDictionary* json = [NSJSONSerialization JSONObjectWithData:data
			options:NSJSONReadingAllowFragments|NSJSONReadingMutableContainers error:error];
#ifdef BAD_SERVER
	if ([self.URL.absoluteString rangeOfString:@"policy?"].location == NSNotFound) {
		json = [self badify:json];
	}
#endif
	return json;
}

- (NSDictionary *)allHTTPHeaderFields {
	NSMutableDictionary *mutableHeaders = [super allHTTPHeaderFields].mutableCopy;
	mutableHeaders[@"Accept"] = @"application/json";
	return mutableHeaders;
}


#ifdef BAD_SERVER

- (NSDictionary*)badify:(NSDictionary*)json {
	NSMutableDictionary* mjson = json.mutableCopy;
	for (NSString* key in mjson.allKeys) {
		NSInteger randomNumber = arc4random() % 100;
		if (randomNumber < BADNESS) {
			continue;
		}
		id<NSObject> value = mjson[key];
		id<NSObject> badvalue = [self badValue:value];
		if (badvalue) {
			mjson[key] = badvalue;
		} else {
			[mjson removeObjectForKey:key];
		}
	}
	return mjson;
}

- (id<NSObject>)badValue:(id<NSObject>)value {

	if ([value isKindOfClass:[NSNumber class]]) {
		switch (arc4random() % 4) {
			case 0:
				return @(arc4random());
			case 1:
			case 2:
				return @"foo";
			case 3:
				return nil;
		}
	}
 
	if ([value isKindOfClass:[NSString class]]) {
		switch (arc4random() % 5) {
			case 0:
				return [NSString stringWithFormat:@"%d", arc4random()];
			case 1:
				return [NSString stringWithFormat:@"1%@", value];
			case 2:
				return [NSString stringWithFormat:@"%@2", value];
			case 3:
				return @"null";
			case 4:
				return nil;
		}
	}
	
	if ([value isKindOfClass:[NSDictionary class]]) {
		switch (arc4random() % 5) {
			case 0:
			case 1:
			case 2:
			case 3:
				return [self badify:(NSDictionary*)value];
			case 4:
				return nil;
		}
	}
	
	if ([value isKindOfClass:[NSArray class]]) {
		NSMutableArray* a = ((NSArray*)value).mutableCopy;
		for (int i=0 ; i<a.count ; i++) {
			switch (arc4random() % 5) {
				case 0:
				case 1:
				case 2:
				case 3:
					value = [self badValue:a[i]];
					if (value) {
						a[i] = value;
					} else {
						[a removeObjectAtIndex:i--];						
					}
					break;
				case 4:
					[a removeObjectAtIndex:i--];
					break;
			}
			
		}
		return a;
	}
	
	NSAssert(0, @"json type not badified");
	return nil;
}
#endif

@end
