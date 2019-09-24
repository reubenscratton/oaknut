//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

class BNCollections {
public:

    static vector<BNContent::stub> rootCollections;
    static vector<BNContent::stub> followedCollections;
    static void checkForUpdatedFollowedTopics(const BNContent::stub& stub);
    static bool isFollowed(const BNContent::stub& stub);
    static void toggleFollowState(const BNContent::stub& stub);
    static void moveFollowedCollectionFrom(int fromIndex, int toIndex);
    static bool isRootCollection(const string& modelId);
    static void applyTopicOverrides(const vector<string>& topicOverrides);
    static void save();

};

/**
 Notifications.
 */

//extern NSString* BNNotificationFollowStateChanged;
//extern NSString* BNNotificationFollowedTopicReordered;
//extern NSString* BNNotificationFollowedTopicChanged;
//extern NSString* BNNotificationFollowedTopicDeleted;
