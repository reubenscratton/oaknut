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
    
    /**  @cond INTERNAL */
    ViewController();
    ~ViewController();
    /**  @endcond */

    /** @name Views & Window
     * @{
     */
    /** Inflate the given layout asset and set it as the root view */
    virtual View* inflate(const string& layoutAsset);
    
    /** Find a subview by ID and store it in the given variable, usually member data.  */
    template<class T>
    void bind(T*& rview, const string& id) {
        rview = (T*)_view->findViewById(id);
    }

    /** Get the root view */
    virtual View* getView() const;
    
    /** Set the root view */
    virtual void setView(View* view);
    
    /** Get the Window that the root view is attached to */
    virtual Window* getWindow() const;
    
    /**@}*/


    /** @name Navigation
     * @brief These control how the ViewController appears when in a `NavigationController`
     * @{
     */
	class NavigationController* _navigationController;
    virtual string getTitle() const;
    virtual void setTitle(const string& title);
    virtual void setTitleView(View* titleView);
    virtual class ToolbarButton* addNavButton(bool rightSide, const string& assetPath, std::function<void()> onClick);
    /**@}*/

    

    /** @name Events
     * @{
     */
    
    /** Called when this view controller is attached to a window */
    virtual void onWindowAttached();

    /** Called when this view controller is detached from a window */
    virtual void onWindowDetached();

    /** Called when this view controller is about to become the current top in a NavigationController */
    virtual void onWillAppear(bool firstTime);

    /** Called when this view controller has become current top in a NavigationController */
    virtual void onDidAppear(bool firstTime);
    
    /** Called when this view controller is about to stop being current top in a NavigationController */
    virtual void onWillDisappear(bool lastTime);

    /** Called when this view controller has stopped being current top in a NavigationController */
    virtual void onDidDisappear(bool lastTime);

    /** Called when the user navigates 'back' */
    virtual void onBackButtonClicked();
    /**@}*/


    /** @name Misc
     * @{
     */
    virtual void applySafeInsets(const EDGEINSETS& safeInsets);

    virtual bool navigateBack();

    virtual void requestScroll(float dx, float dy);
    /**@}*/

protected:
    sp<View> _view;
    
    // Navigation data
    string _title;
    sp<View> _titleView;
    sp<class LinearLayout> _leftButtonsFrame;
    sp<LinearLayout> _rightButtonsFrame;
    friend class NavigationBar;
    friend class NavigationController;
};

