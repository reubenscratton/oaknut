//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>

enum BNAppFlavour {
	BETA,
	UK,
	GNL
};

class BNEnvironment : public Object {
public:

/**
 Returns the current BNAppFlavour for the app-> 
 
 This is a build setting passed in by the compiler and doesn't change at runtime. This is used primarily by the policy file to help it make decisions about what settings to provide the rest of the app
 
 @return BNAppFlavour
 */
static BNAppFlavour currentAppFlavour();

/**
 Returns a String representation of the current App Flavour.
 
 e.g. @"UK", @"GNL", @"BETA" 
 
 @return BNAppFlavour
 */
static string currentAppFlavourString();

/**
 Returns the appGroupID for the current build. Is passed in at build time (hence not in the policy)
 @return appGroupID (e.g. group.uk.co.bbc.newsuk)
 */
static string appGroupID();

/**
 Returns the token to initialise hockeykit with. Is passed in at build time (hence not in the policy)
 @return hockeyAppToken
 */
static string hockeyAppToken();


/**
 Returns the build settings such as the git hash, build date/time and version numbers as dictated by jenkins. Is passed in at build time (hence not in the policy)
 @return buildDetails
 */
//- (id<BBCNBuildDetails>)buildDetails;


static string URLscheme();
static string URLforResourceSpec(const string& resourceSpec);

};
