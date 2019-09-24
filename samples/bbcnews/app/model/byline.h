//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "basemodel.h"

class BNByline : public BNBaseModel {
public:

    string _title;

    BNByline(const variant& json);
};
