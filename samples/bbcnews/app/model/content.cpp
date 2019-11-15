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
bool BNContent::stub::operator<(const BNContent::stub& stub) const {
    return modelId < stub.modelId;
}

string BNContent::stub::url() {
    return BNEnvironment::URLforResourceSpec(string::format("%s?format=%s&title=%s", modelId.c_str(), format.c_str(), name.urlEncode().c_str()));
}

BNContent::stub BNContent::stub::fromID(const string& id, const string& name) {
    return stub(id, name, "");
}
BNContent::stub BNContent::stub::fromID(const string& id, const string& name, const string& format) {
    return stub(id, name, format);
}

struct url {
    string scheme;
    string host;
    int port;
    string path;
    map<string, string> queryparams;
    
    url(const string& str) {
        uint32_t start = 0;
        uint32_t pos = str.find(":");
        if (pos < str.lengthInBytes()) {
            scheme = str.substr(start, pos);
            start = pos+1;
        }
        // Host[+port] specified?
        if (str.skipString(start, "//")) {
            host = str.readUpToOneOf(start, ":/");
            pos = host.find(":");
            if (pos < host.lengthInBytes()) {
                port = host.substr(pos+1).asInt();
                host = host.substr(0, pos);
            }
        }
        pos = str.find("?", 1, start);
        //uint32_t pathEnd = pos;
        if (pos < str.lengthInBytes()) {
            path = str.substr(start, pos);
            string query = str.substr(pos+1);
            auto pairs = query.split("&");
            for (auto pair : pairs) {
                auto bits = pair.split("=");
                auto key = bits[0].urlDecode();
                string value;
                if (bits.size() > 1) {
                    value = bits[1].urlDecode();
                } else {
                    value = "";
                }
                queryparams[key] = value;
            }
        } else {
            path = str.substr(start);
        }
    }
};

BNContent::stub BNContent::stub::fromURL(const string& urlstr) {
    struct url url(urlstr);
    
    // Create a stub for the content we need to show
    string stubID = url.path;
    if (url.host.length()) {
        stubID.insert(0, url.host);
        stubID.insert(0, "/");
        stubID = string::format("/%s%s", url.host.c_str(), url.path.c_str());
    }
    
    return stub(stubID, url.queryparams["title"], url.queryparams["format"]);
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

