//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#define INPUT_SOURCE_TYPE_KEY 0
#define INPUT_SOURCE_TYPE_MOUSE 1
#define INPUT_SOURCE_TYPE_FINGER 2
#define INPUT_SOURCE_TYPE_SCROLLER 3

#define MAKE_SOURCE(type, id) ((type<<8)|id)
#define SOURCE_TYPE(source) (source>>8)
#define SOURCE_ID(source) (source&255)

#define INPUT_EVENT_DOWN 0
#define INPUT_EVENT_MOVE 1
#define INPUT_EVENT_UP   2
#define INPUT_EVENT_CANCEL 3
#define INPUT_EVENT_DRAG 4
#define INPUT_EVENT_TAP 6
#define INPUT_EVENT_TAP_CONFIRMED 7
#define INPUT_EVENT_FLING 8
#define INPUT_EVENT_LONG_PRESS 9

#define NUM_PAST 10

// TODO: these constants should be in platform styles
#define TOUCH_SLOP 10 // DPs
#define MULTI_CLICK_THRESHOLD 400 // ms
#define LONG_PRESS_THRESHOLD 800 // ms

typedef struct INPUTEVENT {
    int type;
    enum {
        Mouse,
        Touch,
        ScrollWheel //
    } deviceType;
    int deviceIndex; 
    TIMESTAMP time;
    POINT pt;
    POINT delta; // specific to ScrollWheel
    POINT velocity; // specific to flings
} INPUTEVENT;



/** @class Window
 *  @brief A window is the top level container for app UI. There is usually only one Window instance, accessible through `app.window`.
 *  @ingroup views
 */
class Window : public Object {
public:
	
	ObjPtr<class ViewController> _rootViewController;
	vector<ObjPtr<ViewController>> _viewControllers;
    ObjPtr<Surface> _surface;
    RECT _surfaceRect;
	float _scale;
	bool _inLayoutPass;
    class MotionTracker {
    public:
        MotionTracker(int source);
        void dispatchInputEvent(INPUTEVENT& event, Window* window);

        int source;
        ObjPtr<class View>  touchedView;
        TIMESTAMP timeOfDownEvent;
        POINT ptDown;
        int numClicks;
        bool isDragging;
        float dragDirection;
        POINT pastPts[NUM_PAST];
        TIMESTAMP pastTime[NUM_PAST];
        int pastIndex, pastCount;
        ObjPtr<Timer> multiclickTimer;
        ObjPtr<Timer> _longpressTimer;
        bool _didSendLongpressEvent;
    };

    // Animations
    list<ObjPtr<class Animation>> _animations;
    void startAnimation(Animation* animation, int duration);
    void startAnimation(Animation* animation, int duration, int delay);
    void stopAnimation(Animation* animation);
    void detachView(View* view);
    bool _animationsModified;
    

    vector<MotionTracker*> _motionTrackers;
	bool _viewLayoutValid;
	bool _redrawNeeded;
    View* _focusedView;
    class IKeyboardInputHandler* _keyboardHandler;
    class ITextInputReceiver* _textInputReceiver;
	
	Window();
	virtual void setRootViewController(ViewController* viewController);
	virtual void setNeedsLayout();
	virtual void resizeSurface(int width, int height, float scale);
	virtual void destroySurface();
	virtual void draw();
	virtual void requestRedraw();
	virtual void dispatchInputEvent(INPUTEVENT event);
	virtual POINT offsetToView(View* view);
    
    virtual bool setFocusedView(View* view);
	void attachRootVC();

    void pushClip(RECT clip);
    void popClip();
    
	// Render state
	int _doneGlInit;
    GLfloat _backgroundColor[4];
    vector<GLProgram*> _loadedProgs;
    list<BitmapBase*> _loadedTextures;
	bool _rootVcAttached;
	QuadBuffer* _quadBuffer;
    stack<RECT> _clips;
	void setBlendMode(int blendMode);
	void bindTexture(Bitmap* texture);
	GLuint _currentProg;
	Surface* _currentSurface;
	Bitmap* _currentTexture;
	int _renderCounter;
	int _currentVertexConfig;
	int _blendMode;
	void prepareToDraw();
	void setCurrentSurface(Surface* surface);
	void setVertexConfig(int vertexConfig);
    
    EDGEINSETS _safeAreaInsets; // not including keyboard
    RECT _softKeyboardRect;
    void setSoftKeyboardRect(const RECT rect);
    void updateSafeArea();
};


