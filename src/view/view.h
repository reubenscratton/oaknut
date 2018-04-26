//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

// Visibility
#define VISIBILITY_VISIBLE   0
#define VISIBILITY_INVISIBLE 1
#define VISIBILITY_GONE 2


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
 * \brief Base class for all views.
 */
class View : public Object {
public:
    
    /** @name Identity & Hierarchy
     * @{
     */
    string _id; /// Identifies this view among it's peers in a layout file. See findViewById().
	Window* _window; /// The Window that this view is attached to. In practice this is the main application window or NULL.
    View* _parent; /// The view that owns and contains this one. NB: Weak ref cos parent holds strong ref on this.
	vector<ObjPtr<View>> _subviews; /// List of child views, in back-to-front order
    /**@}*/
    
    /** @name Size & Location
     * @{
     */
    RECT _frame; /// The visible area of the view, in parent view coords. "Bounds" rect is {_contentOffset, _frame.size}.
    POINT _surfaceOrigin; /// The offset from the top-left of this view to the top left of the surface. Set in onWindowAttach() and updated when self or ancestor frame origin changes.
    MEASURESPEC _widthMeasureSpec;
    MEASURESPEC _heightMeasureSpec;
    ALIGNSPEC _alignspecHorz;
    ALIGNSPEC _alignspecVert;
    void setFrameOrigin(const POINT& pt);
    void adjustSurfaceOrigin(const POINT& d);
    /**@}*/

    /** @name Content Size
     * @{
     */
    SIZE _contentSize;
    POINT _contentOffset;
    EDGEINSETS _padding;
    bool _contentSizeValid;
    GRAVITY _gravity;
    /**@}*/

	STATE _state;
    int _visibility;
	bool _touchEnabled;
    ObjPtr<RenderOp> _currentBackgroundOp;
    vector<pair<ObjPtr<RenderOp>,STATESET>> _backgroundOps;
	bool _opaque;
	ObjPtr<class AlphaAnimation> _alphaAnim;
    
    // Render ordering
    ObjPtr<View> _previousView;
    ObjPtr<View> _nextView;
    
    // The RenderOps that draw this view
    list<ObjPtr<RenderOp>> _renderList;

    // Render surface, i.e. where the view draws itself to
    ObjPtr<Surface> _surface;
    bool _ownsPrivateSurface;
    virtual void setUsePrivateSurface(bool usePrivateSurface);
    virtual void updatePrivateSurface(bool updateSubviews);

    bool _needsFullRedraw;
    bool _updateRenderOpsNeeded;
    virtual void updateRenderOps();
    
    
    
	// Scrolling
    bool _isDragging;
    POINT _ptDrag;
    EDGEINSETS _scrollInsets;
    ObjPtr<Scrollbar> _scrollbarVert;
    ObjPtr<Scrollbar> _scrollbarHorz;
    float _scrollAlpha;
    ObjPtr<class DelegateAnimation> _scrollFadeAnim;
    ObjPtr<Timer> _scrollFadeTimer;
    virtual void scrollStartFadeAnim(float targetAlpha);
    virtual void setContentOffset(POINT contentOffset);
    virtual void setScrollInsets(EDGEINSETS scrollInsets);
    virtual void updateScrollbars();

	// Touch
    std::function<bool(View*,int,int,POINT)> onTouchEventDelegate;
	
    Matrix4* _matrix;

public:
    float getAlpha() {
        return _alpha;
    }
    void setAlpha(float alpha);
    
protected:
    // Cascading properties. Setting these affects the effective properties of all subviews.
    float _alpha; // View's own alpha
    COLOUR _tintColour;
    
    // Effective properties which are affected by corresponding properties of ancestors in the view tree
    float _effectiveAlpha;
    COLOUR _effectiveTintColour;
    
    virtual void updateEffectiveAlpha();
    virtual void updateEffectiveTint();
    virtual void onEffectiveTintColourChanged();
    
public:
    // Construction
    View();
	virtual ~View();
	
	// Size, position
	//virtual void setFrameSize(SIZE frameSize);
	virtual RECT getBounds();
    virtual RECT getBoundsWithPadding();
	virtual void setPadding(EDGEINSETS padding);
	virtual void setMeasureSpecs(MEASURESPEC widthMeasureSpec, MEASURESPEC heightMeasureSpec);
	virtual void setAlignSpecs(ALIGNSPEC alignspecHorz, ALIGNSPEC alignspecVert);
	virtual void setNeedsLayout();
	virtual void measure(float parentWidth, float parentHeight);
	virtual void layout();
	virtual void invalidateContentSize();
	virtual void updateContentSize(float parentWidth, float parentHeight);
    virtual void setGravity(GRAVITY gravity);
	virtual POINT mapPointToWindow(POINT pt);
	virtual View* subviewContainingPoint(POINT pt);
	virtual int indexOfSubviewContainingPoint(POINT pt);
    virtual void setVisibility(int visibility);

	// Window attachment
	virtual void attachToWindow(Window* window);
	virtual void detachFromWindow();
	
	// Background
	virtual void setBackground(RenderOp* drawableOp);
    virtual void setBackground(RenderOp* drawableOp, STATESET stateset);
	virtual void setBackgroundColour(COLOUR colour);
    virtual void updateBackgroundOp();
	
	// Subviews
	virtual void addSubview(View* subview);
	virtual void insertSubview(View* subview, int index);
	virtual void removeSubview(View* subview);
	virtual void removeFromParent();
	virtual int indexOfSubview(View* subview);
    virtual void removeSubviewsNotInVisibleArea();
    virtual View* findViewById(const string& id);

	
    // Drawing
    virtual void addRenderOp(RenderOp* renderOp);
    virtual void addRenderOp(RenderOp* renderOp, bool atFront);
    virtual void removeRenderOp(RenderOp* renderOp);
	virtual void setTintColour(COLOUR tintColour);
    virtual void setNeedsFullRedraw();
	virtual void invalidateRect(const RECT& rect);

    // State
    virtual void setState(STATESET stateset);
    virtual void onStateChanged(STATESET changes);
    virtual bool isPressed();
    virtual void setPressed(bool isPressed);
    virtual void setEnabled(bool enabled);
	virtual bool isTouchable();

    // Touch
	virtual View* hitTest(POINT pt, POINT* ptRel);
	virtual View* dispatchInputEvent(int eventType, int finger, long time, POINT pt);
	virtual bool onTouchEvent(int eventType, int finger, POINT pt);
    
    // Keyboard input
    virtual IKeyboardInputHandler* getKeyboardInputHandler();
    virtual bool becomeFirstResponder();
    virtual bool isFirstResponder();
    virtual void resignFirstResponder();
    
	// Animation helpers
	virtual void animateAlpha(float target, float duration);
	virtual void setAnimTranslate(POINT translation);
    
    // Styles
    virtual void applyStyleValues(const StyleValueList& values);
    virtual bool applyStyleValue(const string& name, StyleValue* value);
    virtual bool applyStyleValueFromChild(const string& name, StyleValue* value, View* subview);

#ifdef DEBUG
    virtual string debugDescription();
    virtual string debugViewType();
    void debugDumpTree(int depth);
#endif

};



