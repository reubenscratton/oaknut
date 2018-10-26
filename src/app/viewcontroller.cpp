//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


ViewController::ViewController() : _view(NULL), _window(NULL) {
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
    attachChildVCsToWindow(window);
}

void ViewController::attachChildVCsToWindow(Window* window) {
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

ToolbarButton* ViewController::addNavButton(bool rightSide, const string& assetPath, std::function<void()> onClick) {
    sp<LinearLayout>& frame = rightSide ? _rightButtonsFrame : _leftButtonsFrame;
    if (!frame) {
        frame = new LinearLayout();
        frame->_orientation = LinearLayout::Horizontal;
        frame->setMeasureSpecs(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
        frame->setAlignSpecs(rightSide ? ALIGNSPEC::Right() : ALIGNSPEC::Left(), ALIGNSPEC::Center());
    }
    ToolbarButton* button = new ToolbarButton();
    ByteBuffer* data = app.loadAsset(assetPath.data());
    Bitmap::createFromData(data->data, (int)data->cb, [=](Bitmap* bitmap) {
        button->setBitmap(bitmap);
    });
    button->onClick = onClick;
    frame->addSubview(button);
    return button;
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
    titleView->setMeasureSpecs(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
    titleView->setAlignSpecs(ALIGNSPEC::Center(), ALIGNSPEC(NULL, 0.5f, -0.5f, 0));
    _titleView = titleView;
    _title = "";
}


