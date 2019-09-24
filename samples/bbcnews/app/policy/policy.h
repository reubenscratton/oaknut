//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>
#include "endpoint.h"
#include "environment.h"
//#import "BBCNAdvertConfiguration.h"

extern string BNPolicyUpdatedNotification;


class BNPolicy : public Object {
public:
    
    //variant json;
    BNAppFlavour _appFlavour;
    


    // Policy Versioning
    float _version;
    string _minAppVersion;

    // Feature Flags / Switches
    bool _killSwitch;
    bool _surveyActive;
    bool _userMessageIsFatal;
    bool appIsOutOfDate();

    // App Behaviour/Configuration
    unsigned int _maxTopicsFollowed;
    int _maxOfflineMBPerDayWifi;
    int _maxOfflineMBPerDay3G;
    vector<int> _imageChefRecipes;
    string _copyrightYear;
    string _contactEmail;
    vector<string> _layoutModes;
    float _betaModeAutoEnabledPercent;
    string _betaModeFeedbackUrl;

    // BBC Endpoint Configuration
    map<string, BBCNEndpoint*> _endpoints;
    BBCNEndpoint* _endpointPolicy;
    BBCNEndpoint* _endpointContent;
    BBCNEndpoint* _endpointImageChef;
    BBCNEndpoint* _endpointMedia;
    BBCNEndpoint* _endpointLocator;
    BBCNEndpoint* _endpointLocatorNewsRegion;
    BBCNEndpoint* _endpointResolver;
    BBCNEndpoint* _endpointLayout;


    // External URLS
    string _termsAndConditionsURL;
    string _privacyPolicyURL;
    string _storeURL;
    string _sportAppITunesUrl;
    string _surveyURL;
    string _appStoreUrl;

    // Messaging
    bool _shouldShowUserMessage;
    string _userMessageTitle;
    string _userMessageText;
    string _userMessageUrlButtonTitle;
    string _userMessageUrlButtonUrl;


// Advert handling
//- (id<BBCNAdvertConfiguration>)advertConfiguration;

/**
 Returns the BBC Standardized user agent string for mobile apps in the form: "productName/productVersion (deviceModel; OS OSVersion)"
 from:https://confluence.dev.bbc.co.uk/display/mobile/Native+mobile+application+User-Agent+identification
 */
    
    static BNPolicy* current();
    static string userAgent();

protected:
    BNPolicy(const variant& json);
    BNPolicy(const variant& json, BNAppFlavour appFlavour);

};



