//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

ActionSheet::ActionSheet() {
    View* view = new View();
    setView(view);

    _outerGroup = new LinearLayout();
    view->addSubview(_outerGroup);
    _outerGroup->applyStyle("ActionSheet.OuterGroup");

    LinearLayout* group = new LinearLayout();
    _outerGroup->addSubview(group);
    group->applyStyle("ActionSheet.Group");
    
    Label* titleLabel = new Label();
    titleLabel->applyStyle("ActionSheet.Title");
    titleLabel->setText("Title goes here");
    group->addSubview(titleLabel);
    
    RectRenderOp* groupBkgndOp = (RectRenderOp*)group->getBackgroundOp();
    for (int i=0 ; i<3 ; i++) {
        Label* actionLabel = new Label();
        actionLabel->applyStyle("ActionSheet.Action");
        actionLabel->setText("Action");
        bool roundTopCorners = (i==0);
        bool roundBottomCorners = (i==2);
        
        group->addSubview(actionLabel);
    }
    
    LinearLayout* group2 = new LinearLayout();
    _outerGroup->addSubview(group2);
    group2->applyStyle("ActionSheet.Group");
    Label* cancelLabel = new Label();
    cancelLabel->applyStyle("ActionSheet.Cancel");
    cancelLabel->setText("Cancel");
    group2->addSubview(cancelLabel);

}

void ActionSheet::attachToWindow(Window *window) {
    ViewController::attachToWindow(window);
    Animation::start(_view, 333, [=](float val) {
        _view->setBackgroundColor(COLOR::interpolate(0, 0x80000000, val));
    });
    _outerGroup->animateInFromBottom(333);
}

void ActionSheet::setTitle(const string& title) {
    
}

void ActionSheet::addAction(const string& action, std::function<void()> onClick) {
    
}

void ActionSheet::addCancelButton() {
    
}
    
