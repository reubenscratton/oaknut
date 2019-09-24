//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>

#include "../../model/_module.h"
//#import "BNURLRequestManager.h"



#define BNURLRequestFlagForceUpdate 1        // Forcibly re-download data, i.e. don't complete with cache data and ignore TTL
#define BNURLRequestFlagNoUpdateRamCache 2    // For BNOfflineManager, used to avoids filling RAM cache with prefetch content


    
    /*- (void)start;
     - (id)processResponseDataInBackground:(NSData*)data error:(NSError*__autoreleasing*)error;
     - (void)onCompleteWithObject:(id<NSCoding>)object error:(NSError*)error isCacheData:(BOOL)isCacheData;
     - (void)cancel;
     */


class BNURLRequest : public Object {
public:
    
    static BNURLRequest* requestContent(const string& modelId, int flags, int priority);
    BNURLRequest(const string& url, int flags, int priority);
    void cancel();
    
    std::function<void(BNBaseModel*)> onHandleContent;
    
    enum Priority {
        High, //    0        // For anything on the screen
        Medium, //  1        // For stuff not on the screen but reasonably likely to be scrolled into view soon
        Low, //     2        // For speculative prefetching
        Offline // 3        // For BNOfflineManager only
    };

protected:
    static string urlForModelId(const string& modelId);

    
    enum Status {
        Created,
        Queued,
        Running,
        Inactive,
        Dead
    } status;
    
    enum Priority priority, priorityForRunQueues;
    int flags;
    class BNURLCachedInfo* cacheInfo;
    int retryCount;
    URLRequest* _req;
    //@property (nonatomic) NSURLSessionDataTask *dataTask;
    //@property (nonatomic) NSMutableArray* delegates;
    //@property (nonatomic) NSHTTPURLResponse* response;
    int ttl;
    
    // private to urlmanager
    bool hasDownloadedNewData;
    bool _cancelled;
};




