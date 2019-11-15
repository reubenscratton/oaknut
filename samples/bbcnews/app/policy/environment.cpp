//
//  BNEnvironment.m
//  BBCNews
//
//  Copyright (c) 2015 BBC News. All rights reserved.
//

#include "environment.h"
//#import "BNBuildEnvironment.h"


BNAppFlavour BNEnvironment::currentAppFlavour() {
	
//	#ifdef UK
    return BNAppFlavour::UK;
/*	#endif
	
	#ifdef GNL
		return BNAppFlavourGNL;
	#endif
	
	#ifdef BETA
		return BNAppFlavourBETA;
	#endif
*/
}

string BNEnvironment::currentAppFlavourString() {

	switch (currentAppFlavour()) {
        case BNAppFlavour::UK:
			return "UK";
        case BNAppFlavour::GNL:
			return "GNL";
        case BNAppFlavour::BETA:
        default:
			return "BETA";
	};
}

#ifndef APP_GROUP_IDENTIFIER
#define APP_GROUP_IDENTIFIER "org.bbc.news.app->debug"
#endif

string BNEnvironment::appGroupID() {
    return string(APP_GROUP_IDENTIFIER);
}

string BNEnvironment::hockeyAppToken() {
#ifdef HOCKEY_ID
    return string(#HOCKEY_ID);
#endif
    return "";
}

/*
- (id<BBCNBuildDetails>)buildDetails
{
	return [[BNBuildEnvironment alloc] init];
}

 */
string BNEnvironment::URLscheme() {
    //NSDictionary* schemeDict = [NSBundle mainBundle].infoDictionary[@"CFBundleURLTypes"][0];
    //return schemeDict[@"CFBundleURLSchemes"][0];
    return "bbcnewsapp"; // todo: load from plist or build settings
}
string BNEnvironment::URLforResourceSpec(const string& resourceSpec) {
    auto scheme = URLscheme();
    return  string::format("%s:%s", scheme.c_str(), resourceSpec.c_str());
}

