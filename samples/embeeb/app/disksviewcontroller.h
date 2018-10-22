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
    void updateSafeArea(const RECT& safeArea) override;

protected:
    std::function<void(Game*)> _delegate;
	SegmentedControl* _segctrl;
    sp<SearchBox> _searchBox;
	
	sp<DisksListAdapter> _disksListAdapterBest;
	sp<DisksListAdapter> _disksListAdapterAll;
    
    EDGEINSETS _safeAreaInsets;
    void updateInsets();
};

#endif
