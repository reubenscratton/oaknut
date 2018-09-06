//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


ViewController::ViewController() : _view(NULL), _window(NULL) {
	_navigationItem = new NavigationItem();
}
ViewController::~ViewController() {
}

void ViewController::attachToWindow(Window* window) {
	detachFromWindow();
	_window = window;
	setView(_view);
}

void ViewController::detachFromWindow() {
	View* view = _view;
	if (view) {
		view->detachFromWindow();
	}
	_window = NULL;
}

View* ViewController::getView() {
    return _view;
}

void ViewController::setView(View* view) {
	if (_view) {
		_view->detachFromWindow();
	}
	_view = view;
	if (view) {
        view->setMeasureSpecs(MEASURESPEC::FillParent(), MEASURESPEC::FillParent());
		if (_window) {
			view->attachToWindow(_window);
		}
	}
	if (_window) {
		_window->requestRedraw();
	}
}



void ViewController::onWillResume() {
}
void ViewController::onDidResume() {
}
void ViewController::onWillPause() {
}
void ViewController::onDidPause() {
}

void ViewController::onBackButtonClicked() {
	if (_navigationController) {
		_navigationController->popViewController();
	}
}


void ViewController::setSafeAreaInsets(const EDGEINSETS& safeAreaInsets) {
    _view->setPadding(safeAreaInsets);
}

bool ViewController::navigateBack() {
	return false;
}