//
//  controller.cpp
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
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


void Controller::fromVariant(const Variant& v) {
    _name = v.stringVal("name");
    auto keys = v.arrayVal("keys");
    for (auto& key : keys) {
        ControllerKey::keyFromJson(key, this);
    }
    _trigger = v.stringVal("trigger");
}

void Controller::toVariant(Variant& v) {
	v.set("name", _name);
	if (_trigger.length()) {
		v.set("trigger", _trigger);
	}
    vector<Variant> vkeys;
    for (auto& key : _keys) {
        Variant vkey;
        key->toVariant(vkey);
		vkeys.push_back(vkey);
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

