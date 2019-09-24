//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>
#include "label.h"
#include "../model/_module.h"

class BNTopicLabel : public BNLabel {
public:
    BNTopicLabel(BNItem* item, int maxTopics, bool inverseColorScheme, bool hideTimestamp, BNCollection* displayingCollection, bool isLongTimestamp);

    //void measureForWidth(float width, POINT offset) override;
    
    
protected:
    void updateText();
    
    long _timestamp;
    bool _hideTimestamp;
    bool _longTimestamp;
    AttributedString _topicLinks;
    Timer* _timer;

};
