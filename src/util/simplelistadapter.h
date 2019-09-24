//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

template <class ITEM>
class SimpleListAdapter : public Object, public IListAdapter {
public:

    // Function that returns the section identifier from the item. If not set then there are no sections.
    std::function<string(const ITEM& item)> getSectionTextFromItem;
    
    // Overrides
    virtual void setListView(ListView* listView) {
        _adapterView = listView;
    }
    int getSectionCount() override {
        if (_filtered) return 1;
        if (getSectionTextFromItem) {
            validateSections();
        }
        return MAX(1,_sections.size());
    }
    int getItemCount(int section) override {
        if (_filtered) return (int)_itemsFiltered.size();
        if (getSectionTextFromItem) {
            if (section >= _sections.size()) {
                return 0;
            }
            validateSections();
            auto& it = _sections.at(section);
            return (int)it.second.size();
        }
        assert(section<=0);
        return _items.size();
    }
    
    virtual float getHeaderHeight(int section) {
        if (_filtered) return 0;
        string title = getSectionTitle(section);
        return title.length() ? app->dp(30) : 0; // TODO: style
    }
    virtual float getItemHeight(LISTINDEX index) {
        return app->dp(68); // TODO: style
    }

    virtual View* createItemView(LISTINDEX index) =0;
    
    virtual ITEM& getItem(LISTINDEX index) {
        if (_filtered) {
            assert(LISTINDEX_SECTION(index)==0);
            return _items.at(_itemsFiltered.at(LISTINDEX_ITEM(index)).second);
        }
        return _items.at(LISTINDEX_ITEM(index));
    }
    virtual View* createHeaderView(int section) {
        Label* label = new Label();
        label->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
        label->setPadding(EDGEINSETS(app->dp(16),app->dp(4),app->dp(16),app->dp(4)));
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

    virtual string getSectionTitle(int section) {
        if (section >= _sections.size()) {
            return "";
        }
        return _sections[section].first;
    }
    
    /** Applies a filter to all items. The filter function must return an integer value
     where positive values mean the item is included and negative values means excluded.
     Higher positive values will be listed first. */
    virtual void setFilter(std::function<int(const ITEM&)> filterFunc) {
        _filtered = true;
        _itemsFiltered.clear();
        int i=0;
        for (auto& item : _items) {
            int score = filterFunc(item);
            if (score>0) {
                _itemsFiltered.push_back({score,i});
            }
            i++;
         }
        struct {
            bool operator() (const pair<int,int>& i, const pair<int,int>& j) {
                if (i.first > j.first) return true;
                if (i.first < j.first) return false;
                return i.second < j.second;
            }
        } comparator;
        std::sort(_itemsFiltered.begin(), _itemsFiltered.end(), comparator);        
        _adapterView->setContentOffset({0,0});
        _adapterView->reload();
    }
    void invalidate() {
        _sectionsValid = false;
        if (_adapterView) {
            _adapterView->setNeedsLayout();
        }
    }

protected:
    ListView* _adapterView;
    vector<ITEM> _items;
    vector<pair<string, vector<int>>> _sections;
    vector<pair<int,int>> _itemsFiltered;
    bool _filtered;
    bool _sectionsValid;
    
    int listIndexToRealIndex(LISTINDEX index) {
        int s = LISTINDEX_SECTION(index);
        int i = LISTINDEX_ITEM(index);
        while (--s >= 0) {
            i += getItemCount(s);
        }
        return i;
    }
    
    void validateSections() {
        _sectionsValid = true;
        _sections.clear();
        map<string, vector<pair<string, vector<int>>>::iterator> sectionLookup;
        for (int i=0 ; i<_items.size() ; i++) {
            auto& item = _items[i];
            string sectionText = getSectionTextFromItem(item);
            auto it = sectionLookup.find(sectionText);
            if (it != sectionLookup.end()) {
                auto& section = it->second;
                section->second.push_back(i);
            } else {
                vector<int> idiocy;
                idiocy.push_back(i);
                auto newLookup = _sections.insert(_sections.end(), std::make_pair(sectionText, idiocy));
                sectionLookup.insert(std::make_pair(sectionText, newLookup));
            }
            
        }
    }
};
