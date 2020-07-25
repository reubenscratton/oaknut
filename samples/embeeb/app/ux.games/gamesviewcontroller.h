//
//  emBeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#pragma once
#include "../app.h"
#include "gameitem.h"


class GamesListAdapter : public SimpleListAdapter<GameItem*> {
public:
    GamesListAdapter(string srcfile, bool best);
    
    View* createItemView(LISTINDEX index) override;
    void bindItemView(View* itemview, LISTINDEX index) override;
    virtual void handleJson(const variant& json);
    
protected:
    string srcfile;
    bool _best;
    
};

class GamesViewController : public ViewController {
public:
	ListView* _listView;
    
    GamesViewController(std::function<void(Game*)> delegate);
	
	// Overrides
	void onWillAppear(bool firstTime) override;
	void onDidDisappear(bool lastTime) override;
    void applySafeInsets(const EDGEINSETS& safeInsets) override;

protected:
    std::function<void(Game*)> _delegate;
	SegmentedControl* _segctrl;
    SearchBox* _searchBox;
	
	sp<GamesListAdapter> _disksListAdapterBest;
	sp<GamesListAdapter> _disksListAdapterAll;
    
    EDGEINSETS _safeAreaInsets;
    void updateInsets();
};

