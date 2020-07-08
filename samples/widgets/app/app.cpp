//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


class MDButton : public Button {
public:
    MDButton() : Button() {
    }
    bool handleInputEvent(INPUTEVENT* event) override {
        if (event->type == INPUT_EVENT_DOWN) {
            POINT dc = event->ptLocal;
            dc.x -= _rect.size.width/2;
            dc.y -= _rect.size.height/2;
            _inkOp->setOrigin(dc);
            _inkOp->setAlpha(1);
            _inkOp->_rippleAnim = Animation::start(this, 500, [=](float val) {
                _inkOp->setRadius(500*val);
            });
        }
        if (event->type == INPUT_EVENT_UP) {
            _inkOp->_fadeAnim = Animation::start(this, 500, [=](float val) {
                _inkOp->setAlpha(1-val);
            });
        }
        return Button::handleInputEvent(event);
    }

};

DECLARE_DYNCREATE(MDButton);

class Tabs : public View {
public:

    Tabs() : View() {
    }

    // Material Design Tabs are a horizontal array of text-style buttons without rounded corners.
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

