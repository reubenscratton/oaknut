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
    virtual void addViewControllerNav(ViewController* viewController);
    virtual void removeViewControllerNav(ViewController* viewController);
    virtual void setBlurEnabled(bool blurEnabled);
    float getPreferredContentHeight() { return _preferredContentHeight; }

    // Overrides
    bool applySingleStyle(const string& name, const style& value) override;
    void setBackground(RenderOp* renderOp) override;
	void setBackgroundColor(COLOR backgroundColor) override;
    void updateContentSize(SIZE constrainingSize) override;

protected:
    const style* _titleStyle;
    float _preferredContentHeight;
};


