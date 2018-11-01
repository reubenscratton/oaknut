//
//  controllerview.cpp
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#include "controllerview.h"

DECLARE_DYNCREATE(ControllerView);

ControllerView::ControllerView() {
    setBackgroundColor(app.getStyleColor("keyboard.background"));
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

    _cachedFrameRect = RECT::zero();
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


bool ControllerView::handleInputEvent(INPUTEVENT *event) {

	if (event->type==INPUT_EVENT_DOWN || event->type==INPUT_EVENT_MOVE) {
		setTouchedKey(event->deviceIndex, hitTest(event->ptLocal));
	}
	if (event->type==INPUT_EVENT_UP) {
		setTouchedKey(event->deviceIndex, NULL);
	}

	return true;
}


void ControllerView::layout(RECT constraint) {
    View::layout(constraint);
    if (!_cachedFrameRect.equal(_rect)) {
        _cachedFrameRect = _rect;
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

IKeyboardInputHandler* ControllerView::getKeyboardInputHandler() {
    return this;
}


int beebKeyFor(int keyCode, char32_t charCode) {
#ifdef PLATFORM_WEB
    if (keyCode==0 && charCode == 16) {
        return ScanCode_Shift;
    }
#endif
    if (charCode != 0) {
        return BeebKey_scancodeForChar(charCode);
    }
#ifdef PLATFORM_APPLE
     return BeebKey_scancodeForMacVKeycode(keyCode);
#else
     return BeebKey_scancodeForUsbHIDKeycode(keyCode);
 #endif
}

// IKeyboardInputHandler
void ControllerView::keyInputEvent(KeyboardInputEventType keyboardInputEventType, KeyboardInputSpecialKeyCode specialKeyCode, int osKeyCode, char32_t charCode) {
    int bk = beebKeyFor(osKeyCode ,charCode);
    if (bk) {
        _beeb->postKeyboardEvent(bk, keyboardInputEventType==KeyDown);
    }
}


