//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "_module.h"
#include "../policy/environment.h"


BNContent::BNContent(const string& type, const string& modelId) : BNBaseModel(type, modelId) {
}

BNContent::BNContent(const variant& json) : BNBaseModel(json) {
		
    _format = json.stringVal("format");
    _language = json.stringVal("language");
    _disallowAdvertising = !json.intVal("allowAdvertising");
    _name = json.stringVal("name");
    _summary = json.stringVal("summary");
    _nameOverride = json.stringVal("nameOverride");
    _summaryOverride = json.stringVal("summaryOverride");
    _shareUrl = json.stringVal("shareUrl");
    _site = json.stringVal("site");

    _iStatsCounterName = json.stringVal("iStatsCounter");
    variant& iStatsLabels = json.get("iStatsLabels");
    if (!iStatsLabels.isEmpty()) {
        _pageType = iStatsLabels.stringVal("page_type");
        _cpsAssetId = iStatsLabels.stringVal("cps_asset_id");
        _nation = iStatsLabels.stringVal("by_nation");
    }
}

BNContent::stub::stub(const string& id, const string& name, const string& format) {
    this->modelId = id;
    this->name = name;
    this->format = format;
}
BNContent::stub::stub(oak::string const& id, oak::string const& name) {
    this->modelId = id;
    this->name = name;
}



bool BNContent::stub::operator==(const BNContent::stub& stub) const {
    return modelId == stub.modelId;
}

string BNContent::stub::url() {
    return BNEnvironment::URLforResourceSpec(string::format("%s?format=%s&title=%s", modelId.data(), format.data(), name.urlEncode().data()));
}

BNContent::stub BNContent::stub::fromID(const string& id, const string& name) {
    return stub(id, name, "");
}
BNContent::stub BNContent::stub::fromID(const string& id, const string& name, const string& format) {
    return stub(id, name, format);
}

BNContent::stub BNContent::stub::fromURL(const string& url) {
    assert(0); // todo
    // Parse the query string into something usable
    /*NSArray * pairs = [url.query componentsSeparatedByString:@"&"];
    NSMutableDictionary * kvPairs = [NSMutableDictionary dictionary];
    for (NSString * pair in pairs) {
        NSArray * bits = [pair componentsSeparatedByString:@"="];
        NSString * key = [[bits objectAtIndex:0] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
        NSString * value = (bits.count<=1) ? @"" : [[bits objectAtIndex:1] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
        [kvPairs setObject:value forKey:key];
    }
    
    // Create a stub for the content we need to show
    string stubID = url.path;
    if (url.host) {
        stubID = string::format("/%s%s", url.host, url.path);
    }
    
    return stub(stubID, kvPairs["title"], kvPairs["format"]);*/
}

BNContent::stub BNContent::getStub() {
	return stub(_modelId, _name, _format);
}
BNContent::stub BNContent::getStubWithOverriddenTitle(const string& title) {
    return stub(_modelId, title, _format);
}


bool BNContent::isFollowable() {
	return !(_format == BNContentFormatInternal) && !(_format == BNContentFormatNewsapps);
}

bool BNContent::isDownloadable() {
    return !(_format == BNContentFormatInternal);
}

bool BNContent::isMediaItem() {
    return (_format == BNContentFormatVideo) || (_format == BNContentFormatAudio);
}
bool BNContent::isVideo() {
    return _format == BNContentFormatVideo;
}
bool BNContent::isAudio() {
    return _format == BNContentFormatAudio;
}

string BNContent::url() {
	return getStub().url();
}

bool BNContent::isEmpty() {
	return _childRelationships.size() == 0;
}

bool BNContent::isCPSTopic() {
	return _modelId.hasPrefix("/cps/");
}

