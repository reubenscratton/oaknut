//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(NavigationBar);

NavigationBar::NavigationBar() {
    applyStyle("NavigationBar");
}

bool NavigationBar::applyStyleValue(const string& name, const StyleValue* value) {
    if (name == "background") {
        setBackgroundColor(value->colorVal()); // ONLY COLOR BACKGROUNDS ALLOWED ON NAVBAR
        return true;
    }
    if (name == "title") {
        _titleStyle = *value;
        return true;
    }
    if (name == "preferredContentHeight") {
        _preferredContentHeight = value->floatVal();
        return true;
    }
    return View::applyStyleValue(name, value);
}

void NavigationBar::updateContentSize(SIZE constrainingSize) {
    _contentSize.height = _preferredContentHeight;
}

void NavigationBar::addNavigationItem(NavigationItem* navigationItem) {
    addSubview(navigationItem->_leftButtonsFrame);
    if (navigationItem->_title.length()) {
        Label* titleLabel = new Label();
        titleLabel->setMeasureSpecs(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
        titleLabel->setAlignSpecs(ALIGNSPEC::Center(), ALIGNSPEC(NULL, 0.5f, -0.5f, 0));
        titleLabel->setText(navigationItem->_title);
        titleLabel->applyStyle(_titleStyle);
        navigationItem->_titleView = titleLabel;
    }
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
    RenderOp* op = _blurEnabled ? (RenderOp*)new BlurRenderOp() : new RectRenderOp();
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


