//
//  emBeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#pragma once
#include "app.h"
#include "diskinfo.h"


class DisksListItem : public Object {
public:
	Game* _game;
	DiskInfo* _selectedDiskInfo;
	
	DisksListItem(Game* game);
	string getTitle();
	string getSubtitle();
	string getImageUrl();
};

class DisksListAdapter : public SimpleListAdapter {
public:
	DisksListAdapter(string srcfile, 
					 const string& itemLayoutId);
    ~DisksListAdapter();
	
protected:
    string srcfile;
    
    virtual void handleJson(const Variant& json);
};

