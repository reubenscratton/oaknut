//
//  controller.cpp
//  emBeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#include "controller.h"



Controller::Controller() {
}

Controller::Controller(const Controller& src) {
	_name = src._name;
	for (int i=0 ;i<src._keys.size() ; i++) {
		ControllerKey* keyCopy = new ControllerKey(*src._keys.at(i)._obj);
		_keys.push_back(keyCopy);
	}
	_trigger = src._trigger;
}


void Controller::fromVariant(const variant& v) {
    _name = v.stringVal("name");
    auto keys = v.arrayVal("keys");
    for (auto& key : keys) {
        ControllerKey::keyFromJson(key, this);
    }
    _trigger = v.stringVal("trigger");
}

void Controller::toVariant(variant& v) {
	v.set("name", _name);
	if (_trigger.length()) {
		v.set("trigger", _trigger);
	}
    variant vkeys;
    vkeys.setType(variant::ARRAY);
    for (auto& key : _keys) {
		vkeys.appendVal(key._obj);
	}
	v.set("keys", vkeys);
}

void Controller::addKey(ControllerKey* key) {
	_keys.push_back(key);
}
void Controller::removeKey(ControllerKey* key) {
	_keys.erase(find(_keys.begin(), _keys.end(), key));
}

/**
 ControllerKeyDelegate
 */
void Controller::handleTouchBeganInKey(ControllerKey* key, ControllerView* controllerView) {
}
void Controller::handleTouchEndInKey(ControllerKey* key, ControllerView* controllerView) {
}

