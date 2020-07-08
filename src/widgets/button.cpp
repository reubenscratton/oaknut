//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(Button);
DECLARE_DYNCREATE(ToolbarButton);


Button::Button() {
    applyStyle("Button");
}

bool Button::applySingleStyle(const string& name, const style& value) {
    if (name == "ink") {
        setInkColor(value.colorVal());
        _inkOp->setCornerRadius(8);
        return true;
    }
    return Label::applySingleStyle(name, value);
}

void Button::setInkColor(COLOR inkColor) {
    if (!inkColor) {
        if (_inkOp) {
            removeRenderOp(_inkOp);
            _inkOp = nullptr;
        }
    } else {
        if (!_inkOp) {
            _inkOp = new InkRenderOp();
            _inkOp->setAlpha(1);
            addRenderOp(_inkOp);
        }
        _inkOp->setColor(inkColor);
    }
}

void Button::updateBackgroundRect() {
    Label::updateBackgroundRect();
    if (_inkOp) {
        _inkOp->setRect(getOwnRect());
    }
}

ToolbarButton::ToolbarButton() {
    applyStyle("ToolbarButton");
}

bool ToolbarButton::handleInputEvent(INPUTEVENT* event) {
    
    // TODO: this is an iOS-only effect
    if (event->type == INPUT_EVENT_DOWN) {
        setAlpha(0.5f);
    }
    if (event->type == INPUT_EVENT_TAP_CANCEL || event->type==INPUT_EVENT_UP) {
        if (_alpha == 0.5f) {
            animateAlpha(1.0, 350);
        }
    }
    
    return ImageView::handleInputEvent(event);
}
