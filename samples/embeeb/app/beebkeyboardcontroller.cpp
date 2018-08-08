//
//  beebkeyboardcontroller.cpp
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#include "beebkeyboardcontroller.h"
#include "controllerview.h"
//@import AudioToolbox;

//extern NSString* kResetCurrentSnapshot;

static COLOR colourForPressedKey;
static COLOR colourForKeyBackground;
static COLOR colourForFunctionKeyBackground;
static COLOR colourForLEDKeyBackground;


BeebKeyboardKey::BeebKeyboardKey(Controller* controller, BeebKey* beebKey)
: BeebKeyboardKey(controller, beebKey, 1.0f) {
}
BeebKeyboardKey::BeebKeyboardKey(Controller* controller, BeebKey* beebKey, float weight)
: ControllerKey(controller) {
	setBeebKey(beebKey);
	_weight = weight;
}

void BeebKeyboardKey::setBeebKey(BeebKey* beebKey) {
	_beebKey = beebKey;
}

void BeebKeyboardKey::invalidate() {
    _bkgndOp->setFillColor((_isTouched||_highlighted)? colourForPressedKey : _keyColour);
    ControllerKey::invalidate();
}



void BeebKeyboardKey::attachToView(ControllerView *view) {

    string name = _beebKey->mainLabel?_beebKey->mainLabel:_beebKey->name;
    _keyColour = colourForKeyBackground;
    if (name[0] == 'f') {
        _keyColour = colourForFunctionKeyBackground;
    }
    
    ControllerKey::attachToView(view);


    _label = new TextRenderer();
    _label->setText(name);
    _label->setDefaultColor(0xFFFFFFFF);
    _label->setDefaultFont(app.getStyleFont((name.length()>1) ? "keyboard.labels-small" : "keyboard.labels-normal"));
    _label->setGravity({GRAVITY_CENTER, GRAVITY_CENTER});
    _label->measure();

    if (_beebKey->shiftLabel) {
        _labelTop = new TextRenderer();
        _labelTop->setText(_beebKey->shiftLabel);
        _labelTop->setDefaultColor(0xFFFFFFFF);
        _labelTop->setDefaultFont(app.getStyleFont("keyboard.labels-tiny"));
        _labelTop->setGravity({GRAVITY_CENTER, GRAVITY_TOP});
        _labelTop->measure();
	}
}
void BeebKeyboardKey::detachFromView(ControllerView *view) {
    ControllerKey::detachFromView(view);
    /*view->removeRenderOp(_label);
    if (_labelTop) {
        view->removeRenderOp(_labelTop);
    }*/
}

void BeebKeyboardKey::layout() {
	ControllerKey::layout();
    _label->layout(_drawRect);
    _label->updateRenderOps(_view);
    if (_labelTop) {
        _labelTop->layout(_drawRect);
        _labelTop->updateRenderOps(_view);
    }
}



class BeebKeyboardShiftKey : public BeebKeyboardKey {
public:
	float timeDown;

	BeebKeyboardShiftKey(Controller* controller) : BeebKeyboardKey(controller, &beebKeyShift, 1.5f) {
	}

	/*- (void)handleTouchBegan {
		[super handleTouchBegan];
		[self performSelector:@selector(onLongPress:) withObject:self.view afterDelay:2];
	}

	- (void)onLongPress:(ControllerView*)controllerView {
		[controllerView becomeFirstResponder];
	}

	- (void)handleTouchEnd {
		[super handleTouchEnd];
		[NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(onLongPress:) object:self.view];
	}*/

};

void BeebKeyboardKey::setRect(const RECT& rect) {
	_rect = rect;
	
	//_label.layout(_rect);
}

void BeebKeyboardController::KeyRow::layout(const RECT& bounds) {
	float sumweights = 0;
	for (auto it = _keys.begin() ; it!=_keys.end() ; it++) {
		sumweights += (*it)->_weight;
	}
	float x = bounds.origin.x;
	float excess_space = bounds.size.width;
	float excess_unit = (sumweights==0) ? 0 : (excess_space / sumweights);
	for (auto it = _keys.begin() ; it!=_keys.end() ; it++) {
		float keywidth =  excess_unit * (*it)->_weight;
		(*it)->setRect(RECT(x, bounds.origin.y, keywidth, bounds.size.height));
		x += keywidth;
	}
}




BeebKeyboardController::BeebKeyboardController() {

    colourForKeyBackground = app.getStyleColor("keyboard.backgroundKeyNormal");
    colourForPressedKey = app.getStyleColor("keyboard.backgroundKeyPressed");
    colourForFunctionKeyBackground = app.getStyleColor("keyboard.backgroundKeyFunction");
    colourForLEDKeyBackground = app.getStyleColor("keyboard.backgroundKeyLED");

	KeyRow* row = new KeyRow(); _rows.push_back(row);
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyTab));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyF0));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyF1));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyF2));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyF3));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyF4));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyF5));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyF6));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyF7));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyF8));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyF9));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyBreak));
	
	row = new KeyRow(); _rows.push_back(row);
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyEscape));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKey1));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKey2));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKey3));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKey4));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKey5));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKey6));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKey7));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKey8));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKey9));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKey0));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyMinus));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyCaret));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyBackslash));
	
	row = new KeyRow(); _rows.push_back(row);
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyQ));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyW));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyE));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyR));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyT));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyY));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyU));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyI));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyO));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyP));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyAt));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyBracketLeft));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyUnderscore));
	
	row = new KeyRow(); _rows.push_back(row);
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyA));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyS));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyD));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyF));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyG));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyH));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyJ));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyK));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyL));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeySemicolon));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyColon));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyBracketRight));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyLeft));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyRight));
	
	row = new KeyRow(); _rows.push_back(row);
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyZ));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyX));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyC));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyV));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyB));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyN));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyM));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyComma));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyPeriod));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeySlash));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyUp));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyDown));
	
	row = new KeyRow(); _rows.push_back(row);
	_capsLockKey = new BeebKeyboardKey(this, &beebKeyCaps);
	row->_keys.push_back(_capsLockKey);
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyCtrl));
	_shiftLockKey = new BeebKeyboardKey(this, &beebKeyShiftLock);
	row->_keys.push_back(_shiftLockKey);
	row->_keys.push_back(new BeebKeyboardShiftKey(this));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeySpace, 3.5f));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyDelete));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyCopy));
	row->_keys.push_back(new BeebKeyboardKey(this, &beebKeyReturn, 2.f));
	
	RECT keybdRect = RECT(0,0,320,216-8); // -8 to leave a small gap at bottom
	keybdRect.size.height /= _rows.size();
	for (auto it = _rows.begin() ; it!=_rows.end() ; it++) {
		(*it)->layout(keybdRect);
		keybdRect.origin.y += keybdRect.size.height;
	}

}

void BeebKeyboardController::highlightKey(const string& keyName) {
	for (auto it=_keys.begin() ; it!=_keys.end() ; it++) {
		BeebKeyboardKey* key = (BeebKeyboardKey*)(*it)._obj;
		key->_highlighted = false;
		if (0==strcmp(keyName.data(), key->_beebKey->name)) {
			key->_highlighted = true;
		}
	}
}

void BeebKeyboardController::setShiftLockLED(bool shiftLockLED) {
    if (_shiftLockLED != shiftLockLED) {
        _shiftLockLED  = shiftLockLED;
        if (_shiftLockKey) {
            _shiftLockKey->_keyColour = shiftLockLED ? colourForLEDKeyBackground : colourForKeyBackground;
            _shiftLockKey->invalidate();
        }
    }
}
void BeebKeyboardController::setCapsLockLED(bool capsLockLED) {
    if (_capsLockLED != capsLockLED) {
        _capsLockLED  = capsLockLED;
        if (_capsLockKey) {
            _capsLockKey->_keyColour = capsLockLED ? colourForLEDKeyBackground : colourForKeyBackground;
            _capsLockKey->invalidate();
        }
    }
}

/**
 BeebKeyboardKey touch handling. Touch handling done at controller 
 level (rather than key level) cos of Shift.
 */
void BeebKeyboardController::handleTouchBeganInKey(ControllerKey* key, ControllerView* controllerView) {
	BeebKeyboardKey* beebKeyboardKey = (BeebKeyboardKey*)key;
	controllerView->_beeb->postKeyboardEvent((int)beebKeyboardKey->_beebKey->scancode, true);
	key->invalidate();
	//AudioServicesPlaySystemSound(1104);
}

void BeebKeyboardController::handleTouchEndInKey(ControllerKey* key, ControllerView* controllerView) {
	BeebKeyboardKey* beebKeyboardKey = (BeebKeyboardKey*)key;
	controllerView->_beeb->postKeyboardEvent((int)beebKeyboardKey->_beebKey->scancode, false);
	key->invalidate();
	
	if (beebKeyboardKey->_beebKey->scancode == ScanCode_Break) {
		app.log("todo: post brk notify");
		//[[NSNotificationCenter defaultCenter] postNotificationName:kResetCurrentSnapshot object:nil];
	}
}


