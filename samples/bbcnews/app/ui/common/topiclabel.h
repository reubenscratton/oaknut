//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>
#include "label.h"
#include "../../model/_module.h"

class BNTopicLabel : public BNLabel {
public:
    BNTopicLabel(int maxTopics, bool inverseColorScheme, bool hideTimestamp, bool isLongTimestamp);

    //void measureForWidth(float width, POINT offset) override;
    
    void setItem(BNItem* item, BNCollection* displayingCollection);
    
    
protected:
    void updateText();
    
    int _maxTopics;
    long _timestamp;
    bool _hideTimestamp;
    bool _longTimestamp;
    attributed_string _topicLinks;
    Timer* _timer;

};
