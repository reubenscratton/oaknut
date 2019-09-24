//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "layoutsgroupmanager.h"
#include "layoutsgroup.h"
/*
#import "BNURLRequestManager.h"
#import "BNContentRequest.h"
#import "BNAppDelegate.h"
#import "BNPolicyManager.h"
#import "BNPolicy.h"
#import "BBCNEndpoint.h"
#import "BNStyles.h"
#import "BNDeviceSpec.h"
*/

//NSString* BNNewLayoutsNotification = @"BNNewLayoutsNotification";



/*
@interface BNLayoutsZipURLRequest : BNURLRequest
@end

@implementation BNLayoutsZipURLRequest

- (id)processResponseDataInBackground:(NSData *)data error:(NSError *__autoreleasing *)error {
	NSFileManager* fm = [NSFileManager defaultManager];
	
	// Write .zip data to a temporary file so ZipArchive can handle it.
	NSString* tempZipPath = [NSTemporaryDirectory() stringByAppendingPathComponent:@"layouts.zip"];
	if (![data writeToURL:[NSURL fileURLWithPath:tempZipPath] options:NSDataWritingAtomic error:error]) {
		BNLog(@"Error: failed to write downloaded layouts.zip to temporary file");
		return nil;
	}
			
	// Create a unique place in the documents dir
	NSString* newLayoutsDirStem = [[NSProcessInfo processInfo] globallyUniqueString];
	NSString* newLayoutsDir = [[BNAppDelegate appSupportDir] stringByAppendingPathComponent:newLayoutsDirStem];
	if (![fm createDirectoryAtPath:newLayoutsDir withIntermediateDirectories:YES attributes:nil error:error]) {
		BNLog(@"Error: failed to create a new layouts dir!");
		[fm removeItemAtPath:tempZipPath error:nil];
		return nil;
	}
	
	return nil;
}


@end

*/


/*

- (void)onRequestLoadedObject:(BNURLRequest *)request object:(id)object isCacheData:(BOOL)isCacheData {

	// Ignore cached data. Downloaded layout path is kept in settings.
	if (isCacheData) {
		return;
	}
	
	// Sanity check. The downloader should be returning us a path.
	if (!object || ![object isKindOfClass:[NSString class]]) {
		return;
	}

	NSString* newLayoutsDirStem = (NSString*)object;
	NSString* oldLayoutsDirStem = [[NSUserDefaults standardUserDefaults] stringForKey:LAYOUT_SETTING_KEY];
	if ([newLayoutsDirStem isEqualToString:oldLayoutsDirStem]) {
		return;
	}

	// Create a new layouts group from the layouts in the dir
	NSString* newLayoutsDir = [[BNAppDelegate appSupportDir] stringByAppendingPathComponent:newLayoutsDirStem];
	BNLayoutsGroup* newLayoutsGroup = [[BNLayoutsGroup alloc] initWithDir:newLayoutsDir];
	if (!newLayoutsGroup.isValid) {
		BNLog(@"Error: Downloaded layouts are not valid");
		[[NSFileManager defaultManager] removeItemAtPath:newLayoutsDir error:nil];
		return;
	}
	
	// New layouts! Lets *really* hope they work!
	if (oldLayoutsDirStem) {
		NSString* oldLayoutDir = [[BNAppDelegate appSupportDir] stringByAppendingPathComponent:oldLayoutsDirStem];
		[[NSFileManager defaultManager] removeItemAtPath:oldLayoutDir error:nil];
	}
	[[NSUserDefaults standardUserDefaults] setObject:newLayoutsDirStem forKey:LAYOUT_SETTING_KEY];
	[[NSUserDefaults standardUserDefaults] synchronize];
	self.currentLayouts = newLayoutsGroup;
	BNLog(@"New layouts loaded");
	
	// Broadcast an event that forces all content views to reload
	[[NSNotificationCenter defaultCenter] postNotificationName:BNNewLayoutsNotification object:nil];
}

-(void)onRequestError:(BNURLRequest *)request error:(NSError *)error httpStatus:(NSInteger)httpStatus{
	
}


@end
*/
