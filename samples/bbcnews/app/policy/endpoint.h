//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>

class BBCNEndpoint : public Object {
public:

    BBCNEndpoint(const variant& json, bool isPolicyEndpoint=false);

    int _ttl;

    string getHREF();
protected:
    bool  _isPolicyEndpoint;
    string _flag;
    string _href;
};
