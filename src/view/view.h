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


// Gravity
#define GRAVITY_LEFT 0
#define GRAVITY_RIGHT 1
#define GRAVITY_CENTER 2
#define GRAVITY_TOP 0
#define GRAVITY_BOTTOM 1
typedef struct {
    uint8_t horz:4;
    uint8_t vert:4;
} GRAVITY;



//
// View measurements.
//
typedef struct _MEASURESPEC {
    int refType;
    class View* refView;
    float refSizeMultiplier;
    float abs;
} MEASURESPEC;
#define REFTYPE_NONE      0  // no measuring is done, frame must be set in code
#define REFTYPE_ABS       1  // measurement is absolute (i.e. abs field)
#define REFTYPE_CONTENT   2  // measurement is taken from intrinsic content size, plus padding
#define REFTYPE_VIEW      3  // measure relative to another view (normally the parent)
#define REFTYPE_ASPECT    4  // measure relative to opposite dimension

MEASURESPEC MEASURESPEC_Make(int refType, View* refView, float refSizeMultiplier, float delta);
#define MEASURESPEC_None		 MEASURESPEC_Make(REFTYPE_NONE, NULL, 0, 0)
#define MEASURESPEC_Abs(x)       MEASURESPEC_Make(REFTYPE_ABS, NULL, 0, x)
#define MEASURESPEC_WrapContent  MEASURESPEC_Make(REFTYPE_CONTENT, NULL, 1, 0)
#define MEASURESPEC_UseAspect(x) MEASURESPEC_Make(REFTYPE_ASPECT, NULL,  x, 0)
#define MEASURESPEC_FillParent   MEASURESPEC_Make(REFTYPE_VIEW, NULL, 1, 0)


//
// View alignment/positioning
//
// View alignment is always relative to an anchor, which defaults to the parent view or (for root views) the screen.
//
// View alignment along a single axis involves two multipliers and a margin. The first multiplier is
// that of the anchor's width, the second is that of one's own width, and the margin is a delta in pixels.
//
typedef struct _ALIGNSPEC {
    View* anchor;
    float multiplierAnchor;
    float multiplierSelf;
	float margin;
} ALIGNSPEC;

ALIGNSPEC ALIGNSPEC_Make(View* anchor, float multiplierAnchor, float multiplierOwn, float margin);

#define ALIGNSPEC_None		 ALIGNSPEC_Make(NO_ANCHOR, 0,0,0)
#define ALIGNSPEC_Left       ALIGNSPEC_Make(NULL, 0.0f, 0.0f, 0)
#define ALIGNSPEC_Center     ALIGNSPEC_Make(NULL, 0.5f,-0.5f, 0)
#define ALIGNSPEC_Right      ALIGNSPEC_Make(NULL, 1.0f,-1.0f, 0)
#define ALIGNSPEC_Top        ALIGNSPEC_Make(NULL, 0.0f, 0.0f, 0)
#define ALIGNSPEC_Bottom     ALIGNSPEC_Make(NULL, 1.0f,-1.0f, 0)
#define ALIGNSPEC_ToLeftOf(view, margin)   ALIGNSPEC_Make(view, 0.0f, -1.0f, margin)
#define ALIGNSPEC_ToRightOf(view, margin)  ALIGNSPEC_Make(view, 1.0f,  0.0f, margin)
#define ALIGNSPEC_Above(view, margin)	   ALIGNSPEC_Make(view, 0.0f, -1.0f, margin)
#define ALIGNSPEC_Below(view, margin)	   ALIGNSPEC_Make(view, 1.0f,  0.0f, margin)

class IKeyboardInputHandler {
public:
    virtual void insertText(string text, int replaceStart, int replaceEnd) = 0;
    virtual void deleteBackward() = 0;
    virtual int getTextLength() = 0;
    virtual int getSelectionStart() = 0;
    virtual int getInsertionPoint() = 0;    // NB: insertion point is also the end of selection
    virtual void setSelectedRange(int start, int end) = 0;
    virtual string textInRange(int start, int end) = 0;
};

class View : public Object {
public:
    string _id;
	Window* _window;
	View* _parent;
	vector<ObjPtr<View>> _subviews; // in back-to-front order
	int tag;
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
    
    
	// Size and location
	RECT _frame; // in parent view coords. "Bounds" rect is {_contentOffset, _frame.size}.
    POINT _surfaceOrigin; // offset of top-left corner to surface-origin. Set in onWindowAttach()
                         // and updated when self or ancestor frame origin changes.
	MEASURESPEC _widthMeasureSpec;
	MEASURESPEC _heightMeasureSpec;
	ALIGNSPEC _alignspecHorz;
	ALIGNSPEC _alignspecVert;
	void setFrameOrigin(const POINT& pt);
    void adjustSurfaceOrigin(const POINT& d);

	// Content
	SIZE _contentSize;
	POINT _contentOffset;
	EDGEINSETS _padding;
	bool _contentSizeValid;
    GRAVITY _gravity;
    
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


extern map<string, View* (*)()>* s_classRegister;

template<typename T>
class ClassRegistrar {
private: static View* createT() {return new T(); }
public:
    ClassRegistrar(const string& className) {
        if (!s_classRegister) {
            s_classRegister = new map<string, View*(*)()>();
        }
        s_classRegister->insert(std::make_pair(className, &createT));
    }
};

#define DECLARE_DYNCREATE(X) static ClassRegistrar<X> s_classReg##X(#X)
View* DYNCREATE(const string& className);

