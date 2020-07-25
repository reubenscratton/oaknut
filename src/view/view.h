//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
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
#define STATE_DISABLED  1   // Highest priority for state selection
#define STATE_ERRORED   2
#define STATE_PRESSED   4
#define STATE_CHECKED   8
#define STATE_SELECTED  16
#define STATE_FOCUSED   32
#define STATE_HOVER     64  // Lowest priority
struct VIEWSTATE {
    uint8_t mask;
    uint8_t state;
    VIEWSTATE(uint8_t amask, uint8_t astate) : mask(amask), state(astate) {
    }
    inline void setBits(uint8_t mask, uint8_t state) {
        this->mask |= mask;
        this->state |= (this->state & ~state) | state;
    }
    inline operator uint16_t() const {
        return *(uint16_t*)this;
    }
};



/**
@ingroup views
@brief Fundamental UI building block
@verbatim
* [Identity & Hierarchy](#identity--hierarchy)
* [Window & Surface](#window--surface)
* [Size & Position](#size--position)
* [Content & Scrolling](#content--scrolling)
* [Rendering](#rendering)
* [State](#state)
* [Styles](#styles)
* [Touch](#touch)
* [User Input](#user-input)
* [Animation](#animation)

#### Style reference

Name         | Type                   | Default
------------ | ---------------------- | -------
`align`      | alignspec[1 or 2]      |
`alignX`     | alignspec              |
`alignY`     | alignspec              |
`background` | drawable               |
`enabled`    | boolean                | true
`gravity`    | gravity[1 or 2]    |
`gravityX`   | gravity    |
`gravityY`   | gravity    |
`height`     | measurement            |
`id`         | text                   |
`padding`    | measurement[1,2,or 4]  |
`paddingX`   | measurement  |
`paddingY`   | measurement  |
`size`       | measurement[1 or 2]    |
`tint`       | color  |
`visibility` | `visible`&#124;`hidden`&#124;`gone`  | visible
`width`      | measurement            |

@endverbatim
*/
class View : public Styleable {
public:
    
    // These friend classes are tightly bound to View and are allowed to directly manipulate state.
    friend class MEASURESPEC;
    friend class Surface;
    friend class RenderOp;
    friend class RenderBatch;
    friend class PrivateSurfaceRenderOp;
    friend class Window;
    friend class ViewController;
    friend class ScrollInfo;
    friend class Animation;
    friend class LayoutAnimation;
    friend class LinearLayout;
    
    /**  @cond INTERNAL */
    View();
    virtual ~View();
    /**  @endcond */

    /** @name Identity & Hierarchy
     * @{
     */
    
    /** Adds a subview to the end of the subviews list, i.e. last in the z-order */
    virtual void addSubview(View* subview);

    /** Inserts a subview at a particular index, z-order ascending */
    virtual void insertSubview(View* subview, int index);

    /** Get the number of subviews. */
    virtual int getSubviewCount();

    /** Get a subview at a particular index, z-order ascending. Returns NULL if no such index. */
    virtual View* getSubview(int index);

    /** Removes a subview */
    virtual void removeSubview(View* subview);
    virtual void removeSubview(int index);
    virtual void removeAllSubviews();

    /** Return the parent view of this view */
    virtual View* getParent() const;
    
    /** Removes this view from its parent view */
    virtual void removeFromParent();
    
    /** Find the index of the given subview, returns -1 if given view is not a subview */
    virtual int indexOfSubview(View* subview);

    /** Find a view among the subviews (and all descendent views) given a string id */
    virtual View* findViewById(const string& id);
    
    /** Return the view at the root of the view tree */
    virtual View* getRootView();
    
    /** Walk the subviews in display (Z-) order. Return false to break the loop. */
    void iterateSubviews(std::function<bool(View*)> callback);



protected:
    
    /** Helper to remove any subviews currently scrolled out of sight */
    virtual void removeSubviewsNotInVisibleArea();
    
    /**  \cond INTERNAL */
    /** Identifies this view among it's peers in a layout file. See findViewById(). */
    string _id;
    
    /** The view that owns and contains this one. NB: Weak ref cos parent holds strong ref on this. */
    View* _parent;
    
    /**  List of child views, in back-to-front order */
    vector<sp<View>> _subviews;
    
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

    virtual void attachToSurface();
    virtual void detachFromSurface();
    
    /**  \cond INTERNAL */

    /** The Window that this view is attached to. In practice this is the main application window or NULL. */
    Window* _window;
    
    /** The rendering surface, i.e. where the view's RenderOps are drawn to */
    sp<Surface> _surface;
    
    /** This flag is true for the view that owns its surface. (Subviews will draw to it but they don't own it)*/
    bool _ownsSurface;
    
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

    /** Get the X-coordinate of the left edge, in parent coords */
    float getLeft() const;
    
    /** Get the X-coordinate of the right edge, in parent coords */
    float getRight() const;
    
    /** Get the Y-coordinate of the top edge, in parent coords */
    float getTop() const;

    /** Get the Y-coordinate of the bottom edge, in parent coords */
    float getBottom() const;

    /** Get the current view rect, in parent coords */
    virtual RECT getRect() const;
    
    /** Get the view rect in it's own coordinate system, i.e. (0,0,_rect.size.width,_rect.size.height) */
    virtual RECT getOwnRect();

    /** Get the visible view rect, i.e. own coordinate system but with content offset applied */
    virtual RECT getVisibleRect();
    
    /** Get the view rect in surface coordinates */
    virtual RECT getSurfaceRect();

    /** Same as getOwnRect() but with current padding insets applied */
    virtual RECT getOwnRectPadded();

    /** Convert a point from own-rect coordinates to window coordinates */
    virtual POINT mapPointToWindow(POINT pt);

    /** Set padding insets, i.e. the space between the view rect and its content */
    virtual void setPadding(EDGEINSETS padding);

    /** Set the size this view would like to have, given various constraints (see MEASURESPEC and layout()). */
    virtual void setLayoutSize(MEASURESPEC widthSpec, MEASURESPEC heightSpec);
    
    /** Set an absolute view rect, in pixels. This is the easiest way to position views in code. */
    virtual void setLayoutRect(const RECT& rect);

    /** Set an absolute view size, in pixels. */
    virtual void setLayoutSize(const SIZE& size);
    
    /** Set the preferred alignment within the parent view. See ALIGNSPEC and layout() **/
    virtual void setLayoutOrigin(ALIGNSPEC alignspecHorz, ALIGNSPEC alignspecVert);
    
    /** Signal that the view needs to recalculate its size and position */
    virtual void setNeedsLayout();

    /** Recalculate and apply this view's size and position (as well as all subviews). Default
        implementation uses SIZESPEC and ALIGNSPEC members but derived types may override the default behaviour. */
    virtual void layout(RECT constraint);

    /** Set this view's measured size to be that of its content size plus padding. */
    virtual void sizeToFit(float widthConstraint = FLT_MAX);
    

    /** Sets the position of the view. CAUTION! This API exists so Views may override the positioning
        of subviews during layout. Ideally this would not be a `public` API. */
    virtual void setRectOrigin(const POINT& origin);
    virtual void setRectSize(const SIZE& size);


    /** Set visibility. A view will only draw in its window when the following are all true:
        - The view is attached to a Window
        - The view's visibility is set to 'Visible'
        - The view is not clipped by an ancestor whose clipControl is 'Auto' or 'On'.
     */
    virtual void setVisibility(Visibility visibility);
    
    virtual bool isHidden() const;

protected:
    /**  \cond INTERNAL */

    virtual void layoutSubviews(RECT constraint);
    /** Determines the view rect size, given the parent size constraint. The default implementation
     uses the MEASURESPECs passed to setLayoutSize() to calculate the view rect size. */
    //virtual void measure(SIZE constrainingSize);
    

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
    float getAlignspecVal(const ALIGNSPEC& spec, bool isVertical);
    /**  \endcond */
    /**@}*/

    
    /** @name Content & Scrolling
     * @{
     *  When the view's content size exceeds the visible rect area then the view automatically becomes
     *  scrollable, i.e. displays scrollbars and handles movement events.
     */
public:
    /** Returns the current content size, as calculated during layout. The content
        size is not directly settable, each view should determine it's own.*/
    virtual SIZE getContentSize() const;

    /** Mark the view's _intrinsicSize as invalid and call setNeedsLayout() */
    virtual void invalidateIntrinsicSize();
    
    /** Called during measure() views should set the _intrinsicSize property here. The
        defaut implementation sets _intrinsicSize to zero. */
    virtual void updateIntrinsicSize(SIZE constrainingSize);
    
    /** Set the gravity flags. Gravity affects how the content is aligned within the view rect
        when the view rect size exceeds the content size. */
    virtual void setGravity(GRAVITY gravity);

    virtual POINT getContentOffset() const;
    virtual void setContentOffset(POINT contentOffset, bool animated=false);
    virtual void setScrollInsets(EDGEINSETS scrollInsets);
    virtual bool canScrollHorizontally();
    virtual bool canScrollVertically();
    virtual float getMaxScrollY();
    
    /** ClipContents determines whether a view's renderops (including those of
     subviews) are clipped to the visible portion of the view or not. Defaults
     to 'true', meaning clipping will be enabled when the view's measured
     contents exceeds its current bounds, and disabled otherwise.
     
     Note that subviews are treated as the view contents by default, i.e. a view that
     contains a scrollable area (e.g. a list of items) will automatically not waste
     time drawing subviews that haven't yet been scrolled into view.
     
     Clipping involves a per-fragment test at the end of the GPU pipeline and
     therefore is not free, however its usually far cheaper than rasterizing a ton
     of things that are currently off the screen. */
    virtual bool clipsContents() const;
    virtual void setClipsContents(bool clipContents);
    
    virtual void scrollBy(POINT scrollAmount);

protected:
    /**  \cond INTERNAL */
    SIZE _intrinsicSize;
    SIZE _contentSize;
    POINT _contentOffset, _contentOffsetAccum;
    GRAVITY _gravity;
    EDGEINSETS _scrollInsets;
    ScrollInfo _scrollVert;
    ScrollInfo _scrollHorz;
    virtual void updateScrollbarVisibility();
    virtual void updateScrollOffsets();
    void adjustContentOffsetAccum(const POINT& d);
    /** @} */
/**  \endcond */
/**@}*/



    /** @name Rendering
     * @{
     * Views render their content with RenderOps. You instantiate the RenderOps you need
     * and pass them to addRenderOp(). The background op is always drawn first and is
     * allowed to vary with state. The Surface the View is attached to will ensure the ops are
     * drawn in the correct order and at the correct location but will also attempt to
     * batch similar ops together to reduce drawing overhead.
     */
public:
    virtual void setBackground(RenderOp* renderOp);
    virtual RenderOp* getBackgroundOp() const { return _backgroundOp; }
    virtual void setBackgroundColor(COLOR color);
    virtual void setBackgroundAlpha(float alpha);
    virtual void setBackgroundStrokeColor(COLOR color);
    virtual void setBackgroundStrokeWidth(float strokeWidth);
    virtual void setBackgroundCornerRadii(const VECTOR4& cornerRadii);
    virtual void setBackgroundEdgeInsets(const EDGEINSETS& edgeInsets);
    virtual void setInkColor(COLOR inkColor);
    virtual void addRenderOp(RenderOp* renderOp);
    virtual void addRenderOp(RenderOp* renderOp, bool atFront);
    virtual void removeRenderOp(RenderOp* renderOp);
    virtual void setNeedsFullRedraw();
    virtual void invalidateRect(const RECT& rect);
    COLOR getTintColor();
    
    /** Sets tint color */
    virtual void setTintColor(COLOR tintColor);
    
    /** Get the current alpha. Defaults to 1.0. Note that the actual alpha value used in rendering
     inherits (i.e. is effectively multiplied by) all ancestor alpha values. */
    float getAlpha();
    
    /** Sets alpha */
    void setAlpha(float alpha);

    void setOpaque(bool opaque);
    
    /** Applies a matrix transform to the view during rendering, optionally resetting first */
    void applyMatrix(const MATRIX4& m, bool reset=true);
    inline void applyTranslate(float tx, float ty, bool reset=true) {
        applyMatrix(MATRIX4::makeTranslate(tx, ty), reset);
    }
    inline void applyScale(float sx, float sy, bool reset=true) {
        applyMatrix(MATRIX4::makeScale(sx, sy), reset);
    }

    void addDecorOp(RenderOp* renderOp);
    void removeDecorOp(RenderOp* renderOp);
    
    void setElevation(float elevation, bool animate=false);
    float getElevation() const;
    
protected:
    /**  \cond INTERNAL */
    virtual void updateRenderOps();
    virtual void updateEffectiveTint();
    virtual void onEffectiveTintColorChanged();
    virtual void updateBackgroundRect();
    COLOR _tintColor;
    COLOR _effectiveTintColor;
    float _alpha; // View's own alpha
    float _elevation; // controls drop-shadow as per Material Design
    float _effectiveAlpha;
    virtual void updateEffectiveAlpha();
    sp<RectRenderOp> _backgroundOp;
    InkRenderOp* _inkOp;
    sp<ShadowRenderOp> _shadowOp;
    MATRIX4* _matrix;
    void addRenderOpToList(RenderOp* renderOp, bool atFront, sp<RenderList>& list);
    void removeRenderOpFromList(RenderOp* renderOp, sp<RenderList>& list);
    void ensureInkOpExists();
    void ensureBackgroundOpExists();
    
    /** The RenderOps that draw this view */
    sp<RenderList> _renderList;
    sp<RenderList> _renderListDecor;
    
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
    virtual void setState(VIEWSTATE state);
    
    virtual bool isPressed() const;
    virtual void setPressed(bool isPressed);
    virtual bool isEnabled() const;
    virtual void setEnabled(bool enabled);
    virtual bool isChecked() const;
    virtual void setChecked(bool checked);
    virtual bool isSelected() const;
    virtual void setSelected(bool checked);
    virtual bool isFocused() const;
    virtual void setFocused(bool focused);
    virtual bool isErrored() const;
    virtual void setErrored(bool errored);


protected:
    /** Called whenever state changes.  */
     virtual void onStateChanged(VIEWSTATE changes);

    virtual void setSelectedSubview(View* subview);

    /**  \cond INTERNAL */
    uint8_t _state;
    /**  \endcond */
    /**@}*/

    
    void inflate(const string& layoutFile);
    template<class T>
    void bind(T*& rview, const string& id) {
        rview = (T*)findViewById(id);
    }

    /** @name Styles
     * @{
     * A view may be declaratively configured in layout and style resource files.
     */
protected:
    /** Applies a single style value for the given attribute name. Custom views
        should override this method to add support for custom attributes. */
    bool applySingleStyle(const string& name, const style& value) override;
    const style* resolveQualifiedStyle(const style* val) override;
    
    /**  \cond INTERNAL */
    void processSizeStyleValue(const style& sizeValue, MEASURESPEC* widthspec, MEASURESPEC* heightspec);
    void processAlignStyleValue(const style& alignValue, ALIGNSPEC* horzspec, ALIGNSPEC* vertspec);
    void processGravityStyleValue(const style& gravityValue, bool horz, bool vert);
    /**  \endcond */
    /**@}*/


    /** @name Touch
     * @{
     */
public:
    std::function<bool(View*,INPUTEVENT*)> onInputEvent;
    std::function<void()> onClick;
    virtual View* hitTest(POINT& pt);
    virtual View* subviewContainingPoint(POINT pt);
    virtual int indexOfSubviewContainingPoint(POINT pt);
    virtual void setDirectionalLockEnabled(bool enabled);
    bool getDirectionalLockEnabled() const;
protected:
    /**  \cond INTERNAL */
    virtual View* dispatchInputEvent(INPUTEVENT* event);
    virtual bool handleInputEvent(INPUTEVENT* event);
	
    POINT _ptDrag;
    /**  \endcond */
    /**@}*/

    // Flags
    struct {
        bool _layoutValid:1;
        bool _needsFullRedraw:1;
        bool _intrinsicSizeValid:1;
        bool _updateRenderOpsNeeded:1;
        bool _clipsContents:1;
        bool _directionalLockEnabled:1;
        bool _subviewsInheritState:1;
        bool _opaque:1;
        bool _selectableSubviews:1;
        bool _hideScrollbars:1;
    };

    
public:

    /** @name User Input
     * @{
     */
    virtual IKeyboardInputHandler* getKeyboardInputHandler();
    virtual ITextInputReceiver* getTextInputReceiver();
    virtual bool requestFocus();
    /**@}*/


    /** @name Animation
     * @{
     */
    virtual void animateAlpha(float target, float duration);
    virtual void animateTranslate(POINT translation, float duration);
    virtual Animation* animateInFromBottom(float duration, InterpolateFunc interpolater = Animation::strongEaseOut);
    virtual Animation* animateOutToBottom(float duration, InterpolateFunc interpolater = Animation::strongEaseOut);
    int _animationCount; // number of active animations currently attached to the view
    /**@}*/

#ifdef DEBUG
    string debugDescription() override;
    virtual string debugViewType();
    void debugDumpTree(int depth);
    string _debugTag;
#endif

};



