//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup app_group
 * @brief Root view controller for typical mobile UX where there is a bar at the top of the screen
 * and pages are pushed and popped in and out of view.
 */
class NavigationController : public ViewController {
public:
	
	sp<ViewController> _currentViewController;
	sp<ViewController> _incomingViewController;
	vector<sp<ViewController>> _navStack;
	sp<class NavigationBar> _navBar;
	sp<View> _contentView;
	typedef enum {
		None,
		Push,
		Pop
	} AnimationState;
	AnimationState _animationState;
	
	NavigationController();
	virtual void pushViewController(ViewController* vc);
	virtual void popViewController();

    void onWindowAttached() override;
    void onWindowDetached() override;
	bool navigateBack() override;
    void requestScroll(float dx, float dy) override;
    void applySafeInsets(const EDGEINSETS& safeInsets) override;
    
    void applySafeInsetsToChild(ViewController* childVC);
    

protected:
	virtual void startNavAnimation(ViewController* incomingVC, AnimationState animationState);
	virtual void applyNavTransitionToViewController(ViewController* vc, float val, bool incoming);
	virtual void onNavTransitionApply(float val);
	virtual void completeIncoming();
};

