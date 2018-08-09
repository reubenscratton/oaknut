//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup app_group
 * @brief Owner of a set of views that collectively make up a discrete piece of user-facing behaviour.
 * It is most often a complete screen but doesn't have to be.
 */
class ViewController : public Object {
public:

    /**
     The window this ViewController is attached to. This is non-null while the ViewController is active
     and also while it is being animated in and out of place.
     */
	class Window* _window;
    
    /**
     When in a NavigationController this object provides title and button information to the nav bar 
     */
	ObjPtr<NavigationItem> _navigationItem;
	class NavigationController* _navigationController;


	ViewController();
    ~ViewController();
    
    /** @name Root view
     * @{
     */
    
    /**
     * Get the root view
     */
    virtual View* getView();

    /**
     * Set the root view
     */
    virtual void setView(View* view);
    
    /**@}*/


    /** @name Events
     * @{
     */
    
    /**
     * Called when this view controller is about to become active
     */
    virtual void onWillResume();

    /**
     * Called when this view controller is now active
     */
    virtual void onDidResume();
    
    /**
     * Called when this view controller is about to become replaced by another
     */
    virtual void onWillPause();
    
    /**
     * Called when this view controller has moved into the background
     */
    virtual void onDidPause();
    
    /**
     * Called when the user navigates 'back'
     */
    virtual void onBackButtonClicked();
    /**@}*/


    /**
     * Called when this controller's view is attached to the window's view tree.
     */
	virtual void attachToWindow(Window* window);

    /**
     * Called when this controller's view is removed from the window's view tree.
     */
    virtual void detachFromWindow();
    
    /**
     * Helper for dispatching touch events from the host OS.
     */
	//virtual View* dispatchTouchEvent(int eventType, int finger, TIMESTAMP time, POINT pt);
	

protected:
    ObjPtr<View> _view;

};

