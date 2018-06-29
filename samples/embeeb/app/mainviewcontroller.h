//
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//


#ifndef _MAINVIEWCONTROLLER_H_
#define _MAINVIEWCONTROLLER_H_

#include "app.h"
#include "beebkeyboardcontroller.h"
#include "diskinfo.h"

class MainViewController : public ViewController,
						   public IBeebKeyboardCallbacks, public IURLRequestDelegate {
public:

	MainViewController();

	Beeb* _beeb;
	class BeebView* _beebView;
	ControllerView* _controllerView;
	BeebKeyboardController* keyboardController;
	DiskInfo* _currentDiskInfo;
    ToolbarButton* _controllersButton;
    ToolbarButton* _nextControllerButton;
    ObjPtr<class Snapshot> _currentSnapshot;
//@property (nonatomic) NSValue* pendingControllerToActivate;

	// Overrides
	virtual void onWillPause();
	virtual void onDidPause();
    virtual void onDidResume();
                               
    virtual void LEDsChanged();
		

	virtual void onUrlRequestLoad(URLData* data);

};

#endif
