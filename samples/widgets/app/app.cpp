//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


class Drawer : public LinearLayout {
public:

    Drawer() : LinearLayout() {
        _orientation = Vertical;
    }

    bool applySingleStyle(const string& name, const style& value) override {
        return LinearLayout::applySingleStyle(name, value);
    }


};
DECLARE_DYNCREATE(Drawer);



class Tabs : public LinearLayout {
protected:
    sp<RectRenderOp> _selectedOp;
    float _barHeight;
    
public:

    Tabs() : LinearLayout() {
        _selectedOp = new RectRenderOp();
        _hideScrollbars = true;
        addDecorOp(_selectedOp);
    }

    bool applySingleStyle(const string& name, const style& value) override {
        if (name == "bar-color") {
            _selectedOp->setFillColor(value.colorVal());
            return true;
        }
        if (name == "bar-height") {
            _barHeight = value.floatVal();
            setNeedsLayout();
            return true;
        }
        if (name == "tabs") {
            auto tabs = value.arrayVal();
            for (auto& vtab : tabs) {
                auto title = vtab.stringVal();
                Button* button = new Button();
                button->setSelectable(true);
                button->applyStyle("material_design.Tabs.Button");
                button->setText(title);
                button->setImage("star.png");
                addSubview(button);
            }
            return true;
        }
        return LinearLayout::applySingleStyle(name, value);
    }

    void setSelectedSubview(View* subview) override {
        for (auto& it : _subviews) {
            if (!it->isSelectable()) {
                continue;
            }
            bool selected = subview == it._obj;
            it->setSelected(selected);
            if (selected) {
                RECT oldRect = _selectedOp->_rect;
                RECT rectTarget = it->getRect();
                rectTarget.origin.y = rectTarget.bottom() - _barHeight;
                rectTarget.size.height = _barHeight;
                if (oldRect.size.width <=0) {
                    _selectedOp->setRect(rectTarget);
                } else {
                Animation::start(this, 300, [=](float val) {
                    RECT rect = rectTarget;
                    rect.origin.x = oldRect.origin.x + (rectTarget.origin.x-oldRect.origin.x) * val;
                    rect.size.width = oldRect.size.width + (rectTarget.size.width-oldRect.size.width) * val;
                    _selectedOp->setRect(rect);
                }, Animation::strongEaseOut);
                }
            }
        }

    }

};
DECLARE_DYNCREATE(Tabs);

class MainViewController : public ViewController {
public:

    MainViewController() {
        inflate("layout/main.res");
        bind(_drawer, "drawer");
        
        /*ShadowRenderOp* shadow = new ShadowRenderOp();
        shadow->setSigma(0);
        shadow->setRect(RECT(50,50,200,200));
        view->addRenderOp(shadow);*/
    }

    Drawer* _drawer;
    
};


class WidgetsApp : public App {
public:


    void main() override {
        
  
        MainViewController* mainVC = new MainViewController();
        _window->setRootViewController(mainVC);
    };

};

static WidgetsApp the_app;

