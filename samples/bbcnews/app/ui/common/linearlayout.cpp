//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "linearlayout.h"
#include "cell.h"

BNLinearLayout::BNLinearLayout(const variant& json) : BNContainerModule(json) {
}

// Cloning
BNLinearLayout::BNLinearLayout(BNLinearLayout* source) : BNContainerModule(source) {
}
BNModule* BNLinearLayout::clone() {
    return new BNLinearLayout(this);
}

View* BNLinearLayout::createView() {
    LinearLayout* view = new LinearLayout();
    view->setBackgroundColor(_backgroundColor);
    view->_orientation = LinearLayout::Vertical;
    return view;
}
/*
void BNLinearLayout::updateSubviews(View* superview) {
	
	bool visible =  superview->getOwnRect().intersects(_frame);
	if (visible) {
		if (!_view) {
            _view = new View();
            _view->setRect(_frame);
			_view->setBackgroundColor(_backgroundColor);
			superview->addSubview(_view);
            for (BNModule* module : _modules) {
				module->updateSubviews(_view);
			}
		}
	} else {
		if (_view) {
            while (_view->getSubviewCount()) {
                _view->removeSubview(_view->getSubview(0));
            }
		}
	}
}

void BNLinearLayout::removeAllViews() {
	if (_view) {
        _view->removeFromParent();
        BNContainerModule::removeAllViews();
		_view = NULL;
	}
}
*/

class BNHorizontalLinearLayout : public BNLinearLayout {
public:
   bool _disableChildHeightExtension;

    BNHorizontalLinearLayout(const variant& json) : BNLinearLayout(json) {
        _disableChildHeightExtension = json.boolVal("disableChildHeightExtension");
    }
    
    // Cloning
    BNHorizontalLinearLayout(BNHorizontalLinearLayout* source) : BNLinearLayout(source) {
        _disableChildHeightExtension = source->_disableChildHeightExtension;
    }
    BNModule* clone() override {
        return new BNHorizontalLinearLayout(this);
    }
    
    View* createView() override {
        LinearLayout* view = new LinearLayout();
        view->_orientation = LinearLayout::Horizontal;
        view->setLayoutSize(LAYOUTSPEC::Fill(), LAYOUTSPEC::Wrap());
        return view;
    }
    /*
    RECT layoutModules(vector<BNModule*> modules, RECT containingRect) override {

        POINT initialOrigin = containingRect.origin;
        containingRect = containingRect.copyWithInsets(_padding);
        RECT localBounds = containingRect;
        localBounds.origin.x -= initialOrigin.x;
        localBounds.origin.y -= initialOrigin.y;
        
        // Get weights sum
        CGFloat sum = 0;
        for (BNModule* submodule : modules) {
            sum += submodule->_json.floatVal("weight");
        }
        
        CGFloat widthCarriedForward = 0;
        CGFloat height = 0;
        for (BNModule* submodule : modules) {
            if (sum > 0) {
                CGFloat weight = submodule->_json.floatVal("weight") / sum;
                localBounds.size.width = containingRect.size.width * weight;
            } else {
                localBounds.size.width = containingRect.size.width / modules.size();
            }
            localBounds.size.width += widthCarriedForward;
            // todo: item padding?
            submodule->layoutWithContainingRect(localBounds);
            localBounds.origin.x += submodule->_frame.size.width;
            widthCarriedForward = localBounds.size.width - submodule->_frame.size.width;
            height = MAX(height, submodule->_frame.size.height);
        }
        
        // Ensure all submodules have same height
        if (!_disableChildHeightExtension) {
            for (BNModule* submodule : modules) {
                submodule->extendToHeight(height);
            }
        }
        
        containingRect.size.height = MIN(containingRect.size.height, height);
        containingRect = containingRect.copyWithUninsets(_padding);
        return containingRect;
    }*/

};

DECLARE_DYNCREATE(BNHorizontalLinearLayout, const variant&);

class BNVerticalLinearLayout : public BNLinearLayout {
public:

    BNVerticalLinearLayout(const variant& json) : BNLinearLayout(json) {
    }
    
    // Cloning
    BNVerticalLinearLayout(BNVerticalLinearLayout* source) : BNLinearLayout(source) {
    }
    BNModule* clone() override {
        return new BNVerticalLinearLayout(this);
    }
    
    View* createView() override {
        LinearLayout* view = new LinearLayout();
        view->_orientation = LinearLayout::Vertical;
        return view;
    }

/*
    RECT layoutModules(vector<BNModule*> modules, RECT containingRect) override {
        
        POINT initialOrigin = containingRect.origin;
        containingRect = containingRect.copyWithInsets(_padding);
        RECT localBounds = containingRect;
        localBounds.origin.x -= initialOrigin.x;
        localBounds.origin.y -= initialOrigin.y;
        CGFloat height = 0;
        for (BNModule* submodule : modules) {
            submodule->layoutWithContainingRect(localBounds);
            localBounds.origin.y += submodule->_frame.size.height;
            height += submodule->_frame.size.height;
        }
        containingRect.size.height = height;
        containingRect = containingRect.copyWithUninsets(_padding);
        if (containingRect.size.height <= 0) {
            containingRect.size.width = 0;
        }
        return containingRect;
    }

    RECT boundsAfter(RECT frame) override {
        frame.origin.y += frame.size.height;
        return frame;
    }*/
};

DECLARE_DYNCREATE(BNVerticalLinearLayout, const variant&);
