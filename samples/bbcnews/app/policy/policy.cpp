//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "policy.h"
#include <sys/utsname.h>


/**
 Local vs remote policy files:
 
 We always use the bundle policy in DEBUG builds, the assumption being that the bundle policy file
 is kept up to date and will contain any changes you want to test / debug.
 
 If you need to debug with a remote policy file then comment out or change this block.
 */
#ifndef DEBUG
#define USE_DOWNLOADED_POLICY
#endif


/*
#import "BNContentRequest.h"
#import "BNStyles.h"
#import "NSDate+Formatter.h"
#import "BNEnvironment.h"
#import "BNFileManager.h"
#import "BBCNDisabledAdvertConfiguration.h"
#import "BBCNPolicyAdvertConfiguration.h"
#import "BBCNFeatureController.h"
*/

static BNPolicy* s_current;

string BNPolicyUpdatedNotification = "BNPolicyUpdatedNotification";


BNPolicy* BNPolicy::current() {
    if (!s_current) {
#ifdef USE_DOWNLOADED_POLICY
        BNPolicy *cachedPolicy = [NSKeyedUnarchiver unarchiveObjectWithFile:[self mainPolicyFileLocation]];
#else
        BNPolicy *cachedPolicy = nil;
#endif
        
        bytearray data;
        app->loadAsset("policy.json", data);
        assert(data.size());
        StringProcessor it(data.toString());
        variant json = variant::parse(it, PARSEFLAG_JSON);

        s_current = new BNPolicy(json);
        
#ifdef USE_DOWNLOADED_POLICY
        /*
         // Request policy file updates
         
         BNDeviceSpec * deviceSpec = [BNDeviceSpec sharedInstance];
         
         string policyContentId = [NSString stringWithFormat:@"?OSVersion=%@&OSPlatform=iOS&DeviceType=%@&DeviceLocale=%@&AppFlavour=UK&AppVersion=%@&AppBuildVersion=%@&NetworkMCC=%@&NetworkMNC=%@",
         [[UIDevice currentDevice] systemVersion],
         IS_IPAD?@"tablet":@"phone",
         [[NSLocale currentLocale] localeIdentifier],
         [[[[BNEnvironment alloc] init] buildDetails] publicVersionNumber],
         [[[[BNEnvironment alloc] init] buildDetails] buildVersionNumber],
         [deviceSpec networkMCC]?[deviceSpec networkMCC]:@"",
         [deviceSpec networkMNC]?[deviceSpec networkMNC]:@""];

         
         dispatch_async(dispatch_get_main_queue(), ^{
         NSURL * policyURL = [NSURL URLWithString:[NSString stringWithFormat:@"%@%@", [s_current->endpointPolicy getHREF], policyContentId]];
         [[BNURLRequestManager sharedInstance] requestURL:policyURL delegate:self flags:0 priority:BNDownloadPriorityHigh ttl:[[_currentPolicy endpointPolicy] ttl] creatorBlock:^BNURLRequest *{
         BNJSONRequest* req = [[BNJSONRequest alloc] initWithURL:policyURL];
         return req;
         }];
         
         });
         */
 #endif

    }
    return s_current;
}

BNPolicy::BNPolicy(const variant& json) : BNPolicy(json, BNEnvironment::currentAppFlavour()) {
}

BNPolicy::BNPolicy(const variant& json, BNAppFlavour appFlavour) {
    //_json = json;
    _appFlavour = appFlavour;
    
    //TODO: check for empty data?
    _version = json.floatVal("version");
    _minAppVersion = json.stringVal("minAppVersion");
    _killSwitch = (json.intVal("killSwitch")!=0) || appIsOutOfDate();
    _maxTopicsFollowed = json.intVal("maxTopicsFollowed");

    
    _maxOfflineMBPerDayWifi = json.intVal("maxOfflineMBPerDayWifi");
    _maxOfflineMBPerDay3G = json.intVal("maxOfflineMBPerDay3G");
    if (_maxOfflineMBPerDayWifi <=0) {
        _maxOfflineMBPerDayWifi = 10;
    }
    if (_maxOfflineMBPerDay3G <=0) {
        _maxOfflineMBPerDay3G = 1;
    }
    
    auto endpoints = json.compoundRef("endpoints");
    for (auto it : endpoints) {
        BBCNEndpoint *endpoint = new BBCNEndpoint(it.second,  (it.first == "policy"));
        _endpoints[it.first] = endpoint;
    }
    _endpointPolicy = _endpoints["policy"];
    _endpointContent = _endpoints["content"];
    _endpointResolver = _endpoints["resolver"];
    
    
    if (appFlavour == BNAppFlavour::BETA) {
        string betaContentEnv = app->getStringSetting("BBCTrevorContentEndpoint", "");
        string betaResolverEnv = app->getStringSetting("BBCTrevorResolverEndpoint", "");
        
        if (betaContentEnv.length() > 0) {
            _endpointContent = new BBCNEndpoint(variant({{"href",betaContentEnv},
                                                         {"flag","green"},
                                                         {"ttl", 600}}));
        }
        
        if (betaResolverEnv.length() > 0) {
            _endpointResolver = new BBCNEndpoint(variant({{"href",betaContentEnv},
                                                          {"flag","green"},
                                                          {"ttl", 0}}));
        }
    }
    
    _endpointImageChef = _endpoints["ichef"];
    _endpointMedia = _endpoints["media"];
    _endpointLocator = _endpoints["locator"];
    _endpointLocatorNewsRegion = _endpoints["locator_newsregion"];
    _endpointLayout = _endpoints["layout"];

    _termsAndConditionsURL = json.stringVal("termsAndConditionsUrl");
    _privacyPolicyURL = json.stringVal("privacyPolicyUrl");
    _storeURL = json.stringVal("storeUrl");
    
    auto imageChefRecipes = json.arrayRef("imageChefRecipes");
    for (auto& r : imageChefRecipes) {
        _imageChefRecipes.push_back(r.intVal());
    }
    auto layoutModes = json.arrayRef("layoutModes");
    for (auto& m : layoutModes) {
        _layoutModes.push_back(m.stringVal());
    }
    _betaModeAutoEnabledPercent = json.floatVal("betaModeAutoEnabledPercent");
    _betaModeFeedbackUrl = json.stringVal("betaModeFeedbackUrl");
    _sportAppITunesUrl = json.stringVal("sportAppITunesUrl");
    _surveyURL = json.stringVal("surveyURL");
    _surveyActive = json.intVal("surveyActive")!=0;
    _copyrightYear = json.stringVal("copyright");
    _contactEmail = json.stringVal("contactEmail");
    
    if (!_copyrightYear.length()){
        _copyrightYear = "2019";// todo: [NSDate longYearStringFromDate:[NSDate date]];
    }
    
    // User message
    auto jsonUserMessage = json.get("userMessage");
    if (!jsonUserMessage.isEmpty()) {
        _userMessageText = jsonUserMessage.stringVal("text");
        _userMessageTitle = jsonUserMessage.stringVal("title");
        _userMessageIsFatal = jsonUserMessage.intVal("canContinue")==0;
        _userMessageUrlButtonTitle = jsonUserMessage.stringVal("urlButtonText");
        _userMessageUrlButtonUrl = jsonUserMessage.stringVal("url");
    }
    _appStoreUrl = json.stringVal("appStoreUrl");
	
}

bool BNPolicy::appIsOutOfDate() {
    return false;
    /*
	BOOL appOutOfDate = NO;
	
	NSString *policyMinVersion = [NSMutableString stringWithFormat:@"%@", _minAppVersion];
	
	if (policyMinVersion) {
		
		NSMutableString *policyMinVersionMutable = [policyMinVersion mutableCopy];
		
		
		NSInteger numberOfPoints = [policyMinVersionMutable replaceOccurrencesOfString:@"."
											withString:@""
											   options:NSLiteralSearch
												 range:NSMakeRange(0, [policyMinVersionMutable length])];
		
		NSString *requiredVersion = policyMinVersion;
		
		if (numberOfPoints == 0) {
			requiredVersion = [NSString stringWithFormat:@"%@.%@", [[[[BNEnvironment alloc] init] buildDetails] publicVersionNumber], policyMinVersion];
		}

		NSString* actualVersion = [[[[BNEnvironment alloc] init] buildDetails] buildVersionNumber];
		
		appOutOfDate = ([requiredVersion compare:actualVersion options:NSNumericSearch] == NSOrderedDescending);
		
	}
	
	return appOutOfDate;*/
}


/*
- (NSString *)mainPolicyFileLocation
{
    return [[[BNFileManager sharedCachesDirectory] URLByAppendingPathComponent:@"policy_main.dat"] path];
}

- (void)onRequestLoadedObject:(BNURLRequest *)request object:(id)object isCacheData:(BOOL)isCacheData
{
    
    self.currentPolicy = [[BNPolicy alloc] initWithJSONDictionary:object];
    
    // When current policy updates from network, write to App Group Sandbox
    // This allows us to read the latest one from network on next launch, rather than from the bundle
    // So extensions have something
    [NSKeyedArchiver archiveRootObject:self.currentPolicy
                                toFile:[self mainPolicyFileLocation]];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:BNPolicyUpdatedNotification object:nil];
}


- (id<BBCNAdvertConfiguration>)advertConfiguration
{
	if (self.appFlavour == BNAppFlavourGNL) {
		return [[BBCNPolicyAdvertConfiguration alloc] initWithDictionary:self.json[@"ad_settings"]];
	} else if (self.appFlavour == BNAppFlavourBETA) {
		if ([BBCNFeatureController advertsEnabled]) {
			
			NSString* policyPath = [[NSBundle mainBundle] pathForResource:@"policy" ofType:@"json"];
			NSData* policyJsonData = [NSData dataWithContentsOfFile:policyPath];
			NSError* error = nil;
			static NSUInteger options = NSJSONReadingAllowFragments|NSJSONReadingMutableContainers;
			NSDictionary* localPolicy = [NSJSONSerialization JSONObjectWithData:policyJsonData options:options error:&error];
			return [[BBCNPolicyAdvertConfiguration alloc] initWithDictionary:localPolicy[@"ad_settings"]];
		}
	}
	return [[BBCNDisabledAdvertConfiguration alloc] init];
}
*/

string BNPolicy::userAgent() {
	
    // Get Real Hardware Version e.g. iPhone4,1 or iPad3,2;
	struct utsname systemInfo;
    uname(&systemInfo);
    string model = systemInfo.machine;

	if (!model.length()) {
		// On startup the specific model version can sometimes return nil, causing a crash. If it's nil, use the device model as a fallback.
        model = "iDevice"; // todo [[UIDevice currentDevice] model];
	}
	
	
    return string::format("BBCNews%@/%s (%s; %s %s)",
            BNEnvironment::currentAppFlavourString().data(),
			"4.0" /*todo [[environment buildDetails] buildVersionNumber]*/,
			model.data(),
			"iOS" /*todo [[UIDevice currentDevice] systemName]*/,
			"12" /*todo [[UIDevice currentDevice] systemVersion]]*/
    );
}



