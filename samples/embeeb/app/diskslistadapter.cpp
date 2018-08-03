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
	URLRequest::request(srcfile, this, 0);

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
    URLRequest::unrequest(srcfile, this);
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




//	IURLRequestDelegate
void DisksListAdapter::onUrlRequestLoad(URLData* data) {
    _items.clear();
	StringProcessor it(data->_value.data->toString(false));
	JsonValue val = JsonValue::parse(it, 0);
	if (val._type != jsonArray) {
		app.log("Oops!");
	} else {
		JsonArray* json = val._arrayVal;
		for (int i=0 ; i<json->_elements.size() ; i++) {
			Game* game = json->getObject<Game>(i);
            _items.push_back(new DisksListItem(game));
		}
	}
	invalidate();
}



