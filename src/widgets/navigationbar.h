//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class NavigationBar : public View {
public:

	bool _blurEnabled;
    COLOR _backgroundColor;

    // API
	NavigationBar();
    virtual void addNavigationItem(NavigationItem* navigationItem);
    virtual void removeNavigationItem(NavigationItem* navigationItem);
    virtual void setBlurEnabled(bool blurEnabled);
    float getPreferredContentHeight() { return _preferredContentHeight; }

    // Overrides
    bool applyStyleValue(const string& name, const StyleValue* value) override;
    void setBackground(RenderOp* renderOp) override;
	void setBackgroundColor(COLOR backgroundColor) override;
    void updateContentSize(SIZE constrainingSize) override;

protected:
    StyleValue _titleStyle;
    float _preferredContentHeight;
};


