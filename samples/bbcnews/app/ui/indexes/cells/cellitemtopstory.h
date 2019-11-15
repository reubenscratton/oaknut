//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "cellitem.h"
#include "../../common/durationlabel.h"


class BNCellItemTopStory : public BNCellItem {
public:

    BNCellItemTopStory(BNCellsModule* module);
    
    void setItem(BNItem* item) override;
    
    
};
