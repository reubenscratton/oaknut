#pragma once
#include "basemodel.h"

class BNImage;


class BNAV : public BNBaseModel {
public:
    string _caption;
    int _duration;
    string _externalId;
    string _guidance;
    bool _isEmbeddable;
    bool _isAvailable;
    bool _isLive;
    string _mediaId;

    BNAV(const variant& json);
    virtual bool isAudio()=0;
    virtual BNImage* posterImage();
    
    bool isAV() override { return true; }
protected:
    BNImage* _posterImage;

};
