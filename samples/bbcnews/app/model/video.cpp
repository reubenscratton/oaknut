//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "_module.h"

BNVideo::BNVideo(const variant& json) : BNAV(json) {
}

bool BNVideo::isAudio() {
    return false;
}
