//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "_module.h"


BNCommentaryItem::BNCommentaryItem(const string& modelId) : BNItem(BNModelTypeCommentary, modelId) {
}


BNCommentaryItem::BNCommentaryItem(const variant& json) : BNItem(json) {
    _heading = json.stringVal("heading");
    _subHeading = json.stringVal("subHeading");
    _isSticky = json.intVal("isSticky")!=0;
    _time = json.stringVal("time");
    _typeCode = json.stringVal("typeCode");
}

