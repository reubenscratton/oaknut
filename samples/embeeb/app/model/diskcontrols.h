//
//  emBeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#pragma once
#include "app.h"
#include "controller.h"


class DiskControls : public Object, public ISerializeToVariant  {
public:

	bool _usesKeyboard;
	vector<sp<Controller>> _controllers;

	DiskControls();
	DiskControls(const DiskControls& src);
	
    void fromVariant(const variant& v) override;
	void toVariant(variant& v) override;
    
	virtual Controller* controllerById(const string& controllerId);
	virtual Controller* initialController();


	static void loadDiskControlsForDiskId(const string& diskId);
	static void setDiskControls(DiskControls* diskControls, const string& diskId);
	static bool hasCustomControlsForDiskId(const string& diskId);
	static void resetControlsForDiskId(const string& diskId);


};

