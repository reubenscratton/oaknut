//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "_module.h"

BNLiveEvent::BNLiveEvent(const string& modelId) : BNItem(BNModelTypeLiveEvent, modelId) {
}

BNLiveEvent::BNLiveEvent(const variant& json) : BNItem(json) {
    _liveStatus = json.stringVal("liveStatus");
    _twitter = json.stringVal("twitter");
    _facebook = json.stringVal("facebook");
    _commentaryId = json.stringVal("commentaryId");
    auto keypoints = json.arrayRef("keyPoints");
    for (auto kp : keypoints) {
        _keyPoints.push_back(kp.stringVal());
    }
    _smsNumber = json.stringVal("smsNumber");
    _isLive = json.intVal("isLive")!=0;
}



