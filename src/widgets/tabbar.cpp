//
// Copyright © 2019 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(TabBar);

TabBar::TabBar() {
    applyStyle("TabBar");
}

bool TabBar::applySingleStyle(const string& name, const style& value) {
    if (name == "button-class") {
        _buttonClassName = value.stringVal();
        return true;
    }
    if (name == "buttons") {
        assert(value.isArray());
        auto a = value.arrayVal();
        string buttonClass = _buttonClassName.lengthInBytes() ? _buttonClassName: "Button";
        int buttonIndex = 0;
        for (const auto& e : a) {
            View* button = (View*)Object::createByName(buttonClass);
            button->applyStyle(e);
            button->onClick = [=]() {
                handleButtonClick(buttonIndex);
            };
            addSubview(button);
            buttonIndex++;
        }
        return true;
    }
    if (name == "selected-tint") {
        _selectedTint = value.colorVal();
        return true;
    }
    return LinearLayout::applySingleStyle(name, value);
}

void TabBar::handleButtonClick(int buttonIndex) {
    setSelectedIndex(buttonIndex);
    if (onButtonClick) {
        onButtonClick(buttonIndex, getSubview(buttonIndex));
    }
}

void TabBar::setSelectedIndex(int buttonIndex) {
    if (_selectedTint) {
        getSubview(_selectedIndex)->setTintColor(0);
        getSubview(buttonIndex)->setTintColor(_selectedTint);
    }
    _selectedIndex = buttonIndex;
    if (onSelectedIndexSet) {
        onSelectedIndexSet(_selectedIndex);
    }
}


#ifdef DEBUG
string TabBar::debugViewType() {
    return "TabBar";
}
#endif
