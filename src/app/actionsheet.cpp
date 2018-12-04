//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

class ActionLabel : public Label {
public:
    ActionLabel(const string& text) {
        applyStyle("ActionSheet.Action");
        setText(text);
    }
    
    void setBackground(RenderOp* op) override {
        applyCornersToOp(op);
        Label::setBackground(op);
    }
    
    void applyCornersToOp(RenderOp* op) {
        float r = app.getStyleFloat("ActionSheet.corner-radius");
        VECTOR4 radii = {_isTop?r:0, _isTop?r:0, _isBottom?r:0, _isBottom?r:0};
        ((RectRenderOp*)op)->setCornerRadii(radii);
        if (!_isTop) {
            if (!_divider) {
                _divider = new View();
                _divider->setMeasureSpecs(MEASURESPEC::Fill(), MEASURESPEC::Abs(1));
                _divider->setBackgroundColor(app.getStyleColor("ActionSheet.divider-color"));
                addSubview(_divider);
            }
        } else {
            if (_divider) {
                removeSubview(_divider);
                _divider = NULL;
            }
        }
    }
    
    void setCorners(bool isTop, bool isBottom) {
        _isTop = isTop;
        _isBottom = isBottom;
        applyCornersToOp(_backgroundOp);
    }
    bool _isTop;
    bool _isBottom;
    View* _divider;
};

class ActionGroup : public LinearLayout {
public:
    ActionGroup() {
        applyStyle("ActionSheet.Group");
    }
    
    ActionLabel* addAction(const string& text, std::function<void()> action, const string& extraStyle=NULL) {
        ActionLabel* actionLabel = new ActionLabel(text);
        if (extraStyle.length()) {
            actionLabel->applyStyle(extraStyle);
        }
        if (action) {
            actionLabel->onClick = action;
        } else {
            actionLabel->setEnabled(false);
        }
        addSubview(actionLabel);
        int last = (int)_subviews.size()-1;
        for (int i=0 ; i<=last ; i++) {
            actionLabel = (ActionLabel*)getSubview(i);
            actionLabel->setCorners(i==0, i==last);
        }
        return actionLabel;
    }
};

ActionSheet::ActionSheet() {
    View* view = new View();
    setView(view);

    _outerGroup = new LinearLayout();
    view->addSubview(_outerGroup);
    _outerGroup->applyStyle("ActionSheet.OuterGroup");
}


void ActionSheet::onWindowAttached() {
    ViewController::onWindowAttached();

    // Create the main group
    ActionGroup* group = new ActionGroup();
    _outerGroup->addSubview(group);
    if (_title.length()) {
        group->addAction(_title, nullptr, "ActionSheet.Title");
    }
    for (auto& action : _actions) {
        group->addAction(action.first, [=]() {
            dismissWithAction(action.second);
        });
    }
 
    // Create the cancel group separately
    if (_hasCancel) {
        ActionGroup* cancelGroup = new ActionGroup();
        cancelGroup->addAction("Cancel", [=]() { dismissWithAction(nullptr); }, "ActionSheet.Cancel");
        _outerGroup->addSubview(cancelGroup);
    }

}

void ActionSheet::setTitle(const string& title) {
    _title = title;
}

void ActionSheet::addAction(const string& action, std::function<void()> onClick) {
    _actions.push_back(make_pair(action, onClick));
}

void ActionSheet::addCancelButton() {
    _hasCancel = true;
}

void ActionSheet::dismissWithAction(std::function<void()> action) {
    getWindow()->dismissModalViewController(this, action);
}
