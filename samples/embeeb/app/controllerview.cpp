//
//  controllerview.cpp
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#include "controllerview.h"

DECLARE_DYNCREATE(ControllerView);

ControllerView::ControllerView() {
    setBackgroundColour(app.getStyleColour("keyboard.background"));
	_opaque = true;
	setUsePrivateSurface(true);
}

void ControllerView::setController(Controller* controller) {
    assert(!_controller);
	_controller = controller;
	for (int i=0 ; i<controller->_keys.size() ; i++) {
		ControllerKey* key = controller->_keys.at(i);
        key->attachToView(this);
	}

    _cachedFrameRect = RECT_Zero;
    invalidateContentSize();
	setNeedsLayout();
}

ControllerKey* ControllerView::hitTest(POINT pt) {
	for (int i=0 ; i<_controller->_keys.size() ; i++) {
		ControllerKey* key = _controller->_keys.at(i);
		if (key->_drawRect.contains(pt)) {
			return key;
		}
	}
	return nullptr;
}


void ControllerView::setTouchedKey(int finger, ControllerKey* currentKey) {
	if (_touchedKeys[finger] != currentKey) {
		if (_touchedKeys[finger]) {
			_touchedKeys[finger]->_isTouched = false;
			_touchedKeys[finger]->handleTouchEnd();
		}
		_touchedKeys[finger] = currentKey;
		if (currentKey) {
			currentKey->_isTouched = true;
			currentKey->handleTouchBegan();
		}
	}
}


bool ControllerView::onTouchEvent(int eventType, int source, POINT pt) {

	if (eventType==INPUT_EVENT_DOWN || (eventType==INPUT_EVENT_MOVE && SOURCE_TYPE(source)==INPUT_SOURCE_TYPE_FINGER)) {
		setTouchedKey(SOURCE_ID(source), hitTest(pt));
	}
	if (eventType==INPUT_EVENT_UP) {
		setTouchedKey(SOURCE_ID(source), NULL);
	}

	return true;
}


void ControllerView::layout() {
    if (!_cachedFrameRect.equal(_rect)) {
        _cachedFrameRect = _rect;
        View::layout();
        for (auto it = _controller->_keys.begin() ; it!=_controller->_keys.end() ; it++) {
            ControllerKey* key = *it;
            key->layout();
        }
    }
}



void ControllerView::activateControllerByName(const string& controllerName) {
	/*for (Controller* controller in self.diskControls.controllers) {
		if ([controller.name isEqualToString:controllerName]) {
			self.controller = controller;
			return;
		}
	}
	app.warn("Couldn't activate controller %@", controllerName);
	*/
}

	

