//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "scrollablecontainer.h"
#include "../cells/cell.h"


class BNHorizontalStack : public BNContainerModule {
public:
    
    BNHorizontalStack(const variant& json) : BNContainerModule(json) {
    }
    
    
    BNHorizontalStack(BNHorizontalStack* source) : BNContainerModule(source) {
    }
    BNModule* clone() override {
        return new BNHorizontalStack(this);
    }
    
    View* createView() override {
        auto ll = new LinearLayout();
        ll->setBackgroundColor(_backgroundColor);
        ll->_orientation = LinearLayout::Horizontal;
        ll->setSpacing(app->dp(8));
        ll->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
        ll->setClipsContents(false);
        return ll;
    }
    
/*
    RECT layoutModules(vector<BNModule*> modules, RECT bounds) override {
        POINT initialOrigin = bounds.origin;
        bounds = bounds.copyWithInsets(_padding);
        float initialWidth = bounds.size.width;
        bounds.size.width=0; // i.e. no width restriction
        float left = bounds.origin.x;
        float width = 0;
        float height = 0;
        for (BNModule* submodule : _modules) {
            RECT localBounds = bounds;
            localBounds.origin.x -= initialOrigin.x;
            localBounds.origin.y -= initialOrigin.y;
            submodule->layoutWithContainingRect(localBounds);
            bounds.origin.x += submodule->_frame.size.width;
            width += submodule->_frame.size.width;
            height = MAX(height, submodule->_frame.size.height);
        }
        bounds = {left, bounds.origin.y, MAX(initialWidth, width), height};
        bounds = bounds.copyWithUninsets(_padding);
        return bounds;
    }
    
    
    RECT boundsAfter(RECT frame) override {
        frame.origin.x += frame.size.width;
        return frame;
    }
*/
};

DECLARE_DYNCREATE(BNHorizontalStack, const variant&);


