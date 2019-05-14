//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(TextField);

TextField::TextField() {
    _label = new Label();
    addSubview(_label);
    editText = new EditText();
    addSubview(editText);
    applyStyle("TextField");
}

bool TextField::applySingleStyle(const string& name, const style& value) {
    if (name == "label") {
        if (value.isString()) {
            _label->setText(value.stringVal());
        } else {
            _label->applyStyle(value);
        }
        return true;
    }
    if (name == "actionType" || name == "next") {
        return editText->applySingleStyle(name, value);
    }
    if (name == "edittext") {
        editText->applyStyle(value);
        return true;
    }
    return View::applySingleStyle(name, value);
}

bool TextField::requestFocus() {
    return editText->requestFocus();
}

void TextField::setText(const string &text) {
    editText->setText(text);
}
