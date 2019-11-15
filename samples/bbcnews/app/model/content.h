//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include "basemodel.h"

class BNContent : public BNBaseModel {
public:
    BNContent(const variant& json);
    BNContent(const string& type, const string& modelId);

    struct stub {
        string modelId;
        string name;
        string format;
        
        string url();
        
        stub(const string& id, const string& name, const string& format);
        stub(const string& id, const string& name);
        bool operator==(const stub& stub) const;
        bool operator<(const stub& stub) const;
        
        static stub fromID(const string& id, const string& name, const string& format);
        static stub fromID(const string& id, const string& name);
        static stub fromURL(const string& url);
    };
    
    bool isContent() override { return true; }


    string _format;
    string _iStatsCounterName;
    string _pageType;
    string _cpsAssetId;
    string _nation;
    string _language;
    string _name;
    string _summary;
    string _nameOverride;
    string _summaryOverride;
    string _shareUrl;
    string _site;
    bool _disallowAdvertising;

    bool isFollowable();
    bool isDownloadable();
    bool isMediaItem();
    bool isVideo();
    bool isAudio();
    bool isEmpty();
    bool isCPSTopic();

    stub getStub();
    stub getStubWithOverriddenTitle(const string& title);

    string url();

};


