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

void NavigationController::updateSafeArea(const RECT& safeArea) {
    _safeArea = safeArea;
    _navBar->setPadding({0,safeArea.top(),0,0});
    // don't apply safe area to root view cos we want to extend our navbar under status bar rather than have padding
    if (_currentViewController) {
        updateChildSafeArea(_currentViewController, safeArea);
    }
    if (_incomingViewController) {
        updateChildSafeArea(_incomingViewController, safeArea);
    }
    /*for (auto it : _navStack) {
        updateChildSafeArea(it, safeArea);
    }*/
}

void NavigationController::updateChildSafeArea(ViewController* childVc, const RECT& safeArea) {
    RECT childSafeArea = safeArea;
    EDGEINSETS navbarInsets = {0,_navBar->getPreferredContentHeight(),0,0};
    navbarInsets.applyToRect(childSafeArea);
    childVc->updateSafeArea(childSafeArea);
}

void NavigationController::pushViewController(ViewController* vc) {
    if (_childViewControllers.size()) {
        if (!vc->_leftButtonsFrame) {
            vc->addNavButton(false, "images/back.png", [=] () { vc->onBackButtonClicked(); });
        }
    }
    addChildViewController(vc);

    if (!_currentViewController) {
		vc->_navigationController = this;
		_navBar->addViewControllerNav(vc);
		_currentViewController = vc;
		_currentViewController->onWillResume();
		_contentView->addSubview(vc->getView());
		_currentViewController->onDidResume();
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
	_currentViewController->onWillPause();
    
	_incomingViewController = incomingVC;
	_incomingViewController->_navigationController = this;
    _navBar->addViewControllerNav(incomingVC);
	_incomingViewController->onWillResume();
    
	_contentView->addSubview(_incomingViewController->getView());

	if(!_window) {
		completeIncoming();
		return;
	}

	// Do an initial apply so the translation and alpha on the incoming view & navitem is correct
	onNavTransitionApply(0);

	// Create the animation
    Animation* anim = Animation::start(_view, 350, [=](float val) {
        onNavTransitionApply(val);
    });
    anim->_interpolator = Animation::regularEaseInOut;
    anim->_onFinished = [=](Animation* anim) {
		completeIncoming();
        _animationState = None;
    };
	_view->setNeedsLayout();
}

void NavigationController::attachChildVCsToWindow(Window* window) {
    if (_currentViewController) {
        _currentViewController->attachToWindow(window);
    }
}

void NavigationController::completeIncoming() {
	_currentViewController->getView()->removeFromParent();
	_navBar->removeViewControllerNav(_currentViewController);
	_currentViewController->onDidPause();
	_currentViewController = _incomingViewController;
	_incomingViewController = NULL;
	_currentViewController->onDidResume();

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


void NavigationController::onWillResume() {
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
}

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

