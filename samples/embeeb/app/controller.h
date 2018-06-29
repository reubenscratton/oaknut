//
//  controller.h
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "app.h"
#include "controllerkey.h"

class Controller : public Object, public IControllerKeyDelegate {
public:

	string _name;
	vector<ObjPtr<ControllerKey>> _keys;
	string _trigger;

	Controller();
	Controller(const Controller& src);
	Controller(JsonObject* json);
	
	virtual JsonObject* toJson();
	virtual void addKey(ControllerKey* key);
	virtual void removeKey(ControllerKey* key);

	// IControllerKeyDelegate
	void handleTouchBeganInKey(ControllerKey* key, ControllerView* controllerView);
	void handleTouchEndInKey(ControllerKey* key, ControllerView* controllerView);

};

#endif
