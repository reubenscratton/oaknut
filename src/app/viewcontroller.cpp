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
    if (window->_rootViewController == this) {
        setView(_view);
    }
    for (auto it : _childViewControllers) {
        it->attachToWindow(window);
    }
}

void ViewController::detachFromWindow() {
    if (_window && _window->_rootViewController == this) {
        View* view = _view;
        if (view) {
            view->detachFromWindow();
        }
    }
    for (auto it : _childViewControllers) {
        it->detachFromWindow();
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

void ViewController::updateSafeArea(const RECT& safeArea) {
    _safeArea = safeArea;
    EDGEINSETS padding;
    padding.left = safeArea.left() - _window->_surfaceRect.left();
    padding.top = safeArea.top() - _window->_surfaceRect.top();
    padding.right = _window->_surfaceRect.right() - safeArea.right();
    padding.bottom =  _window->_surfaceRect.bottom() - safeArea.bottom();
    _view->setPadding(padding);
}

bool ViewController::navigateBack() {
	return false;
}

void ViewController::addChildViewController(ViewController* childVC) {
    _childViewControllers.push_back(childVC);
    
    // Give the new chld VC appropriate safe area insets
    if (_window) {
        childVC->attachToWindow(_window);
        updateChildSafeArea(childVC, _safeArea);
    }

}

void ViewController::updateChildSafeArea(ViewController* childVc, const RECT& safeArea) {
    childVc->updateSafeArea(safeArea);
}
