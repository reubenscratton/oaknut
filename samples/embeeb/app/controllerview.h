//
//  controllerview.h
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#include "app.h"
#include "diskcontrols.h"
#include "controller.h"


class ControllerView : public View { // <UIKeyInput>
public:

	DiskControls* _diskControls;
	ObjPtr<Controller> _controller;
	Beeb* _beeb;
	bool _hardwareKeyboardAttached;
	bool _softwareKeyboardVisible;
	ControllerKey* _touchedKeys[10];
    RECT _cachedFrameRect;
    
	ControllerView();
	
	// Overrides
	virtual bool onTouchEvent(int eventType, int finger, POINT pt);
	virtual void layout();
	virtual void setController(Controller* controller);
	virtual void activateControllerByName(const string& controllerName);
	virtual ControllerKey* hitTest(POINT pt);
	virtual void setTouchedKey(int finger, ControllerKey* currentKey);
	

};


