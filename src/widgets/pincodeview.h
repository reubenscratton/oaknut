//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class PinCodeView : public LinearLayout {
public:
    PinCodeView();
    
    // API
    string getText();
    void clear();
    std::function<void(bool)> onFilled;
    std::function<void()> onKeyboardAction;
    
    // Overrides
    bool applyStyleValue(const string &name, const StyleValue *value) override;
    
};
