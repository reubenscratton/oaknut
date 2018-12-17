//
//  emBeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#include "gamesviewcontroller.h"
#include "../model/diskinfo.h"



GamesListAdapter::GamesListAdapter(string srcfile, bool best) : SimpleListAdapter() {
    this->srcfile = srcfile;
    this->_best = best;
    
    URLRequest::get(srcfile)->handleJson([&](URLRequest* req, const variant& json) {
        handleJson(json);
    });
}
    
View* GamesListAdapter::createItemView(LISTINDEX index) {
    return new GameItemView(_best);
}

void GamesListAdapter::bindItemView(View* itemview, LISTINDEX index) {
    GameItemView* gameItemView = (GameItemView*)itemview;
    GameItem& gameItem = getItem(index);
    gameItemView->bind(gameItem, LISTINDEX_ITEM(index)+1);
}


void GamesListAdapter::handleJson(const variant& json) {
    _items.clear();
    if (json.type != variant::ARRAY) {
        app.log("Oops!");
    } else {
        auto& vals = json.arrayVal();
        for (auto& val : vals) {
            Game* game = new Game();
            game->fromVariant(val);
            _items.push_back(GameItem(game));
        }
    }
    invalidate();
}



GamesViewController::GamesViewController(std::function<void(Game*)> delegate) {
	_delegate = delegate;
	
    inflate("layout/disks.res");
    bind(_listView, "listView");
    bind(_searchBox, "searchBox");

	
	// Navbar
	_segctrl = new SegmentedControl();
	_segctrl->setSelectedTextColor(app.getStyleColor("NavigationBar.background")); // as if text is transparent
	_segctrl->addSegment("Best");
	_segctrl->addSegment("All");
    _segctrl->onSegmentSelected = [=](int index) {
        _searchBox->getParent()->setVisibility((index==1) ? Visible : Gone);
        _listView->setAdapter(index?_disksListAdapterAll:_disksListAdapterBest);
        updateInsets();
    };
	setTitleView(_segctrl);
	
	_disksListAdapterBest = new GamesListAdapter("http://www.ibeeb.co.uk/best.json", true);
	_disksListAdapterAll = new GamesListAdapter("http://www.ibeeb.co.uk/all.json", false);
    
    _disksListAdapterAll->getSectionTextFromItem = [=](const GameItem& item) -> string {
        char32_t ch = item._game->_title.charAt(0);
        if (ch>='a' && ch<='z') ch='A'+(ch-'a');
        if (!(ch>='A' && ch<='Z')) ch='#';
        return string((char*)&ch, 1);
    };
	
    _listView->_onItemTapDelegate = [=](View* itemView, LISTINDEX index) {
        GameItem& gameItem = _disksListAdapterBest->getItem(index);
        _delegate(gameItem._game);
        _navigationController->popViewController();
    };
    _listView->_onItemLongPressDelegate = [=](View* itemView, LISTINDEX index) {
    };
    
    _searchBox->setSearchTextChangedDelegate([=](SearchBox* searchBox, const string& text) {
        _disksListAdapterAll->setFilter([=](const GameItem& gameItem) -> int {
             const string title = gameItem.getTitle();
             if (title.hasPrefix(text, false)) {
                 return 2;
             }
             if (title.contains(text, false)) {
                 return 1;
             }
             return -1;
        });
    });
}


void GamesViewController::onWillAppear(bool firstTime) {
	_segctrl->setSelectedIndex(app.getIntSetting("disksSeg", 0));
    POINT pt;
    pt.x = 0;
    pt.y = app.getIntSetting("disksY", 0);
    //_listView->setContentOffset(pt);
}
void GamesViewController::onDidDisappear(bool lastTime) {
	app.setIntSetting("disksSeg", _segctrl->getSelectedIndex());
	app.setIntSetting("disksY", _listView->getContentOffset().y);
}

void GamesViewController::applySafeInsets(const EDGEINSETS &safeInsets) {
    _safeAreaInsets = safeInsets;
    updateInsets();
}

void GamesViewController::updateInsets() {
    int index = _segctrl->getSelectedIndex();
    _listView->setScrollInsets(EDGEINSETS(0, _safeAreaInsets.top + ((index==0)?0:app.dp(40)), 0, 0));
    _searchBox->getParent()->setAlignSpecs(ALIGNSPEC::Left(), ALIGNSPEC(NULL, 0.0f, 0.0f, _safeAreaInsets.top ));
}
