//
//  controllerkey.h
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#ifndef _CONTROLLERKEY_H_
#define _CONTROLLERKEY_H_

#include "app.h"

class Controller;
class ControllerView;
class ControllerKey;


class IControllerKeyDelegate {
public:
	virtual void handleTouchBeganInKey(ControllerKey* key, ControllerView* controllerView) =0;
	//- (void)handleTouchMoveInView:(ControllerView*)controllerView;
	virtual void handleTouchEndInKey(ControllerKey* key, ControllerView* controllerView) =0;
};



class ControllerKey : public Object { // <NSCopying>
public:

	static ControllerKey* keyFromJson(JsonObject* json, Controller* controller);

	Controller* _controller; 	// Weak cos pointing against ownership dir
	IControllerKeyDelegate* _delegate;
protected:
	ControllerView* _view;		// Weak cos pointing against ownership dir
public:
    ObjPtr<RectRenderOp> _bkgndOp;

	/**
	 Rect is normalized to a parent size of 320x216, i.e. a standard
	 portrait keyboard.
	 */
	RECT _rect;
	RECT normalizeRect(RECT rect);

	bool _isTouched;

	ControllerKey(Controller* controller);
	ControllerKey(Controller* controller, JsonObject* json);
	ControllerKey(const ControllerKey& src);
	
    virtual void attachToView(ControllerView* view);
    virtual void detachFromView(ControllerView* view);

	virtual JsonObject* toJson();

	// Touch
	virtual void handleTouchBegan();
	virtual void handleTouchMove();
	virtual void handleTouchEnd();

	// Drawing
	virtual void layout();
	virtual void invalidate();
//protected:
	RECT _drawRect;
    COLOR _keyColour;

};

class ControllerKeySingle : public ControllerKey {
public:

	string _action;
	BeebKey* _beebKey;
	string _beebKeyName;
	string _nameOfControllerToActivate;
	
	ControllerKeySingle(Controller* controller, JsonObject* json);
	JsonObject* toJson();
	void setBeebKey(BeebKey* beebKey);
	
	virtual void handleTouchBegan();
	virtual void handleTouchMove();
	virtual void handleTouchEnd();

};


#endif
