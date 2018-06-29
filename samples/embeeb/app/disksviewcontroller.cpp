//
//  disksviewcontroller.cpp
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#include "disksviewcontroller.h"
#include "diskinfo.h"
#include "bestdiskslistadapter.h"
#include "alldiskslistadapter.h"



DisksViewController::DisksViewController(std::function<void(Game*)> delegate) {
	_delegate = delegate;
	
	// Root view
	View* view = new View();
	view->setBackgroundColour(0xfff8f8f8);
	setView(view);
	
	
	// Navbar
	_segctrl = new SegmentedControl();
	//_segctrl->setTintColour(colourForNavTint);
	_segctrl->setSelectedTextColour(app.getStyleColour("navbar.background")); // as if text is transparent
	_segctrl->addSegment("Best");
	_segctrl->addSegment("All");
    _segctrl->setSegmentSelectedDelegate([=](int index) {
        _searchBox->setVisibility((index==1) ? Visible : Gone);
        _listView->setAdapter(index?_disksListAdapterAll:_disksListAdapterBest);
        _listView->setScrollInsets(EDGEINSETS(0, _minTopScrollInset + ((index==0)?0:app.dp(40)), 0, 0));
    });
	_navigationItem->setTitleView(_segctrl);
    _navigationItem->addLeftButton(NavigationItem::createIconButton("images/back.png", [&] (View*) { onBackButtonClicked(); }));
	
	_disksListAdapterBest = new BestDisksListAdapter("http://www.ibeeb.co.uk/best.json");
	_disksListAdapterAll = new AllDisksListAdapter("http://www.ibeeb.co.uk/all.json");
	
	_listView = new ListView();
    _listView->setMeasureSpecs(MEASURESPEC::FillParent(), MEASURESPEC::FillParent());
    float statusBarHeight = app.getStyleFloat("statusbar.height");
    _minTopScrollInset = app.getStyleFloat("navbar.height") + statusBarHeight;
    _listView->_onItemTapDelegate = [=](View* itemView, LISTINDEX index) {
        DisksListItem* gameItem = (DisksListItem*)_listView->_adapter->getItem(index);
        _delegate(gameItem->_game);
        _navigationController->popViewController();
    };
    _listView->_onItemLongPressDelegate = [=](View* itemView, LISTINDEX index) {
    };
	view->addSubview(_listView);
    
    _searchBox = new SearchBox();
    _searchBox->setMeasureSpecs(MEASURESPEC::FillParent(), MEASURESPEC::Abs(app.dp(40))); // todo: wrap_content
    _searchBox->setAlignSpecs(ALIGNSPEC::Left(), ALIGNSPEC(NULL, 0.0f, 0.0f, _minTopScrollInset));
    view->addSubview(_searchBox);
    _searchBox->setSearchTextChangedDelegate([=](SearchBox* searchBox, const string& text) {
        _disksListAdapterAll->setFilter(text);
    });
}

/*
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return self.sections.count;
}
- (nullable NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)sectionIndex {
    GamesSection* section = self.sections[sectionIndex];
    return section.title;
}
- (NSInteger)numberOfRowsInSection:(NSInteger)sectionIndex {
    NSArray* section = self.sections[sectionIndex];
    return section.count;
}
*/


void DisksViewController::onWillResume() {
	_segctrl->setSelectedSegment(app.getIntSetting("disksSeg", 0));
    POINT pt;
    pt.x = 0;
    pt.y = app.getIntSetting("disksY", 0);
    //_listView->setContentOffset(pt);
}
void DisksViewController::onDidPause() {
	app.setIntSetting("disksSeg", _segctrl->_selectedIndex);
	app.setIntSetting("disksY", _listView->getContentOffset().y);
	app.saveSettings();
}

