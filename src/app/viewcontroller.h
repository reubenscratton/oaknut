//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class ViewController : public Object {
public:

	ObjPtr<View> _view;
	class Window* _window;
	ObjPtr<NavigationItem> _navigationItem;
	class NavViewController* _navigationController;


	ViewController();
    ~ViewController();
	virtual void setView(View* view);
	virtual void attachToWindow(Window* window);
	virtual void detachFromWindow();
	virtual View* dispatchTouchEvent(int eventType, int finger, long time, POINT pt);
	
	virtual void onWillResume();
	virtual void onDidResume();
	virtual void onWillPause();
	virtual void onDidPause();
	
	virtual void onBackButtonClicked();

};

class NavViewController : public ViewController {
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
	
	NavViewController();
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

