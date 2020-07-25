//
//  emBeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//


#include "controllerkey.h"
#include "../model/controller.h"
#include "controllerview.h"




ControllerKey* ControllerKey::keyFromJson(const variant& v, Controller* controller) {
	auto key = new ControllerKeySingle(controller);
    key->fromVariant(v);
    return key;
}

ControllerKey::ControllerKey(Controller* controller) {
	_controller = controller;
    _keyColour = 0xff404040;
	_delegate = controller;
    controller->addKey(this);
}

void ControllerKey::fromVariant(const variant& v) {
	_rect = RECT(v.stringVal("rect"));
}

ControllerKey::ControllerKey(const ControllerKey& src) {
	_controller = src._controller;
	_rect = src._rect;
}


void ControllerKey::toVariant(variant &v) {
	v["rect"] = _rect.toString();
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
    _bkgndOp = new RectRenderOp();
    _bkgndOp->setRect(rect);
    _bkgndOp->setFillColor(_keyColour);
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





ControllerKeySingle::ControllerKeySingle(Controller* controller) : ControllerKey(controller) {
}

void ControllerKeySingle::fromVariant(const variant& v) {
    ControllerKey::fromVariant(v);
	_action = v.stringVal("a");
    _beebKey = BeebKey_keyByName(v.stringVal("b").c_str());
	_beebKeyName = _beebKey->name;
	_nameOfControllerToActivate = v.stringVal("c");
}

void ControllerKeySingle::toVariant(variant& v) {
	ControllerKey::toVariant(v);
	v["a"] = _action;
	v["b"] = _beebKeyName;
	v["c"] = _nameOfControllerToActivate;
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

