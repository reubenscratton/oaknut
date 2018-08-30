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

class Controller : public Object, public ISerializeToVariant, public IControllerKeyDelegate {
public:

	string _name;
	vector<ObjPtr<ControllerKey>> _keys;
	string _trigger;

	Controller();
	Controller(const Controller& src);
	
	virtual void addKey(ControllerKey* key);
	virtual void removeKey(ControllerKey* key);

    void fromVariant(const variant& v) override;
    void toVariant(variant& v) override;

	// IControllerKeyDelegate
	void handleTouchBeganInKey(ControllerKey* key, ControllerView* controllerView) override;
	void handleTouchEndInKey(ControllerKey* key, ControllerView* controllerView) override;

};

#endif
