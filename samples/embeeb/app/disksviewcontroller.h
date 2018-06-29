//
//  disksviewcontroller.h
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#ifndef _DISKSVIEWCONTROLLER_H_
#define _DISKSVIEWCONTROLLER_H_

#include "app.h"
#include "diskslistadapter.h"



class DisksViewController : public ViewController {
public:
	ListView* _listView;
    float _minTopScrollInset;
    
    DisksViewController(std::function<void(Game*)> delegate);
	
	
	// Overrides
	virtual void onWillResume();
	virtual void onDidPause();

protected:
    std::function<void(Game*)> _delegate;
	SegmentedControl* _segctrl;
    ObjPtr<SearchBox> _searchBox;
	
	ObjPtr<DisksListAdapter> _disksListAdapterBest;
	ObjPtr<DisksListAdapter> _disksListAdapterAll;
};

#endif
