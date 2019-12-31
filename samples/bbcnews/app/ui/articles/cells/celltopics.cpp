//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "../../common/cell.h"
#include "../../common/topiclabel.h"

//#import "BNStyles.h"
//#import "BNItem.h"
//#import "BNCell.h"
//#import "BNTopicLabelInfo.h"

class BNCellTopics : public BNCell {
public:
    
    BNTopicLabel* _label;


    BNCellTopics(BNCellsModule* module) : BNCell(module) {
        setPadding({32,8,0,8});
        _label = new BNTopicLabel(1, false, false, true);
        _label->setLayoutSize(LAYOUTSPEC::Fill(), LAYOUTSPEC::Wrap());
        _label->setUseDynamicText(true);
        addSubview(_label);
    }

    void setItem(BNItem* item) override {
        _label->setItem(item, _module->_contentObject.as<BNCollection>());
    }


};

DECLARE_DYNCREATE(BNCellTopics, BNCellsModule*);
