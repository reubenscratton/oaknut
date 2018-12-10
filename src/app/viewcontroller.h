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
     When in a NavigationController this object provides title and button information to the nav bar 
     */
	class NavigationController* _navigationController;
    virtual string getTitle() const;
    virtual void setTitle(const string& title);
    virtual void setTitleView(View* titleView);
    virtual class ToolbarButton* addNavButton(bool rightSide, const string& assetPath, std::function<void()> onClick);

	ViewController();
    ~ViewController();
    
    /**
     * Get the root view
     */
    virtual View* getView() const;

    /**
     * Set the root view
     */
    virtual void setView(View* view);
    
    /**
     * Get the Window that the root view is attached to
     */
    virtual Window* getWindow() const;
    
    /**@}*/
    
    virtual View* inflate(const string& layoutAsset);
    
    template<class T>
    void bind(T*& rview, const string& id) {
        rview = (T*)_view->findViewById(id);
    }


    /** @name Events
     * @{
     */
    
    /**
     * Called when this view controller is attached to the window
     */
    virtual void onWindowAttached();
    virtual void onWindowDetached();

    /**
     * Navigation events
     */
    virtual void onWillAppear(bool firstTime);
    virtual void onDidAppear(bool firstTime);
    virtual void onWillDisappear(bool lastTime);
    virtual void onDidDisappear(bool lastTime);

    
    /**
     * Called when the user navigates 'back'
     */
    virtual void onBackButtonClicked();
    /**@}*/


    virtual void applySafeInsets(const EDGEINSETS& safeInsets);

    virtual bool navigateBack();

    virtual void requestScroll(float dx, float dy);

protected:
    sp<View> _view;
    bool _viewHasSafeAreaPaddingApplied;
    EDGEINSETS _safeAreaInsets;
    
    // Navigation data
    string _title;
    sp<View> _titleView;
    sp<class LinearLayout> _leftButtonsFrame;
    sp<LinearLayout> _rightButtonsFrame;
    friend class NavigationBar;
    friend class NavigationController;
};

