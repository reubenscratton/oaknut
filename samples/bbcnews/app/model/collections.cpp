//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "_module.h"
#include "../policy/policy.h"
//#import "BNStyles.h"
//#import "BNNavigationController.h"
//#import "BNAManager.h"
//#import "BNWalkThroughStatsControl.h"
//#import "BNSearchHelper.h"
//#import "BNFileManager.h"


// Root collections
vector<BNContent::stub> BNCollections::rootCollections = [] {
    vector<BNContent::stub> roots = {
        BNContent::stub(BNModelIdLive, "LIVE", BNContentFormatVideo),
        BNContent::stub(BNModelIdTopStories, "Top Stories"),
        BNContent::stub(BNModelIdMyNews, "My News"),
    };
    if (0/*app->isTablet()*/) {
        roots.push_back(BNContent::stub(BNModelIdMostPopular, "Most Popular"));
    } else {
        roots.push_back(BNContent::stub(BNModelIdMostRead, "Most Read"));
        roots.push_back(BNContent::stub(BNModelIdMostWatched, "Most Watched"));
    }
    return roots;
}();

vector<BNContent::stub> BNCollections::followedCollections = [] {
    // Followed collections
    /*NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    s_followedDatFilePath = [NSString stringWithFormat:@"%@/followed.dat", documentsDirectory];
    s_followedCollections = [[NSMutableArray alloc] init];
    [s_followedCollections addObjectsFromArray:[NSKeyedUnarchiver unarchiveObjectWithFile:s_followedDatFilePath]];*/
    app->log("TODO: load followed collections");
    return vector<BNContent::stub>();
} ();
static string s_followedDatFilePath;

//NSString* BNNotificationFollowStateChanged = @"BNNotificationFollowStateChanged";
//NSString* BNNotificationFollowedTopicReordered = @"BNNotificationFollowedTopicReordered";
//NSString* BNNotificationFollowedTopicChanged = @"BNNotificationFollowedTopicChanged";
//NSString* BNNotificationFollowedTopicDeleted = @"BNNotificationFollowedTopicDeleted";


bool BNCollections::isFollowed(const BNContent::stub& stub) {
    for (BNContent::stub& fstub : followedCollections) {
        if (fstub.modelId == stub.modelId) {
            return true;
        }
    }
    return false;
}

void BNCollections::toggleFollowState(const BNContent::stub& stub) {

    bool found = false;
    for (int i=0 ; i<followedCollections.size() ; i++) {
        BNContent::stub& fstub = followedCollections.at(i);
        if (fstub.modelId == stub.modelId) {
            followedCollections.erase(followedCollections.begin()+i);
            found = true;
            break;
        }
    }
    //int index=0;
    if (!found) {
        if (followedCollections.size() >= BNPolicy::current()->_maxTopicsFollowed) {
            //TODO [[BNNavigationController get] showSimpleAmbientNotification:@"You're following the maximum number of topics. Please remove some from 'Manage' first." timeout:4.f];
            return;
        }
        followedCollections.push_back(stub);
        
        /* TODO
        index = [s_followedCollections indexOfObject:stub];
        
        NSString* message = [NSString stringWithFormat:@"%@ added to My News", stub.name];
        
        BNAmbientNotification* ambientNotification;
        
        if([BBCNFeatureController tabBarEnabled]){
            ambientNotification = [[BNAmbientNotification alloc] initWithText:message
                                                                   buttonText:@"Undo"
                                                                   andTimeout:4.f];
            
            ambientNotification.buttonTappedBlock = ^() {
                [BNCollections toggleFollowState:stub];
            };
        }else{
            ambientNotification = [[BNAmbientNotification alloc] initWithText:message
                                                                   buttonText:@"X"
                                                                   andTimeout:4.f];
        }
        [[BNNavigationController get] showAmbientNotification:ambientNotification];*/
    }
//    [self save];
//    [[NSNotificationCenter defaultCenter] postNotificationName:BNNotificationFollowStateChanged object:nil userInfo:@{@"stub":stub, @"index":@(index)}];
}



void BNCollections::applyTopicOverrides(const vector<string>& topicOverrides) {
	
    app->log("TODO:applyTopicOverrides");
	
    // Iterate over all the overrides
	/*bool changedFollowedCollections = false;
	
	for (NSDictionary* topicOverride in topicOverrides) {
		
		// Decode the override
		NSString *oldId = topicOverride[@"id_old"];
		
		BNContentStub *updatedStub = [[BNContentStub alloc] initWithID:topicOverride[@"id"] name:topicOverride[@"name"]];
		
		// Is the override one of our followed topics?
		for (int i=0 ; i<s_followedCollections.count ; i++)
		{
			BNContentStub *followedStub = s_followedCollections[i];
	
			if ([followedStub.modelId isEqual:oldId]) {
				
				// If there's no new ID - OR there is a new ID but it's already being followed - then just unfollow
				if (!updatedStub.modelId || [s_followedCollections containsObject:updatedStub])
				{
					[s_followedCollections removeObjectAtIndex:i];
					[[NSNotificationCenter defaultCenter] postNotificationName:BNNotificationFollowedTopicDeleted object:nil userInfo:@{@"stub":followedStub, @"index":@(i)}];
					i--;
				}
				else
				{
					// Otherwise, update the followed topic with the new ID and (optionally) the new name.
					BNContentStub *modifiedStub = [BNContentStub contentStubWithID:updatedStub.modelId
																			 name:(updatedStub.name) ? updatedStub.name : followedStub.name
																		   format:followedStub.format];
					s_followedCollections[i] = modifiedStub;
					
					[self notifyFollowedTopicChanged:modifiedStub index:i];
				}
				changedFollowedCollections = true;
			}
		}
		
		// Update search suggestions
		[[BNSearchHelper sharedInstance] updateWithStub:updatedStub oldId:oldId];
	}
	
	if (changedFollowedCollections)
	{
		[BNCollections save];
	}*/

}



/*+ (void)notifyFollowedTopicChanged:(BNContentStub*)stub index:(NSUInteger)index
{
	[[NSNotificationCenter defaultCenter] postNotificationName:BNNotificationFollowedTopicChanged object:nil userInfo:@{@"stub":stub, @"index":@(index)}];
}

+ (void)checkForUpdatedFollowedTopics:(BNContentStub *)stub
{
	dispatch_async(dispatch_get_main_queue(), ^{
		if ([BNCollections isFollowed:stub]) {
			NSUInteger index = [s_followedCollections indexOfObject:stub];
			BNContentStub *existing = [s_followedCollections objectAtIndex:index];
			if (![existing.name isEqualToString:stub.name]) {
				[s_followedCollections replaceObjectAtIndex:index withObject:stub];
				[self notifyFollowedTopicChanged:stub index:index];
				[self save];
			}
		}
	});
}*/



void BNCollections::save() {
    app->log("TODO: BNCollections::save()");
    /*
	dispatch_async(dispatch_get_main_queue(), ^() {
		NSArray *copyCollections = [s_followedCollections copy];
		dispatch_async(dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
			
			// Save to App Sandbox
			[NSKeyedArchiver archiveRootObject:copyCollections toFile:s_followedDatFilePath];
			
			// Save a copy to App Group Sandbox (so watch extension can access)
			NSString *followedCollectionsSharedFilePath = [[[BNFileManager sharedCachesDirectory] URLByAppendingPathComponent:@"followed.dat"] path];
			
			// Convert to BNContentStubs for watch.
			NSMutableArray *followedCollections = [[NSMutableArray alloc] init];
			for (NSInteger i = 0; i < copyCollections.count; i++) {
				if ([copyCollections[i] isKindOfClass:[BNContentStub class]]) {
					BNContentStub* initalStub = (BNContentStub*)copyCollections[i];
					BNContentStub* safeStub = [BNContentStub contentStubWithID:initalStub.modelId name:initalStub.name];
					[followedCollections addObject:safeStub];
				}
			}
			
			[NSKeyedArchiver archiveRootObject:[followedCollections copy] toFile:followedCollectionsSharedFilePath];
		});
	});*/
}

/*
+ (void)moveFollowedCollectionFrom:(NSInteger)fromIndex to:(NSInteger)toIndex
{
	BNContentStub* stub = [s_followedCollections objectAtIndex:fromIndex];
	[s_followedCollections removeObjectAtIndex:fromIndex];
	[s_followedCollections insertObject:stub atIndex:toIndex];
	[[NSNotificationCenter defaultCenter] postNotificationName:BNNotificationFollowedTopicReordered object:nil userInfo:@{@"stub":stub}];
	[self save];
}


+ (BOOL)isRootCollection:(NSString*)modelId {
	for (BNContentStub* rootStub in s_rootCollections) {
		if ([rootStub.modelId isEqualToString:modelId]) {
			return YES;
		}
	}
	if ([modelId isEqualToString:modelIdMostPopular]) {
		return YES;
	}
	return NO;
}
*/

