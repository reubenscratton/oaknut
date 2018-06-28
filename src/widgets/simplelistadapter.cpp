//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


SimpleListAdapter::SimpleListAdapter(const string& itemLayoutId) {
    _itemLayoutId = itemLayoutId;
}
void SimpleListAdapter::setListView(ListView* listView) {
    _adapterView = listView;
}
int SimpleListAdapter::getSectionCount() {
    return 1;
}
float SimpleListAdapter::getHeaderHeight(int section) {
    if (_filterText.length()) return 0;
    string title = getSectionTitle(section);
    return title.length() ? app.dp(30) : 0;
}
string SimpleListAdapter::getSectionTitle(int section) {
    return "";
}
int SimpleListAdapter::getItemCount(int section) {
    assert(section<=0);
    return (int)_items.size();
}
float SimpleListAdapter::getItemHeight(LISTINDEX index) {
    return app.dp(64);
}
View* SimpleListAdapter::createItemView(LISTINDEX index) {
    return app.layoutInflate(_itemLayoutId);
}

View* SimpleListAdapter::createHeaderView(int section) {
    Label* label = new Label();
    label->setMeasureSpecs(MEASURESPEC::FillParent(), MEASURESPEC::WrapContent());
    label->setPadding(EDGEINSETS(app.dp(16),app.dp(4),app.dp(16),app.dp(4)));
    label->setText(getSectionTitle(section));
    label->setBackgroundColour(0xFFeeeeee);
    return label;
}
int SimpleListAdapter::listIndexToRealIndex(LISTINDEX index) {
    int s = LISTINDEX_SECTION(index);
    int i = LISTINDEX_ITEM(index);
    while (--s >= 0) {
        i += getItemCount(s);
    }
    return i;
}
Object* SimpleListAdapter::getItem(LISTINDEX index) {
    return _items[listIndexToRealIndex(index)];
}

void SimpleListAdapter::invalidate() {
    if (_adapterView) {
        _adapterView->setNeedsLayout();
    }
}


void SimpleListAdapter::bindItemView(View* itemView, LISTINDEX index, Object* item) {
    //assert(LISTINDEX_SECTION(index)==0);
    //Item* item = (_filterText.length()) ? _itemsFiltered.at(LISTINDEX_ITEM(index)) :
    //     _items.at(LISTINDEX_ITEM(index));
    if (_itemViewBindFunc) {
        _itemViewBindFunc(itemView, index, item);
    } else {
        assert(0); // need an item view-binding function!
    }
}


bool SimpleListAdapter::canDeleteItem(LISTINDEX index) {
    return true;
}
void SimpleListAdapter::deleteItem(LISTINDEX index) {
    int realIndex = listIndexToRealIndex(index);
    _items.erase(_items.begin()+realIndex);
    invalidate();
}

void SimpleListAdapter::setFilter(const string& filterText) {
    app.log("TODO! filter");
    /*_filterText = filterText;
    _itemsFiltered.clear();
    vector<ObjPtr<Item>> secondaryMatches;
    for (auto i : _items) {
        const string title = i->getTitle();
        if (stringStartsWith(title, filterText, false)) {
            _itemsFiltered.push_back(i);
        } else if (title.find(filterText) < title.length()) {
            secondaryMatches.push_back(i);
        }
    }
    _itemsFiltered.insert(_itemsFiltered.end(), secondaryMatches.begin(), secondaryMatches.end());
    _adapterView->setContentOffset(POINT_Make(0,0));
    _adapterView->reload();*/
}



