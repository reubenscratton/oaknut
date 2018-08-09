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

ToolbarButton::ToolbarButton() {
    applyStyle("ToolbarButton");
}

bool ToolbarButton::onInputEvent(INPUTEVENT* event) {
    
    // TODO: this is an iOS-only effect
    if (event->type == INPUT_EVENT_DOWN) {
        setAlpha(0.5f);
    }
    if (event->type == INPUT_EVENT_CANCEL || event->type==INPUT_EVENT_UP) {
        if (_alpha == 0.5f) {
            animateAlpha(1.0, 350);
        }
    }
    
    return ImageView::onInputEvent(event);
}
