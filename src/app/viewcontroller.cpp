//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
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


void ViewController::setView(View* view) {
	if (_view) {
		_view->detachFromWindow();
	}
	_view = view;
	if (view) {
		view->setMeasureSpecs(MEASURESPEC_FillParent, MEASURESPEC_FillParent);
		if (_window) {
			view->attachToWindow(_window);
		}
	}
	if (_window) {
		_window->requestRedraw();
	}
}


View* ViewController::dispatchTouchEvent(int eventType, int finger, long time, POINT pt) {
	return _view->dispatchInputEvent(eventType, finger, time, pt);
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

NavViewController::NavViewController() {

	_view = new View();
	_view->setMeasureSpecs(MEASURESPEC_FillParent, MEASURESPEC_FillParent);
	
	_contentView = new View();
	_contentView->setMeasureSpecs(MEASURESPEC_FillParent, MEASURESPEC_FillParent);
	_view->addSubview(_contentView);

	// Navbar
	_navBar = new NavigationBar();
	_view->addSubview(_navBar);


}

void NavViewController::pushViewController(ViewController* vc) {
    if (!_currentViewController) {
		vc->_navigationController = this;
		_navBar->addNavigationItem(vc->_navigationItem);
		_currentViewController = vc;
		_currentViewController->onWillResume();
		_contentView->addSubview(vc->_view);
		_currentViewController->onDidResume();
		return;
	}
    _navStack.push_back(_currentViewController);
	startNavAnimation(vc, Push);
}

void NavViewController::popViewController() {
	if (!_navStack.size()) {
		return;
	}
	ObjPtr<ViewController> vc  = *_navStack.rbegin();
    _navStack.pop_back();
	startNavAnimation(vc, Pop);
}

void NavViewController::startNavAnimation(ViewController* incomingVC, AnimationState animationState) {

	_animationState = animationState;
	_currentViewController->onWillPause();
    
	_incomingViewController = incomingVC;
	_incomingViewController->_navigationController = this;
    _navBar->addNavigationItem(incomingVC->_navigationItem);
	_incomingViewController->onWillResume();
    
	_contentView->addSubview(_incomingViewController->_view);

	// Do an initial apply so the translation and alpha on the incoming view & navitem is correct
	onNavTransitionApply(0);

	// Create the animation
	DelegateAnimation* anim = new DelegateAnimation();
	anim->_interpolater = regularEaseInOut;
    anim->_delegate = [=](float val) {
        onNavTransitionApply(val);
    };
    anim->_onFinished = [=](Animation* anim) {
        _currentViewController->_view->removeFromParent();
        _navBar->removeNavigationItem(_currentViewController->_navigationItem);
        _currentViewController->onDidPause();
        _currentViewController = _incomingViewController;
        _incomingViewController = NULL;
        _currentViewController->onDidResume();
        _animationState = None;
    };
	anim->start(_window, 350);
	_view->setNeedsLayout();
}


void NavViewController::onNavTransitionApply(float val) {
	applyNavTransitionToViewController(_incomingViewController, val, true);
	applyNavTransitionToViewController(_currentViewController, val, false);
}

void NavViewController::applyNavTransitionToViewController(ViewController* vc, float val, bool incoming) {
	float tx;
	if (_animationState == Push) {
		tx = incoming ? (1-val) : -val/2;
	}
	else  {
		tx = incoming ? (val-1) : val/2;
	}
	vc->_view->setAnimTranslate(POINT_Make(tx * _view->_frame.size.width, 0));
	vc->_navigationItem->applyTransition(_navBar, val, incoming, _animationState == Pop);
}


void NavViewController::onWillResume() {
	if (_currentViewController) {
		_currentViewController->onWillResume();
	}
}
void NavViewController::onDidResume() {
	if (_currentViewController) {
		_currentViewController->onDidResume();
	}
}
void NavViewController::onWillPause() {
	if (_currentViewController) {
		_currentViewController->onWillPause();
	}
}
void NavViewController::onDidPause() {
	if (_currentViewController) {
		_currentViewController->onWillPause();
	}
}

