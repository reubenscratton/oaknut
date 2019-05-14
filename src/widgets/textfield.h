//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class TextField  : public View {
public:
    
    // API
    TextField();
    EditText* editText;
    void setText(const string& text);
    
    // Overrides
    bool applySingleStyle(const string& name, const style& value) override;
    bool requestFocus() override;
    
protected:
    Label* _label;
    
};

