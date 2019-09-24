//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "basemodel.h"

class BNPerson : public BNBaseModel {
public:
    
    BNPerson(const variant& json);

    string _function;
    string _name;
    string _thumbnailUrl;
};
