//
//  emBeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#include "alldiskslistadapter.h"



AllDisksListAdapter::AllDisksListAdapter(string srcfile) : DisksListAdapter(srcfile, "layout/disk_listitem.res") {
}


int AllDisksListAdapter::getSectionCount() {
    if (_filterText.length()) return 1;
    return (int)_sections.size();
}
int AllDisksListAdapter::getItemCount(int section) {
    if (_filterText.length()) return (int)_itemsFiltered.size();
    auto it = _sections.at(section);
    return (int)it->second.size();
}
Object* AllDisksListAdapter::getItem(LISTINDEX index) {
    if (_filterText.length()) return _itemsFiltered.at(LISTINDEX_ITEM(index));
    return _sections.at(LISTINDEX_SECTION(index))->second.at(LISTINDEX_ITEM(index));
}


void AllDisksListAdapter::handleJson(const variant& json) {
    _sections.clear();
    _sectionMap.clear();
    DisksListAdapter::handleJson(json);
    for (auto git : _items) {
        DisksListItem* gameItem = (DisksListItem*)(Object*)git;
        char32_t ch = gameItem->_game->_title.charAt(0);
        if (ch>='a' && ch<='z') ch='A'+(ch-'a');
        if (!(ch>='A' && ch<='Z')) ch='#';
        
        map<char32_t, vector<DisksListItem*>>::iterator it = _sectionMap.find(ch);
        if (it == _sectionMap.end()) {
            vector<DisksListItem*> v;
            it = _sectionMap.insert({ch, v}).first;
            _sections.push_back(it);
        }
        it->second.push_back(gameItem);
    }
}


string AllDisksListAdapter::getSectionTitle(int section) {
    string str;
    str.append(_sections.at(section)->first);
    return str;
}

