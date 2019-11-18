//
//  BNPolicyEndpoint.m
//  BBCNews
//
// Copyright (c) 2015 BBC News. All rights reserved.
//

#include "endpoint.h"

static string BNEndpointHREFKey = "href";
static string BNEndpointFlagKey = "flag";
static string BNEndpointTTLKey = "ttl";
static string BNEndpointIsPolicyEndpointKey = "isPolicyEndpoint";



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

