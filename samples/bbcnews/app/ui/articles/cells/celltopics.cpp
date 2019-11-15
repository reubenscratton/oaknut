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
        _label = new BNTopicLabel(module->_maxTopics, false, false, module->_json.boolVal("long_timestamp"));
        _label->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
        _label->setUseDynamicText(true);
        //_label->applyStyle("H1");
        addSubview(_label);
    }

    void setItem(BNItem* item) override {
        _label->setItem(item, _module->_contentObject.as<BNCollection>());
    }


};

DECLARE_DYNCREATE(BNCellTopics, BNCellsModule*);
