//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include "item.h"


class BNCommentaryItem : public BNItem {
public:
    string _heading;
    string _subHeading;
    bool _isSticky;
    string _time;
    string _typeCode;
    
    BNCommentaryItem(const string& modelId);
    BNCommentaryItem(const variant& json);

};
