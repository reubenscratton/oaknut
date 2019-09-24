//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "_module.h"

BNAudio::BNAudio(const variant& json) : BNAV(json) {
}

bool BNAudio::isAudio() {
    return true;
}
