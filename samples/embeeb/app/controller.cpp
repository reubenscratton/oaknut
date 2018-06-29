//
//  controller.cpp
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#include "controller.h"



Controller::Controller() {
}

Controller::Controller(JsonObject* json) {
	_name = json->getString("name");
	JsonArray* jsonKeys = json->getArray("keys");
	for (int i=0 ; i<jsonKeys->_elements.size() ; i++) {
		JsonObject* jsonKey = jsonKeys->getObject<JsonObject>(i);
		ControllerKey::keyFromJson(jsonKey, this);
	}
	_trigger = json->getString("trigger");
}

Controller::Controller(const Controller& src) {
	_name = src._name;
	for (int i=0 ;i<src._keys.size() ; i++) {
		ControllerKey* keyCopy = new ControllerKey(*src._keys.at(i)._obj);
		_keys.push_back(keyCopy);
	}
	_trigger = src._trigger;
}

JsonObject* Controller::toJson() {
	JsonObject* json = new JsonObject();
	json->putValue("name", _name);
	if (_trigger.size()) {
		json->putValue("trigger", _trigger);
	}
	JsonArray* keys = new JsonArray();
	for (int i=0 ;i<_keys.size() ; i++) {
		ControllerKey* key = _keys.at(i);
		keys->addValue(key->toJson());
	}
	json->putValue("keys", keys);
	return json;
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

