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

View* ViewController::inflate(const string& layoutAsset) {
    View* view = app.layoutInflate(layoutAsset);
    assert(view);
    setView(view);
    return view;
}

void ViewController::attachToWindow(Window* window) {
	detachFromWindow();
	_window = window;
    _view->attachToWindow(window);
    for (auto it : _childViewControllers) {
        it->attachToWindow(window);
    }
}

void ViewController::detachFromWindow() {
    if (_window) {
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
    _viewHasSafeAreaPaddingApplied = false;
	if (view) {
        view->setMeasureSpecs(MEASURESPEC::Fill(), MEASURESPEC::Fill());
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
    
    // Get view padding and unapply previous safe area
    EDGEINSETS padding = _view->_padding;
    if (_viewHasSafeAreaPaddingApplied) {
        padding.left -= _safeAreaInsets.left;
        padding.right -= _safeAreaInsets.right;
        padding.top -= _safeAreaInsets.top;
        padding.bottom -= _safeAreaInsets.bottom;
    }
    
    // Convert safe area rect to edgeinsets
    _safeAreaInsets.left = safeArea.left() - _window->_surfaceRect.left();
    _safeAreaInsets.top = safeArea.top() - _window->_surfaceRect.top();
    _safeAreaInsets.right = _window->_surfaceRect.right() - safeArea.right();
    _safeAreaInsets.bottom =  _window->_surfaceRect.bottom() - safeArea.bottom();

    _safeArea = safeArea;

    // Add safe area insets to view padding
    padding.left += _safeAreaInsets.left;
    padding.top += _safeAreaInsets.top;
    padding.right += _safeAreaInsets.right;
    padding.bottom += _safeAreaInsets.bottom;

    _viewHasSafeAreaPaddingApplied = true;
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

void ViewController::requestScroll(float dx, float dy) {
    _view->scrollBy({dx, dy});
}
