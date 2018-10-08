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
    _label->setMeasureSpecs(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
    addSubview(_label);
    editText = new EditText();
    editText->setMeasureSpecs(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
    editText->setAlignSpecs(ALIGNSPEC::Left(), ALIGNSPEC::Below(_label, app.dp(4)));
    addSubview(editText);
}

bool TextField::applyStyleValue(const string& name, const StyleValue* value) {
    if (name == "label") {
        _label->applyStyle(*value);
        return true;
    }
    return View::applyStyleValue(name, value);
}


