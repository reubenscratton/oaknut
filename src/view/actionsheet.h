//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class ActionSheet : public ViewController {
public:

    // API
    ActionSheet();
    virtual void setTitle(const string& title);
    virtual void addAction(const string& action, std::function<void()> onClick);
    virtual void addCancelButton();
    
    // Overrides
    void attachToWindow(Window *window) override;
    
protected:
    LinearLayout* _outerGroup;
    string _title;
    vector<pair<string, std::function<void()>>> _actions;
};
