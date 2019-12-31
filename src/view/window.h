//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


enum Permission {
    PermissionCamera,
    PermissionMic,
};

/** @class Window
 *  @brief A window is the top level container for app UI. There is usually only one Window instance, globally
   accessible through `app->window`. It usually wraps a native window in some way.
 *  @ingroup views
 */

class Window : public View {
protected:
    Window();
public:
    static Window* create();

    sp<class ViewController> _rootViewController;
	vector<sp<ViewController>> _viewControllers;
    class MotionTracker {
    public:
        MotionTracker(int source);
        void dispatchInputEvent(INPUTEVENT& event, ViewController* topVC);

        int source;
        sp<View>  touchedView;
        TIMESTAMP timeOfDownEvent;
        POINT ptDown;
        POINT ptPrev;
        int numClicks;
        bool isDragging;
        bool _dragIsVertical;
        POINT pastPts[NUM_PAST];
        TIMESTAMP pastTime[NUM_PAST];
        int pastIndex, pastCount;
        sp<Timer> multiclickTimer;
        sp<Timer> _longpressTimer;
        bool _didSendLongpressEvent;
    };

    // Animations
    list<sp<class Animation>> _animations;
    void startAnimation(Animation* animation, int duration);
    void startAnimation(Animation* animation, int duration, int delay);
    void stopAnimation(Animation* animation);
    void detachView(View* view);
    bool _animationsModified;
    
    // Permissions
	virtual bool hasPermission(Permission permission);
	virtual void runWithPermission(Permission permission, std::function<void(bool)> callback);
    virtual void runWithPermissions(vector<Permission> permission, std::function<void(vector<bool>)> callback);

    // Show modal VCs
    virtual void presentModalViewController(ViewController* viewController);
    virtual void dismissModalViewController(ViewController* viewController, std::function<void()> onComplete);

    vector<MotionTracker*> _motionTrackers;
	bool _redrawNeeded;
    View* _focusedView;
    class IKeyboardInputHandler* _keyboardHandler;
    class ITextInputReceiver* _textInputReceiver;
	
	virtual void setRootViewController(ViewController* viewController);
	virtual void resizeSurface(int width, int height);
	virtual void destroySurface();
	virtual void draw();
	virtual void requestRedraw();
    virtual void requestRedrawNative();
    virtual void show();
    
    /** Show or hide the system soft keyboard, if there is one */
    virtual void keyboardShow(bool show);
    virtual void keyboardNotifyTextChanged();    
    virtual void keyboardNotifyTextSelectionChanged();

	virtual void dispatchInputEvent(INPUTEVENT event);
	virtual POINT offsetToView(View* view);
    
    virtual bool setFocusedView(View* view);
	void attachViewController(ViewController* vc);
    void detachViewController(ViewController* vc);
    

	// Render state
    Renderer* _renderer;

    COLOR _backgroundColor;
    
    void layout(RECT constraint) override;
    
    EDGEINSETS _systemSafeInsets;
    EDGEINSETS _softKeyboardInsets;
    void setSafeInsets(const EDGEINSETS& insets);
    void setSoftKeyboardInsets(const EDGEINSETS& insets);
    EDGEINSETS getSafeInsets() const;
    void applySafeInsetsChange(bool updateFocusedView);
    void ensureFocusedViewIsInSafeArea();
    
    RectRenderOp* _renderOpDecorTop;
	RectRenderOp* _renderOpDecorBottom;
	void updateDecorOp(bool bottom, float height);
};


