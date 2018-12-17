//
//  emBeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#include "gameitem.h"




GameItem::GameItem(Game* game) : _game(game), _selectedDiskInfo(NULL) {
    _selectedDiskInfo = _game->defaultDiskInfo();
}
string GameItem::getTitle() const {
    return _game->_title;
}
string GameItem::getSubtitle() const {
    return _selectedDiskInfo->_publisher;
}
string GameItem::getImageUrl() const {
    return _selectedDiskInfo->imageUrl();
}





