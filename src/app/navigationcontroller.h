//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup app_group
 * @brief Root view controller for typical mobile UX where there is a bar at the top of the screen
 * and pages are pushed and popped into view.
 */
class NavigationController : public ViewController {
public:
	
	ObjPtr<ViewController> _currentViewController;
	ObjPtr<ViewController> _incomingViewController;
	vector<ObjPtr<ViewController>> _navStack;
	ObjPtr<NavigationBar> _navBar;
	ObjPtr<View> _contentView;
	typedef enum {
		None,
		Push,
		Pop
	} AnimationState;
	AnimationState _animationState;
	
	NavigationController();
	virtual void pushViewController(ViewController* vc);
	virtual void popViewController();
	virtual void onWillResume();
	virtual void onDidResume();
	virtual void onWillPause();
	virtual void onDidPause();
	


protected:
	virtual void startNavAnimation(ViewController* incomingVC, AnimationState animationState);
	virtual void applyNavTransitionToViewController(ViewController* vc, float val, bool incoming);
	virtual void onNavTransitionApply(float val);
};

