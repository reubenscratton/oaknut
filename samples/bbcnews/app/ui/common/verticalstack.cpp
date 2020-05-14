//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "verticalstack.h"
#include "cell.h"


DECLARE_DYNCREATE(BNVerticalStack, const variant&);

BNVerticalStack::BNVerticalStack(const variant& json) : BNContainerModule(json) {
}


BNVerticalStack::BNVerticalStack(BNVerticalStack* source) : BNContainerModule(source) {
}
BNModule* BNVerticalStack::clone() {
    return new BNVerticalStack(this);
}

View* BNVerticalStack::createView() {
    auto ll = new LinearLayout();
    ll->_orientation = LinearLayout::Vertical;
    //ll->setSpacing(app->dp(8));
    ll->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
    return ll;
}
/*
RECT BNVerticalStack::layoutModules(vector<BNModule*> modules, RECT containingRect) {

    POINT origin = containingRect.origin;
	containingRect = containingRect.copyWithInsets(_padding);
	containingRect.size.height = FLT_MAX; // i.e. no height restriction
	float top = containingRect.origin.y;
    for (BNModule* submodule : _modules) {
		containingRect.origin.x -= origin.x;
		submodule->layoutWithContainingRect(containingRect);
		containingRect.origin.x += origin.x;
		containingRect.origin.y = MAX(containingRect.origin.y, submodule->_frame.bottom());
	}
	containingRect = RECT(containingRect.origin.x, top, containingRect.size.width, containingRect.origin.y-top);
	containingRect = containingRect.copyWithUninsets(_padding);
	return containingRect;
}

RECT BNVerticalStack::boundsAfter(RECT frame) {
	frame.origin.y += frame.size.height;
	return frame;
}


void BNVerticalStack::updateScrollsToTop() {
    warn("TODO: updateScrollsToTop");
	// self.scrollView.scrollsToTop = self.isOnScreen;
}
*/

