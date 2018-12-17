//
//  emBeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#pragma once
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



class ControllerKey : public Object, public ISerializeToVariant {
public:

	static ControllerKey* keyFromJson(const variant& v, Controller* controller);

	Controller* _controller; 	// Weak cos pointing against ownership dir
	IControllerKeyDelegate* _delegate;
protected:
	ControllerView* _view;		// Weak cos pointing against ownership dir
public:
    sp<RectRenderOp> _bkgndOp;

	/**
	 Rect is normalized to a parent size of 320x216, i.e. a standard
	 portrait keyboard.
	 */
	RECT _rect;
	RECT normalizeRect(RECT rect);

	bool _isTouched;

	ControllerKey(Controller* controller);
	ControllerKey(const ControllerKey& src);
	
    virtual void attachToView(ControllerView* view);
    virtual void detachFromView(ControllerView* view);

    // ISerializeToVariant
    void fromVariant(const variant& v) override;
	void toVariant(variant& v) override;

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
	
	ControllerKeySingle(Controller* controller);
    
    // ISerializeToVariant
    void fromVariant(const variant& v) override;
    void toVariant(variant& v) override;

    void setBeebKey(BeebKey* beebKey);
	
	void handleTouchBegan() override;
	void handleTouchMove() override;
	void handleTouchEnd() override;

};


