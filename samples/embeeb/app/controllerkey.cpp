//
//  controllerkey.cpp
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//


#include "controllerkey.h"
#include "controller.h"
#include "controllerview.h"




ControllerKey* ControllerKey::keyFromJson(JsonObject* json, Controller* controller) {
	return new ControllerKeySingle(controller, json);
}

ControllerKey::ControllerKey(Controller* controller) {
	_controller = controller;
    _keyColour = 0xff404040;
	_delegate = controller;
    controller->addKey(this);
}

ControllerKey::ControllerKey(Controller* controller, JsonObject* json) : ControllerKey(controller) {
	_rect = RECTfromString(json->getString("rect"));
}

ControllerKey::ControllerKey(const ControllerKey& src) {
	_controller = src._controller;
	_rect = src._rect;
}


JsonObject* ControllerKey::toJson() {
	JsonObject* json = new JsonObject();
	json->putValue("rect", _rect.toString());
	return json;
}


#define SNAP_TO_GRID(x) (((int)x + 8) & ~15);
/*
RECT ControllerKey::drawRect() {
	RECT r = _rect;
	RECT_scale(r, _view->getWidth()/320.0, _view->getHeight()/216.0);
	return r;
}*/

RECT ControllerKey::normalizeRect(RECT rect) {
	rect.scale(320.0/_view->getWidth(), 216.0/_view->getHeight());
	return rect;
}

void ControllerKey::layout() {
	_drawRect = _rect;
	_drawRect.scale(_view->getWidth()/320.0, _view->getHeight()/216.0);
    RECT rect = _drawRect;
    rect.inset(1, 1);
    _bkgndOp->setRect(rect);    
}

void ControllerKey::attachToView(ControllerView* view) {
    _view = view;

    RECT rect = _drawRect;
    rect.inset(1, 1);
    //_bkgndOp = new RoundRectRenderOp(_view, rect, _isTouched ? colourForPressedKey : colourForKeyBackground, 0,0, 4);
    //COLOUR fillColour = (_isTouched||_highlighted)? colourForPressedKey : _keyColour;
    _bkgndOp = new ColorRectFillRenderOp(_view, rect, _keyColour);
    view->addRenderOp(_bkgndOp);
}
void ControllerKey::detachFromView(ControllerView* view) {
    _view = NULL;
}


void ControllerKey::invalidate() {
	_view->invalidateRect(_drawRect);
}


// Touch
void ControllerKey::handleTouchBegan() {
	_delegate->handleTouchBeganInKey(this, _view);
}
void ControllerKey::handleTouchMove() {
}
void ControllerKey::handleTouchEnd() {
	_delegate->handleTouchEndInKey(this, _view);
}





ControllerKeySingle::ControllerKeySingle(Controller* controller, JsonObject* json) : ControllerKey(controller, json) {
	_action = json->getString("a");
	_beebKey = BeebKey_keyByName(json->getString("b").data());
	_beebKeyName = _beebKey->name;
	_nameOfControllerToActivate = json->getString("c");
}

JsonObject* ControllerKeySingle::toJson() {
	JsonObject* json = ControllerKey::toJson();
	json->putValue("a", _action);
	json->putValue("b", _beebKeyName);
	json->putValue("c", _nameOfControllerToActivate);
	return json;
}

/*
- (id)copyWithZone:(NSZone *)zone {
	ControllerKeySingle* copy = [super copyWithZone:zone];
	copy.action = self.action;
	copy.beebKey = self.beebKey;
	copy.nameOfControllerToActivate = self.nameOfControllerToActivate;
	return copy;
}*/

void ControllerKeySingle::setBeebKey(BeebKey* beebKey) {
    _beebKey = beebKey;
    _beebKeyName = beebKey->name;
}


// Touch
void ControllerKeySingle::handleTouchBegan() {
	//Beeb_postKeyboardEvent(self.view.hbeeb, (int)self.beebKey->scancode, true);
	invalidate();
	ControllerKey::handleTouchBegan();
}

void ControllerKeySingle::handleTouchMove() {
}

void ControllerKeySingle::handleTouchEnd() {
	//Beeb_postKeyboardEvent(self.view.hbeeb, (int)self.beebKey->scancode, false);
	invalidate();
	ControllerKey::handleTouchEnd();
	if (_nameOfControllerToActivate.length()) {
		_view->activateControllerByName(_nameOfControllerToActivate);
	}
}

