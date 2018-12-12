//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


NavigationController::NavigationController() {

	_view = new View();
    _view->setMeasureSpecs(MEASURESPEC::Fill(), MEASURESPEC::Fill());
	
	_contentView = new View();
    _contentView->setMeasureSpecs(MEASURESPEC::Fill(), MEASURESPEC::Fill());
	_view->addSubview(_contentView);

	// Navbar
	_navBar = new NavigationBar();
	_view->addSubview(_navBar);

}


void NavigationController::onWindowAttached() {
    ViewController::onWindowAttached();
    for (auto vc : _navStack) {
        vc->onWindowAttached();
    }
    if (_currentViewController) {
        _currentViewController->onWindowAttached();
    }
}

void NavigationController::onWindowDetached() {
    ViewController::onWindowDetached();
    for (auto vc : _navStack) {
        vc->onWindowDetached();
    }
    if (_currentViewController) {
        _currentViewController->onWindowDetached();
    }
}


void NavigationController::applySafeInsets(const EDGEINSETS& safeInsets) {

    _safeAreaInsets = safeInsets;
    _navBar->setPadding({0,safeInsets.top,0,0});
    
    if (_currentViewController) {
        applySafeInsetsToChild(_currentViewController);
    }
    if (_incomingViewController) {
        applySafeInsetsToChild(_incomingViewController);
    }
}

void NavigationController::applySafeInsetsToChild(ViewController* childVC) {
    EDGEINSETS childInsets = _safeAreaInsets;
    childInsets.top += _navBar->getPreferredContentHeight();
    app.log("applySafeInsetsToChild %f %f", childInsets.top, childInsets.bottom);
    childVC->applySafeInsets(childInsets);
}

void NavigationController::pushViewController(ViewController* vc) {
    if (_navStack.size()) {
        if (!vc->_leftButtonsFrame) {
            vc->addNavButton(false, "images/back.png", [=] () { vc->onBackButtonClicked(); });
        }
    }

    if (!_currentViewController) {
		vc->_navigationController = this;
		_navBar->addViewControllerNav(vc);
		_currentViewController = vc;
		_currentViewController->onWillAppear(true);
		_contentView->addSubview(vc->getView());
        if(getWindow()) {
            vc->onWindowAttached();
        }
        applySafeInsetsToChild(_currentViewController);
		_currentViewController->onDidAppear(true);
		return;
	}
    _navStack.push_back(_currentViewController);
	startNavAnimation(vc, Push);
}

void NavigationController::popViewController() {
	if (!_navStack.size()) {
		return;
	}
	sp<ViewController> vc  = *_navStack.rbegin();
    _navStack.pop_back();
	startNavAnimation(vc, Pop);
}

void NavigationController::startNavAnimation(ViewController* incomingVC, AnimationState animationState) {

	_animationState = animationState;
	_currentViewController->onWillDisappear(animationState==Pop);
    
	_incomingViewController = incomingVC;
	_incomingViewController->_navigationController = this;
    applySafeInsetsToChild(_incomingViewController);
    _navBar->addViewControllerNav(incomingVC);
	_incomingViewController->onWillAppear(animationState==Push);
    
	_contentView->addSubview(_incomingViewController->getView());
    if(getWindow()) {
        _incomingViewController->onWindowAttached();
    } else {
		completeIncoming();
		return;
	}

	// Do an initial apply so the translation and alpha on the incoming view & navitem is correct
	onNavTransitionApply(0);

	// Create the animation
    Animation* anim = Animation::start(_view, 350, [=](float val) {
        onNavTransitionApply(val);
    });
    anim->setInterpolater(Animation::regularEaseInOut);
    anim->onFinished = [=](Animation* anim) {
		completeIncoming();
        _animationState = None;
    };
	_view->setNeedsLayout();
}


void NavigationController::completeIncoming() {
	_currentViewController->getView()->removeFromParent();
    _currentViewController->onWindowDetached();
	_navBar->removeViewControllerNav(_currentViewController);
	_currentViewController->onDidDisappear(_animationState==Pop);
	_currentViewController = _incomingViewController;
	_incomingViewController = NULL;
	_currentViewController->onDidAppear(_animationState==Push);

}


void NavigationController::onNavTransitionApply(float val) {
	applyNavTransitionToViewController(_incomingViewController, val, true);
	applyNavTransitionToViewController(_currentViewController, val, false);
}

void NavigationController::applyNavTransitionToViewController(ViewController* vc, float val, bool incoming) {
	float tx;
	if (_animationState == Push) {
		tx = incoming ? (1-val) : -val/2;
	}
	else  {
		tx = incoming ? (val-1) : val/2;
	}
    vc->getView()->setTranslate({tx * _view->getWidth(), 0});

    bool isPop = _animationState == Pop;
    float alpha = incoming?val:(1-val);
    if (vc->_leftButtonsFrame) {
        vc->_leftButtonsFrame->setAlpha(alpha);
    }
    if (vc->_titleView) {
        vc->_titleView->setAlpha(alpha);
        float tx = incoming ? (1-val) : -val;
        float titleDistance = vc->_titleView->getWidth()/2 + _navBar->getWidth()/2;
        if (isPop) {
            vc->_titleView->setTranslate({-tx * titleDistance,0});
        } else {
            vc->_titleView->setTranslate({tx * titleDistance,0});
        }
    }
    if (vc->_rightButtonsFrame) {
        vc->_rightButtonsFrame->setAlpha(alpha);
    }
}


/*void NavigationController::onWillResume() {
	if (_currentViewController) {
		_currentViewController->onWillResume();
	}
}
void NavigationController::onDidResume() {
	if (_currentViewController) {
		_currentViewController->onDidResume();
	}
}
void NavigationController::onWillPause() {
	if (_currentViewController) {
		_currentViewController->onWillPause();
	}
}
void NavigationController::onDidPause() {
	if (_currentViewController) {
		_currentViewController->onWillPause();
	}
}*/

bool NavigationController::navigateBack() {
	if (!_navStack.size()) {
		return false;
	}
	popViewController();
	return true;
}

void NavigationController::requestScroll(float dx, float dy) {
    if (_currentViewController) {
        _currentViewController->requestScroll(dx, dy);
    }
}

