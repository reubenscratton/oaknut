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
        _label->setLayoutSize(LAYOUTSPEC::Fill(), LAYOUTSPEC::Wrap());
        _label->applyStyle("H1");
        _label->setUseDynamicText(true);
        _label->setPadding(EDGEINSETS(32, 16, 32, 8));
        addSubview(_label);
    }
    /*
     label()
        .style("H1")
        .useDynamicText()
        .padding({32,...});
     */

    void setItem(BNItem* item) override {
		// bool invColor = item->isMediaItem() || _module->_json.boolVal("inverseColorScheme");
        _label->setText(item->_name);
    }

};


DECLARE_DYNCREATE(BNCellHeadline, BNCellsModule*);
