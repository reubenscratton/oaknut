//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "../../common/cell.h"
#include "../../common/label.h"


class BNCellHeadline : public BNCell {
public:
    
    BNLabel* _label;

    BNCellHeadline(BNCellsModule* module) : BNCell(module) {
        _label = new BNLabel();
        _label->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
        _label->applyStyle("H1");
        _label->setUseDynamicText(true);
        addSubview(_label);
    }

    void setItem(BNItem* item) override {
		bool invColor = item->isMediaItem() || _module->_json.boolVal("inverseColorScheme");
        _label->setText(item->_name);
    }

};


DECLARE_DYNCREATE(BNCellHeadline, BNCellsModule*);
