//
//  BNPolicyEndpoint.m
//  BBCNews
//
// Copyright (c) 2015 BBC News. All rights reserved.
//

#include "endpoint.h"

static const char* BNEndpointHREFKey = "href";
static const char* BNEndpointFlagKey = "flag";
static const char* BNEndpointTTLKey = "ttl";
static const char* BNEndpointIsPolicyEndpointKey = "isPolicyEndpoint";



BBCNEndpoint::BBCNEndpoint(const variant& json, bool isPolicyEndpoint/*=false*/) {
    _href = json.stringVal(BNEndpointHREFKey);
    _flag = json.stringVal(BNEndpointFlagKey);
    _ttl = json.intVal(BNEndpointTTLKey);
    _isPolicyEndpoint = isPolicyEndpoint;
}


string BBCNEndpoint::getHREF() {
	if((_flag=="green") || _isPolicyEndpoint) {
		return _href;
	}
	return "BNFLAGPOLEDOWN";
}

