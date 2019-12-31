//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


ViewController::ViewController() : _view(NULL)/*, _window(NULL)*/ {
}
ViewController::~ViewController() {
}

View* ViewController::inflate(const string& layoutAsset) {
    View* view = app->layoutInflate(layoutAsset);
    assert(view);
    setView(view);
    return view;
}


View* ViewController::getView() const {
    return _view;
}
Window* ViewController::getWindow() const {
    return _view ? _view->getWindow() : NULL;
}

void ViewController::setView(View* view) {
    Window* window = NULL;
	if (_view) {
        window = _view->_window;
		_view->detachFromWindow();
	}
	_view = view;
	if (view) {
        view->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Fill());
		if (window) {
			view->attachToWindow(window);
		}
	}
	if (window) {
		window->requestRedraw();
	}
}


void ViewController::onWindowAttached() {
}
void ViewController::onWindowDetached() {
}
void ViewController::onWillAppear(bool firstTime) {
}
void ViewController::onDidAppear(bool firstTime) {
}
void ViewController::onWillDisappear(bool lastTime) {
}
void ViewController::onDidDisappear(bool lastTime) {
}


void ViewController::onBackButtonClicked() {
	if (_navigationController) {
		_navigationController->popViewController();
	}
}

void ViewController::applySafeInsets(const EDGEINSETS& safeInsets) {
}

bool ViewController::navigateBack() {
	return false;
}

void ViewController::requestScroll(float dx, float dy) {
    _view->scrollBy({dx, dy});
}

ToolbarButton* ViewController::addNavButton(bool rightSide, const string& assetPath, std::function<void()> onClick) {
    sp<LinearLayout>& frame = rightSide ? _rightButtonsFrame : _leftButtonsFrame;
    if (!frame) {
        frame = new LinearLayout();
        frame->_orientation = LinearLayout::Horizontal;
        frame->setLayoutSize(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
        frame->setLayoutOrigin(rightSide ? ALIGNSPEC::Right() : ALIGNSPEC::Left(), ALIGNSPEC::Center());
    }
    ToolbarButton* button = new ToolbarButton();
    button->setImageAsset(assetPath);
    button->onClick = onClick;
    frame->addSubview(button);
    return button;
}

string ViewController::getTitle() const {
    return _title;
}
void ViewController::setTitle(const string& title) {
    if (_titleView) {
        _titleView->removeFromParent();
        _titleView = NULL;
    }
    _title = title;
}

void ViewController::setTitleView(View* titleView) {
    if (_titleView) {
        _titleView->removeFromParent();
    }
    titleView->setLayoutSize(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
    titleView->setLayoutOrigin(ALIGNSPEC::Center(), ALIGNSPEC(NULL, 0.5f, -0.5f, 0));
    _titleView = titleView;
    _title = "";
}


