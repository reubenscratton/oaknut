//
//  BNDownloadBudget.h
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BNDownloadBudget : NSObject

@property (nonatomic) NSInteger maxMBperDay;

- (id)initWithDefaultsKey:(NSString*)key andPolicySetting:(NSInteger)maxMBperDay;
- (BOOL)exceeded;
- (void)deductBytes:(NSInteger)numBytes;
- (void)save;

@end
