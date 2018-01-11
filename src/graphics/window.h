//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#define TOUCH_EVENT_DOWN 0
#define TOUCH_EVENT_MOVE 1
#define TOUCH_EVENT_UP   2
#define TOUCH_EVENT_CANCEL 3
#define TOUCH_EVENT_DRAG 4
//#define TOUCH_EVENT_DRAG_Y 5
#define TOUCH_EVENT_TAP 6
#define TOUCH_EVENT_FLING 7
//#define TOUCH_EVENT_EX_TAP_CONFIRMED 7
#define TOUCH_SLOP 10 // DPs
#define DBLCLICK_THRESHOLD 400
#define NUM_PAST 10

typedef struct {
	class View* touchedView;
    long timeOfDownEvent;
    POINT ptDown;
    int numClicks;
    bool isDragging;
    float dragDirection;
    POINT pastPts[NUM_PAST];
    long pastTime[NUM_PAST];
    int pastIndex, pastCount;
} TOUCH;

class Window : public Object {
public:
	
	ObjPtr<class ViewController> _rootViewController;
	vector<ObjPtr<ViewController>> _viewControllers;
    ObjPtr<Surface> _surface;
    RECT _surfaceRect;
	float _scale;
	bool _inLayoutPass;
	ObjPtr<Canvas> _canvas;
	TOUCH _touches[10];
	list<ObjPtr<class Animation>> _animations;
	bool _viewLayoutValid;
	bool _redrawNeeded;
    View* _firstResponder;
#if IOS_WEBGL_BUG
	bool _oddFrame;
#endif
    class IKeyboardInputHandler* _keyboardHandler;
	
	Window();
	virtual void setRootViewController(ViewController* viewController);
	virtual void setNeedsLayout();
	virtual void resizeSurface(int width, int height, float scale);
	virtual void draw();
	virtual void requestRedraw();
	virtual void dispatchTouchEvent(int eventType, int eventSourceId, long time, int x, int y);
	virtual POINT offsetToView(View* view);
    
    virtual bool setFirstResponder(View* view);
};


