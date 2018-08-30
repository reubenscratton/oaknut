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
    
    DisksViewController(std::function<void(Game*)> delegate);	
	
	// Overrides
	void onWillResume() override;
	void onDidPause() override;
    void setSafeAreaInsets(const EDGEINSETS& safeAreaInsets) override;

protected:
    std::function<void(Game*)> _delegate;
	SegmentedControl* _segctrl;
    ObjPtr<SearchBox> _searchBox;
	
	ObjPtr<DisksListAdapter> _disksListAdapterBest;
	ObjPtr<DisksListAdapter> _disksListAdapterAll;
    
    EDGEINSETS _safeAreaInsets;
    void updateInsets();
};

#endif
