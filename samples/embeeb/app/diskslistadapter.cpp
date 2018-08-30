//
//  diskslistadapter.cpp
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#include "diskslistadapter.h"


DisksListAdapter::DisksListAdapter(string srcfile,const string& itemLayoutId) 
	: SimpleListAdapter(itemLayoutId) {
    this->srcfile = srcfile;
        
    URLRequest::get(srcfile)->handleJson([&](URLRequest* req, const variant& json) {
        handleJson(json);
    });

	_itemViewBindFunc = [=](View* view, LISTINDEX index, Object* item) {
		DisksListItem* disksListItem = (DisksListItem*)item;
        Label* numberLabel = (Label*)view->findViewById("number");
        if (numberLabel) {
            numberLabel->setText(string::format("%d.", LISTINDEX_ITEM(index)+1));
        }
        ImageView* imageView = (ImageView*)view->findViewById("image");
        imageView->setImageUrl(disksListItem->getImageUrl());
		Label* titleLabel = (Label*)view->findViewById("title");
		titleLabel->setText(disksListItem->getTitle());
		Label* subtitleLabel = (Label*)view->findViewById("subtitle");
		subtitleLabel->setText(disksListItem->getSubtitle());
	};
}
DisksListAdapter::~DisksListAdapter() {
}
DisksListItem::DisksListItem(Game* game) : _game(game) {
}
string DisksListItem::getTitle() {
	if (!_selectedDiskInfo) {
		_selectedDiskInfo = _game->defaultDiskInfo();
	}
	return _game->_title;
}
string DisksListItem::getSubtitle() {
	if (!_selectedDiskInfo) {
		_selectedDiskInfo = _game->defaultDiskInfo();
	}
	return _selectedDiskInfo->_publisher;
}
string DisksListItem::getImageUrl() {
	if (!_selectedDiskInfo) {
		_selectedDiskInfo = _game->defaultDiskInfo();
	}
    return _selectedDiskInfo->imageUrl();
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
            _items.push_back(new DisksListItem(game));
		}
	}
	invalidate();
}



