//
//  emBeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#include "../app.h"
#include "../model/controller.h"


class BeebKeyboardKey : public ControllerKey {
public:
	BeebKey* _beebKey;
	TextRenderer* _label;
	TextRenderer* _labelTop;
	float _weight;
	bool _highlighted;
	
	BeebKeyboardKey(Controller* controller, BeebKey* beebKey);
	BeebKeyboardKey(Controller* controller, BeebKey* beebKey, float weight);
	void setBeebKey(BeebKey* beebKey);
	void setRect(const RECT& rect);
	
	virtual void layout();
    virtual void invalidate();

    virtual void attachToView(ControllerView* view);
    virtual void detachFromView(ControllerView* view);
};


class BeebKeyboardController : public Controller {
public:
	class KeyRow {
	public:
		vector<BeebKeyboardKey*> _keys;
		void layout(const RECT& bounds);
	};
	
	vector<KeyRow*> _rows;
	BeebKeyboardKey* _capsLockKey;
	BeebKeyboardKey* _shiftLockKey;
	bool _capsLockLED;
	bool _shiftLockLED;

	BeebKeyboardController();
	void highlightKey(const string& keyName);
	void setShiftLockLED(bool shiftLockLED);
	void setCapsLockLED(bool capsLockLED);
	
	// IControllerKeyDelegate
	void handleTouchBeganInKey(ControllerKey* key, ControllerView* controllerView);
	void handleTouchEndInKey(ControllerKey* key, ControllerView* controllerView);

};




