//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "cellitem.h"
#include "../durationlabel.h"


class BNCellItemTopStory : public BNCellItem {
public:

    BNCellItemTopStory(BNCellsModule* module);
    
    void setItem(BNItem* item) override;
    
    enum BNCellOrientation {
        kPortrait,
        kLandscape,
        kLandscapeWithFullWidthHeadline
    } _orientation;

    BNDurationLabel* _duration;
    
};
