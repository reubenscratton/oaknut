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

bool NavigationBar::applySingleStyle(const string& name, const style& value) {
    if (name == "background") {
        setBackgroundColor(value.colorVal()); // ONLY COLOR BACKGROUNDS ALLOWED ON NAVBAR
        return true;
    }
    if (name == "title") {
        _titleStyle = &value;
        return true;
    }
    if (name == "preferredContentHeight") {
        _preferredContentHeight = value.floatVal();
        return true;
    }
    return View::applySingleStyle(name, value);
}

void NavigationBar::updateContentSize(SIZE constrainingSize) {
    _contentSize.height = _preferredContentHeight;
}

void NavigationBar::addViewControllerNav(ViewController* viewController) {
    if (viewController->_leftButtonsFrame) {
        addSubview(viewController->_leftButtonsFrame);
    }
    if (viewController->_title.length()) {
        Label* titleLabel = new Label();
        titleLabel->setLayoutSize(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
        titleLabel->setLayoutOrigin(ALIGNSPEC::Center(), ALIGNSPEC(NULL, 0.5f, -0.5f, 0));
        titleLabel->setText(viewController->_title);
        titleLabel->applyStyle(*_titleStyle);
        viewController->_titleView = titleLabel;
    }
    if (viewController->_titleView) {
        addSubview(viewController->_titleView);
    }
    if (viewController->_rightButtonsFrame) {
        addSubview(viewController->_rightButtonsFrame);
    }
}
void NavigationBar::removeViewControllerNav(ViewController* viewController) {
    if (viewController->_leftButtonsFrame) {
        viewController->_leftButtonsFrame->removeFromParent();
    }
    if (viewController->_titleView) {
        viewController->_titleView->removeFromParent();
    }
    if (viewController->_rightButtonsFrame) {
        viewController->_rightButtonsFrame->removeFromParent();
    }
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


