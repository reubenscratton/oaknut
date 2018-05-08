//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(NavigationBar);

NavigationBar::NavigationBar() {
    float statusBarHeight = app.getStyleFloat("statusbar.height");
    setMeasureSpecs(MEASURESPEC::FillParent(), MEASURESPEC::Abs(app.getStyleFloat("navbar.height")+statusBarHeight));
	_padding = EDGEINSETS(0,statusBarHeight,0,0);
	setBackgroundColour(0xffffffff);
}

void NavigationBar::addNavigationItem(NavigationItem* navigationItem) {
    addSubview(navigationItem->_leftButtonsFrame);
    if (navigationItem->_titleView) {
        addSubview(navigationItem->_titleView);
    }
    addSubview(navigationItem->_rightButtonsFrame);
}
void NavigationBar::removeNavigationItem(NavigationItem* navigationItem) {
    navigationItem->_leftButtonsFrame->removeFromParent();
    if (navigationItem->_titleView) {
        navigationItem->_titleView->removeFromParent();
    }
    navigationItem->_rightButtonsFrame->removeFromParent();
}

void NavigationBar::setBackground(RenderOp* renderOp) {
	assert(false); // Navbars don't have arbitrary backgrounds, it can only be a colour
}
void NavigationBar::setBackgroundColour(COLOUR colour) {
    _backgroundColour = colour;
    RenderOp* op = _blurEnabled ? (RenderOp*)new BlurRenderOp(this) :
                                  new ColorRectFillRenderOp(this, getOwnRect(), colour);
    op->setColour(_backgroundColour);
    View::setBackground(op);
}

void NavigationBar::setBlurEnabled(bool blurEnabled) {
    if (blurEnabled != _blurEnabled) {
        _blurEnabled = blurEnabled;
        setBackgroundColour(_backgroundColour);
    }
}


