#pragma once
#include "item.h"

class BNLiveEvent : public BNItem {
public:

    string _commentaryId;
    string _facebook;
    bool _isLive;
    vector<string> _keyPoints;
    string _liveStatus;
    string _smsNumber;
    string _twitter;

    BNLiveEvent(const string& modelId);
    BNLiveEvent(const variant& json);
    
    bool isLiveEvent() override { return true; }
};
