//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

// Visibility
enum Visibility {
    Visible,
    Invisible,
    Gone
};


// State
#define STATE_DISABLED 1
#define STATE_FOCUSED  2
#define STATE_SELECTED 4
#define STATE_CHECKED  8
#define STATE_PRESSED 16
typedef uint16_t STATE;
typedef struct {
    STATE mask;
    STATE state;
} STATESET;


/**
 * \class View
 * \brief Base class for all views
 */
class View : public Object {
public:
    
    // These friend classes are tightly bound to View and are allowed to directly manipulate state.
    friend class Surface;
    friend class RenderOp;
    friend class RenderBatch;
    friend class PrivateSurfaceRenderOp;
    friend class Window;
    friend class ViewController;
    friend class Scrollbar;
    
    // Construction
    View();
    virtual ~View();

    /** @name Identity & Hierarchy
     * @{
     */
    
    /** Adds a subview to the end of the subviews list, i.e. last in the z-order */
    virtual void addSubview(View* subview);

    /** Inserts a subview at a particular index, z-order ascending */
    virtual void insertSubview(View* subview, int index);

    /** Removes a subview */
    virtual void removeSubview(View* subview);

    /** Return the parent view of this view */
    virtual View* getParent() const;
    
    /** Removes this view from its parent view */
    virtual void removeFromParent();
    
    /** Find the index of the given subview, returns -1 if given view is not a subview */
    virtual int indexOfSubview(View* subview);

    /** Find a view among the subviews (and all descendent views) given a string id */
    virtual View* findViewById(const string& id);


protected:
    
    /** Helper to remove any subviews currently scrolled out of sight */
    virtual void removeSubviewsNotInVisibleArea();
    
    /**  \cond INTERNAL */
    /** Identifies this view among it's peers in a layout file. See findViewById(). */
    string _id;
    
    /** The view that owns and contains this one. NB: Weak ref cos parent holds strong ref on this. */
    View* _parent;
    
    /**  List of child views, in back-to-front order */
    vector<ObjPtr<View>> _subviews;
    
    /**  \endcond */
    /**@}*/
    
    
    /** @name Window & Surface
     * @{
     */
public:
    /** Controls whether the view should render to a private surface or not. A private surface is useful
        when rendering is particularly expensive and likely to contribute to lag. */
    virtual void setUsePrivateSurface(bool usePrivateSurface);
    
    /** Return the Window that this View is attached to. In practice this is the main application window or NULL. */
    virtual Window* getWindow() const;

protected:
    /** Called when the view is attached to the application Window and will be drawn
        and able to receive input events, etc. */
    virtual void attachToWindow(Window* window);

    /** Called when the view is detached from the application Window */
    virtual void detachFromWindow();

    /**  \cond INTERNAL */
    /** A private helper for updating internal state when the private surface flag changes */
    virtual void updatePrivateSurface(bool updateSubviews);

    /** The Window that this view is attached to. In practice this is the main application window or NULL. */
    Window* _window;
    
    /** The rendering surface, i.e. where the view's RenderOps are drawn to */
    ObjPtr<Surface> _surface;
    
    /** This flag is true for the view that owns its private surface. (Subviews will draw to it but they don't own it)*/
    bool _ownsPrivateSurface;
    
    /** The offset from the top-left of this view to the top left of the surface.
     Set in onWindowAttach() and updated when self or ancestor rect origin changes. */
    POINT _surfaceOrigin;
    /**  \endcond */
    /**@}*/

    
    /** @name Size & Position
     * @{
     */
public:

    /** Get the width of the view */
    float getWidth() const;
    
    /** Get the height of the view */
    float getHeight() const;

    /** Get the current view rect, in parent coords */
    virtual RECT getRect() const;
    
    /** Get the view rect in it's own coordinate system, i.e. (0,0,_rect.size.width,_rect.size.height) */
    virtual RECT getOwnRect();
    
    /** Same as getOwnRect() but with current padding insets applied */
    virtual RECT getOwnRectPadded();

    /** Convert a point from own-rect coordinates to window coordinates */
    virtual POINT mapPointToWindow(POINT pt);

    /** Set padding insets, i.e. the space between the view rect and its content */
    virtual void setPadding(EDGEINSETS padding);
    
    /** Set the size this view would like to have, given various constraints (see MEASURESPEC and measure()). */
    virtual void setMeasureSpecs(MEASURESPEC widthMeasureSpec, MEASURESPEC heightMeasureSpec);
    
    /** Set the preferred alignment within the parent view. See ALIGNSPEC and layout() **/
    virtual void setAlignSpecs(ALIGNSPEC alignspecHorz, ALIGNSPEC alignspecVert);
    
    /** Signal that the view needs to recalculate its size and position */
    virtual void setNeedsLayout();
    
    /** Determines the view rect size, given the parent size constraint. The default implementation
        uses the MEASURESPECs passed to setMeasureSpecs() to calculate the view rect size. */
    virtual void measure(float parentWidth, float parentHeight);

    /** Sets the size of the view rect. CAUTION! This API exists so Views may override the size of
        their subviews during layout. Ideally it would not be a `public` API. */
    virtual void setRectSize(const SIZE& size);

    /** Sets the position of the view. CAUTION! This API exists so Views may override the positioning
        of subviews during layout. Ideally this would not be a `public` API. */
    virtual void setRectOrigin(const POINT& origin);

    /** Updates the view position, i.e. view rect origin. The default implementation uses the
        ALIGNSPECs passed to setAlignSpecs() to set the view position */
    virtual void layout();

    virtual void setVisibility(Visibility visibility);

protected:
    /**  \cond INTERNAL */

    /** The visible area covered by the view, in parent view coords */
    RECT _rect;
    
    // TODO: internal docs
    MEASURESPEC _widthMeasureSpec;
    MEASURESPEC _heightMeasureSpec;
    ALIGNSPEC _alignspecHorz;
    ALIGNSPEC _alignspecVert;
    EDGEINSETS _padding;
    Visibility _visibility;
    void adjustRectSize(const SIZE& d);
    void adjustSurfaceOrigin(const POINT& d);
    /**  \endcond */
    /**@}*/

    
    /** @name Content & Scrolling
     * @{
     *  When the view's content size exceeds the visible rect area then the view automatically becomes
     *  scrollable, i.e. displays scrollbars and handles movement events.
     */
public:
    /** Returns the current content size, as calculated by updateContentSize() during layout. The content
        size is not directly settable, each view should determine it's own.*/
    virtual SIZE getContentSize() const;

    /** Mark the view's contentSize as invalid and call setNeedsLayout() */
    virtual void invalidateContentSize();
    
    /** Called during measure() views should set the _contentSize property here. The
        defaut implementation does nothing. */
    virtual void updateContentSize(float parentWidth, float parentHeight);
    
    /** Set the gravity flags. Gravity affects how the content is aligned within the view rect
        when the view rect size exceeds the content size. */
    virtual void setGravity(GRAVITY gravity);

    virtual POINT getContentOffset() const;
    virtual void setContentOffset(POINT contentOffset);
    virtual void setScrollInsets(EDGEINSETS scrollInsets);


protected:
    /**  \cond INTERNAL */
    SIZE _contentSize;
    POINT _contentOffset;
    bool _contentSizeValid;
    GRAVITY _gravity;
    EDGEINSETS _scrollInsets;
    ObjPtr<Scrollbar> _scrollbarVert;
    ObjPtr<Scrollbar> _scrollbarHorz;
    float _scrollAlpha;
    ObjPtr<class DelegateAnimation> _scrollFadeAnim;
    ObjPtr<Timer> _scrollFadeTimer;
    virtual void scrollStartFadeAnim(float targetAlpha);
    virtual void updateScrollbars();
    /** @} */
/**  \endcond */
/**@}*/



    /** @name Drawing
     * @{
     * Views draw themselves through RenderOps. You instantiate the RenderOps you need
     * and pass them to addRenderOp(). The background op is always drawn first and is
     * allowed to vary with state. The Surface the View is attached to will ensure the ops are
     * drawn in the correct order and at the correct location but will also attempt to
     * batch similar ops together to reduce drawing overhead.
     */
public:
    virtual void setBackground(RenderOp* drawableOp);
    virtual void setBackground(RenderOp* drawableOp, STATESET stateset);
    virtual void setBackgroundColour(COLOUR colour);
    virtual void addRenderOp(RenderOp* renderOp);
    virtual void addRenderOp(RenderOp* renderOp, bool atFront);
    virtual void removeRenderOp(RenderOp* renderOp);
    virtual void setNeedsFullRedraw();
    virtual void invalidateRect(const RECT& rect);
    COLOUR getTintColour();
    
    /** Sets tint colour */
    virtual void setTintColour(COLOUR tintColour);
    
    /** Get the current alpha. Defaults to 1.0. Note that the actual alpha value used in rendering
     inherits (i.e. is effectively multiplied by) all ancestor alpha values. */
    float getAlpha();
    
    /** Sets alpha */
    void setAlpha(float alpha);

protected:
    /**  \cond INTERNAL */
    virtual void updateBackgroundOp();
    virtual void updateRenderOps();
    virtual void updateEffectiveTint();
    virtual void onEffectiveTintColourChanged();
    COLOUR _tintColour;
    COLOUR _effectiveTintColour;
    float _alpha; // View's own alpha
    float _effectiveAlpha;
    virtual void updateEffectiveAlpha();
    ObjPtr<RenderOp> _currentBackgroundOp;
    vector<pair<ObjPtr<RenderOp>,STATESET>> _backgroundOps;
    bool _needsFullRedraw;
    bool _updateRenderOpsNeeded;
    bool _opaque;
    Matrix4* _matrix;

    /** Links to the adjacent views in the render order. All views attached to the window
        form a doubly-linked list used in rendering. */
    ObjPtr<View> _previousView;
    ObjPtr<View> _nextView;
    
    /** The RenderOps that draw this view */
    list<ObjPtr<RenderOp>> _renderList;
    
    /**  \endcond */
    /**@}*/

    
    /** @name State
     * @{
     * Each view has 16 bits of 'state'. The low 5 bits are reserved for the standard bit flags: enabled,
     * focused, selected, and so on, the remaining bits are free for use.
     */
public:
    
    /** Set one or more state bits at once. The STATESET parameter has a 'mask' member which determines
        the state bits that get updated. */
    virtual void setState(STATESET stateset);
    
    virtual bool isPressed();
    virtual void setPressed(bool isPressed);
    virtual bool isEnabled();
    virtual void setEnabled(bool enabled);

protected:
    /** Called whenever state changes.  */
     virtual void onStateChanged(STATESET changes);

    /**  \cond INTERNAL */
    STATE _state;
    /**  \endcond */
    /**@}*/

    
    /** @name Styles
     * @{
     * A view may be declaratively configured in layout and style resource files.
     */
public:
    /** Applies a set of styles to the view */
    virtual void applyStyleValues(const StyleValueList& values);

protected:
    /** Applies a single style value for the given attribute name. Custom views
        should override this method to add support for custom attributes. */
    virtual bool applyStyleValue(const string& name, StyleValue* value);
    
    /**  \cond INTERNAL */
    virtual bool applyStyleValueFromChild(const string& name, StyleValue* value, View* subview);
    /**  \endcond */
    /**@}*/


    /** @name Touch
     * @{
     */
public:
    /** Convenience function returning true if the view is both visible and enabled */
    virtual bool isTouchable();
    std::function<bool(View*,int,int,POINT)> onTouchEventDelegate;
    virtual View* hitTest(POINT pt, POINT* ptRel);
    virtual View* subviewContainingPoint(POINT pt);
    virtual int indexOfSubviewContainingPoint(POINT pt);
protected:
    /**  \cond INTERNAL */
    virtual View* dispatchInputEvent(int eventType, int finger, long time, POINT pt);
    virtual bool onTouchEvent(int eventType, int finger, POINT pt);
	
    bool _isDragging;
    POINT _ptDrag;
    /**  \endcond */
    /**@}*/

    
public:

    // Keyboard input
    virtual IKeyboardInputHandler* getKeyboardInputHandler();
    virtual bool setFocused(bool focused);
    virtual bool isFocused();

	// Animation
	virtual void animateAlpha(float target, float duration);
	virtual void setTranslate(POINT translation);
    int _animationCount; // number of active animations currently attached to the view
    
#ifdef DEBUG
    virtual string debugDescription();
    virtual string debugViewType();
    void debugDumpTree(int depth);
#endif

};



