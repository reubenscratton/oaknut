//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(NavigationBar);

NavigationBar::NavigationBar() {
    auto safeAreaInsets = app.getWindowSafeAreaInsets();
    setMeasureSpecs(MEASURESPEC::FillParent(), MEASURESPEC::Abs(app.getStyleFloat("navbar.height")+safeAreaInsets.top));
	_padding = EDGEINSETS(0,safeAreaInsets.top,0,0);
	setBackgroundColor(0xffffffff);
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
	assert(false); // Navbars don't have arbitrary backgrounds, it can only be a color
}
void NavigationBar::setBackgroundColor(COLOR color) {
    _backgroundColor = color;
    RenderOp* op = _blurEnabled ? (RenderOp*)new BlurRenderOp(this) : new RectRenderOp(this);
    op->setRect(getOwnRect());
    op->setColor(color);
    View::setBackground(op);
}

void NavigationBar::setBlurEnabled(bool blurEnabled) {
    if (blurEnabled != _blurEnabled) {
        _blurEnabled = blurEnabled;
        setBackgroundColor(_backgroundColor);
    }
}


