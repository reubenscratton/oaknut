//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

template <class ITEM, class ITEMVIEW>
class SimpleListAdapter : public Object, public IListAdapter {
public:

    // Overrides
    virtual void setListView(ListView* listView) {
        _adapterView = listView;
    }
    virtual int getSectionCount() {
        return 1;
    }
    virtual float getHeaderHeight(int section) {
        if (_filterText.length()) return 0;
        string title = getSectionTitle(section);
        return title.length() ? app.dp(30) : 0;
    }
    virtual int getItemCount(int section) {
        assert(section<=0);
        return (int)_items.size();
    }
    virtual float getItemHeight(LISTINDEX index) {
        return app.dp(64);
    }

    virtual View* createItemView(LISTINDEX index) {
        return (View*)new ITEMVIEW(); //app.layoutInflate(_itemLayoutId);
    }
    
    virtual ITEM& getItem(LISTINDEX index) {
        return _items.at(LISTINDEX_ITEM(index));
    }
    /*virtual void bindItemView(View* itemView, LISTINDEX index, ITEM* item) {
        //assert(LISTINDEX_SECTION(index)==0);
        //Item* item = (_filterText.length()) ? _itemsFiltered.at(LISTINDEX_ITEM(index)) :
        //     _items.at(LISTINDEX_ITEM(index));
        if (_itemViewBindFunc) {
            _itemViewBindFunc(itemView, index, item);
        } else {
            assert(0); // need an item view-binding function!
        }
    }*/
    virtual View* createHeaderView(int section) {
        Label* label = new Label();
        label->setMeasureSpecs(MEASURESPEC::FillParent(), MEASURESPEC::WrapContent());
        label->setPadding(EDGEINSETS(app.dp(16),app.dp(4),app.dp(16),app.dp(4)));
        label->setText(getSectionTitle(section));
        label->setBackgroundColor(0xFFeeeeee); // TODO: style
        return label;
    }
    virtual bool canDeleteItem(LISTINDEX index) {
        return true;
    }
    virtual void deleteItem(LISTINDEX index) {
        int realIndex = listIndexToRealIndex(index);
        _items.erase(_items.begin()+realIndex);
        invalidate();
    }

    // API
    virtual string getSectionTitle(int section) {
        return "";
    }
    virtual void setFilter(const string& filterText) {
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
         _adapterView->setContentOffset({0,0});
         _adapterView->reload();*/
    }
    void invalidate() {
        if (_adapterView) {
            _adapterView->setNeedsLayout();
        }
    }

protected:
    ListView* _adapterView;
    vector<ITEM> _items;
    vector<LISTINDEX> _itemsFiltered;
    string _filterText;
    
    int listIndexToRealIndex(LISTINDEX index) {
        int s = LISTINDEX_SECTION(index);
        int i = LISTINDEX_ITEM(index);
        while (--s >= 0) {
            i += getItemCount(s);
        }
        return i;
    }
};
