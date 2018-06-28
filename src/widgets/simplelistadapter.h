//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class SimpleListAdapter : public Object, public IListAdapter {
public:

    SimpleListAdapter(const string& itemLayoutId);

    // Overrides
    virtual void setListView(ListView* listView);
    virtual int getSectionCount();
    virtual float getHeaderHeight(int section);
    virtual int getItemCount(int section);
    virtual float getItemHeight(LISTINDEX index);
    virtual View* createItemView(LISTINDEX index);
    virtual void bindItemView(View*, LISTINDEX index, Object*);
    virtual View* createHeaderView(int section);
    virtual Object* getItem(LISTINDEX index);
    virtual bool canDeleteItem(LISTINDEX index);
    virtual void deleteItem(LISTINDEX index);

    // API
    virtual string getSectionTitle(int section);
    virtual void setFilter(const string& filterText);
    void invalidate();

protected:
    ListView* _adapterView;
    string _itemLayoutId;
    vector<ObjPtr<Object>> _items;
    vector<ObjPtr<Object>> _itemsFiltered;
    std::function<void(View*, LISTINDEX, Object*)> _itemViewBindFunc;
    string _filterText;
    
    int listIndexToRealIndex(LISTINDEX index);
};
