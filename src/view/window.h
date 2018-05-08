//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#define INPUT_SOURCE_TYPE_KEY 0
#define INPUT_SOURCE_TYPE_MOUSE 1
#define INPUT_SOURCE_TYPE_FINGER 2

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

#define NUM_PAST 10

// TODO: these constants should be in platform styles
#define TOUCH_SLOP 10 // DPs
#define MULTI_CLICK_THRESHOLD 400 // ms



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
        void dispatchInputEvent(int event, long time, POINT pt, Window* window);

        int source;
        ObjPtr<class View>  touchedView;
        long timeOfDownEvent;
        POINT ptDown;
        int numClicks;
        bool isDragging;
        float dragDirection;
        POINT pastPts[NUM_PAST];
        long pastTime[NUM_PAST];
        int pastIndex, pastCount;
        ObjPtr<Timer> multiclickTimer;
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
	
	Window();
	virtual void setRootViewController(ViewController* viewController);
	virtual void setNeedsLayout();
	virtual void resizeSurface(int width, int height, float scale);
	virtual void draw();
	virtual void requestRedraw();
	virtual void dispatchInputEvent(int event, int source, long time, int x, int y);
	virtual POINT offsetToView(View* view);
    
    virtual bool setFocusedView(View* view);
	void attachRootVC();

	// Render state
	int _doneGlInit;
	bool _rootVcAttached;
	QuadBuffer* _quadBuffer;
	struct {
		//GLint blend:1;
		GLint scissorTest:1;
	} _enabledFlags;
	void setBlendMode(int blendMode);
	void glEnableScissorTest(bool enabled);
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

};


