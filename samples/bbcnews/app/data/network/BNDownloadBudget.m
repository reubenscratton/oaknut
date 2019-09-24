//
//  BNDownloadBudget.m
//  BBCNews
//
//  Copyright (c) 2014 BBC News. All rights reserved.
//

#import "BNDownloadBudget.h"

@interface BNDownloadBudget ()

@property (nonatomic) NSString* defaultsKey;
@property (nonatomic) NSDate* dateOfFirstDownload;
@property (nonatomic) NSInteger downloadedByteCount;

@end

@implementation BNDownloadBudget

- (id)initWithDefaultsKey:(NSString*)key andPolicySetting:(NSInteger)maxMBperDay {
	self = [super init];
	if (self) {
		self.defaultsKey = key;
		self.maxMBperDay = maxMBperDay;
		self.dateOfFirstDownload = [[NSUserDefaults standardUserDefaults] objectForKey:[key stringByAppendingString:@"dateOfFirstDownload"]];
		self.downloadedByteCount = [[NSUserDefaults standardUserDefaults] integerForKey:[key stringByAppendingString:@"downloadedByteCount"]];
		[self checkDate];
	}
	return self;
}

- (void)checkDate {
	NSTimeInterval age = -[self.dateOfFirstDownload timeIntervalSinceNow];
	if (age >= (24*60*60)) {
		self.dateOfFirstDownload = nil;
		self.downloadedByteCount = 0;
	}
}

- (BOOL)exceeded {
	[self checkDate];
	return self.downloadedByteCount >= self.maxMBperDay*1024*1024;
}

- (void)deductBytes:(NSInteger)numBytes {
	if (!self.dateOfFirstDownload) {
		self.dateOfFirstDownload = [NSDate new];
	}
	self.downloadedByteCount += numBytes;
}
- (void)save {
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:self.dateOfFirstDownload forKey:[self.defaultsKey stringByAppendingString:@"dateOfFirstDownload"]];
	[defaults setInteger:self.downloadedByteCount forKey:[self.defaultsKey stringByAppendingString:@"downloadedByteCount"]];
	[defaults synchronize];
}

@end

