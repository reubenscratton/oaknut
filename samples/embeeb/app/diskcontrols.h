//
//  diskcontrols.h
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//


#ifndef _DISKCONTROLS_H_
#define _DISKCONTROLS_H_

#include "app.h"
#include "controller.h"


class DiskControls : public Object, public ISerializeToVariant  {
public:

	bool _usesKeyboard;
	vector<ObjPtr<Controller>> _controllers;

	DiskControls();
	DiskControls(const DiskControls& src);
	
    void fromVariant(const Variant& v) override;
	void toVariant(Variant& v) override;
    
	virtual Controller* controllerById(const string& controllerId);
	virtual Controller* initialController();


	static void loadDiskControlsForDiskId(const string& diskId);
	static void setDiskControls(DiskControls* diskControls, const string& diskId);
	static bool hasCustomControlsForDiskId(const string& diskId);
	static void resetControlsForDiskId(const string& diskId);


};

#endif

