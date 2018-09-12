//
//  disksviewcontroller.cpp
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#include "disksviewcontroller.h"
#include "diskinfo.h"
#include "alldiskslistadapter.h"


class BestDisksListAdapter : public DisksListAdapter {
public:
    BestDisksListAdapter(string srcfile) : DisksListAdapter(srcfile) {
    }    
};


DisksViewController::DisksViewController(std::function<void(Game*)> delegate) {
	_delegate = delegate;
	
    View* view = app.layoutInflate("layout/disks.res");
	setView(view);
	
	
	// Navbar
	_segctrl = new SegmentedControl();
	_segctrl->setSelectedTextColor(app.getStyleColor("navbar.background")); // as if text is transparent
	_segctrl->addSegment("Best");
	_segctrl->addSegment("All");
    _segctrl->setSegmentSelectedDelegate([=](int index) {
        _searchBox->getParent()->setVisibility((index==1) ? Visible : Gone);
        _listView->setAdapter(index?_disksListAdapterAll:_disksListAdapterBest);
        updateInsets();
    });
	_navigationItem->setTitleView(_segctrl);
    _navigationItem->addLeftButton(NavigationItem::createIconButton("images/back.png", [&] (View*) { onBackButtonClicked(); }));
	
	_disksListAdapterBest = new DisksListAdapter("http://www.ibeeb.co.uk/best.json");
	_disksListAdapterAll = new DisksListAdapter("http://www.ibeeb.co.uk/all.json");
	
    _listView = (ListView*)view->findViewById("listView");
    _listView->_onItemTapDelegate = [=](View* itemView, LISTINDEX index) {
        DiskItem& gameItem = _disksListAdapterBest->getItem(index);
        _delegate(gameItem._game);
        _navigationController->popViewController();
    };
    _listView->_onItemLongPressDelegate = [=](View* itemView, LISTINDEX index) {
    };
    
    _searchBox = (SearchBox*)view->findViewById("searchBox");
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
	app.setIntSetting("disksSeg", _segctrl->getSelectedIndex());
	app.setIntSetting("disksY", _listView->getContentOffset().y);
}

void DisksViewController::updateSafeArea(const RECT &safeArea) {
    _safeAreaInsets.left = safeArea.left();
    _safeAreaInsets.top = safeArea.top();
    _safeAreaInsets.right = _window->_surfaceRect.size.width - safeArea.right();
    _safeAreaInsets.bottom = _window->_surfaceRect.size.height - safeArea.bottom();
    updateInsets();
}

void DisksViewController::updateInsets() {
    int index = _segctrl->getSelectedIndex();
    _listView->setScrollInsets(EDGEINSETS(0, _safeAreaInsets.top + ((index==0)?0:app.dp(40)), 0, 0));
    _searchBox->getParent()->setAlignSpecs(ALIGNSPEC::Left(), ALIGNSPEC(NULL, 0.0f, 0.0f, _safeAreaInsets.top ));
}
