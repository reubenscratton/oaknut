#pragma once
#include "av.h"

class BNVideo : public BNAV {
public:
    BNVideo(const variant& json);

    bool isAudio() override;

};

