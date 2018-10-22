//
//  controllerview.h
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#include "app.h"
#include "diskcontrols.h"
#include "controller.h"


class ControllerView : public View, public IKeyboardInputHandler {
public:

	DiskControls* _diskControls;
	sp<Controller> _controller;
	Beeb* _beeb;
	bool _hardwareKeyboardAttached;
	bool _softwareKeyboardVisible;
	ControllerKey* _touchedKeys[10];
    RECT _cachedFrameRect;
    
	ControllerView();
	
	// Overrides
	bool handleInputEvent(INPUTEVENT* event) override;
	void layout() override;
    IKeyboardInputHandler* getKeyboardInputHandler() override;

    // API
    virtual void setController(Controller* controller);
	virtual void activateControllerByName(const string& controllerName);
	virtual ControllerKey* hitTest(POINT pt);
	void setTouchedKey(int finger, ControllerKey* currentKey);
	
    // IKeyboardInputHandler
    void keyInputEvent(KeyboardInputEventType keyboardInputEventType, KeyboardInputSpecialKeyCode specialKeyCode, int osKeyCode, char32_t charCode) override;
};


