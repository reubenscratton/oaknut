//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "_module.h"



BNPerson::BNPerson(const variant& json) : BNBaseModel(json) {
    _function = json.stringVal("function");
	_name = json.stringVal("name");
	_thumbnailUrl = json.stringVal("thumbnailUrl");
}

