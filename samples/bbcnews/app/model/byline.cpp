//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "_module.h"


BNByline::BNByline(const variant& json) : BNBaseModel(json) {
    _title = json.stringVal("title");
}


