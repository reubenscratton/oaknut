//
//  diskslistadapter.cpp
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#include "diskslistadapter.h"


DiskItem::DiskItem(Game* game) : _game(game), _selectedDiskInfo(NULL) {
}
string DiskItem::getTitle() {
    if (!_selectedDiskInfo) {
        _selectedDiskInfo = _game->defaultDiskInfo();
    }
    return _game->_title;
}
string DiskItem::getSubtitle() {
    if (!_selectedDiskInfo) {
        _selectedDiskInfo = _game->defaultDiskInfo();
    }
    return _selectedDiskInfo->_publisher;
}
string DiskItem::getImageUrl() {
    if (!_selectedDiskInfo) {
        _selectedDiskInfo = _game->defaultDiskInfo();
    }
    return _selectedDiskInfo->imageUrl();
}



DisksListAdapter::DisksListAdapter(string srcfile) : SimpleListAdapter() {
    this->srcfile = srcfile;
        
    URLRequest::get(srcfile)->handleJson([&](URLRequest* req, const variant& json) {
        handleJson(json);
    });
}

void DisksListAdapter::bindItemView(View* itemview, LISTINDEX index) {
    DiskItemView* diskItemView = (DiskItemView*)itemview;
    DiskItem& diskItem = _items[listIndexToRealIndex(index)];
    diskItemView->bind(diskItem, LISTINDEX_ITEM(index)+1);
}
DisksListAdapter::~DisksListAdapter() {
}





//
void DisksListAdapter::handleJson(const variant& json) {
    _items.clear();
    if (json.type != variant::ARRAY) {
		app.log("Oops!");
	} else {
		auto& vals = json.arrayVal();
        for (auto& val : vals) {
			Game* game = new Game();
            game->fromVariant(val);
            _items.push_back(DiskItem(game));
		}
	}
	invalidate();
}



