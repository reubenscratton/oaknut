//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>




class Tabs : public LinearLayout {
public:

    Tabs() : LinearLayout() {
    }

    // Material Design Tabs are a horizontal array of text-style buttons without rounded corners.
    
    bool applySingleStyle(const string& name, const style& value) override {
        if (name == "tabs") {
            auto tabs = value.arrayVal();
            for (auto& vtab : tabs) {
                auto title = vtab.stringVal();
                Button* button = new Button();
                button->applyStyle("material_design.Tabs.Button");
                button->setText(title);
                addSubview(button);
            }
            return true;
        }
        return LinearLayout::applySingleStyle(name, value);
    }

};
DECLARE_DYNCREATE(Tabs);

class MainViewController : public ViewController {
public:

    MainViewController() {
        setView(inflate("layout/main.res"));
        
        /*ShadowRenderOp* shadow = new ShadowRenderOp();
        shadow->setSigma(0);
        shadow->setRect(RECT(50,50,200,200));
        view->addRenderOp(shadow);*/
    }


};


class WidgetsApp : public App {
public:


    void main() override {
        
        MainViewController* mainVC = new MainViewController();
        _window->setRootViewController(mainVC);
    };

};

static WidgetsApp the_app;

