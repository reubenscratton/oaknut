//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>
#include "../../model/_module.h"
#include "containermodule.h"


class BNLayout : public Object {
public:
    string _emptyView;
    sp<BNContainerModule> _rootContainer;

    BNLayout(const variant& json);

    // Cloning
    BNLayout(BNLayout* source);
    BNLayout* clone();
};
