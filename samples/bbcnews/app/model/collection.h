#pragma once
#include "content.h"

class BNCollection : public BNContent {
public:
    
    BNCollection(const string& modelId);
    BNCollection(const variant& json);
    
    bool canBeTopicLink();
    bool isFollowable();

    bool isCollection() override { return true; }

protected:
    void ensureChildItemsAreRelatedToSelf();
};

