//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

typedef int64_t LISTINDEX;

#define LISTINDEX_NONE ((int64_t)-1)
#define LISTINDEX_SECTION(i) ((i)>>32)
#define LISTINDEX_ITEM(i) ((int32_t)((i)&0xFFFFFFFF))
#define LISTINDEX_MAKE(s,i) ((((int64_t)s)<<32)|(i))

class IListAdapter {
public:
    virtual void setListView(class ListView* listView) =0;
    virtual int getSectionCount() = 0;
    virtual float getHeaderHeight(int section) = 0;
	virtual int getItemCount(int section) = 0;
	virtual float getItemHeight(LISTINDEX index) = 0;
	virtual View* createItemView(LISTINDEX index) = 0;
    virtual View* createHeaderView(int section) = 0;
	virtual Object* getItem(LISTINDEX index) = 0;
    virtual void bindItemView(View*, Object*) = 0;
};

class ListView : public View {
public:
	IListAdapter* _adapter;
    vector<pair<LISTINDEX,View*>> _itemViews;
    vector<pair<int,View*>> _headerViews;
	float _dividerHeight;
    COLOUR _dividerColour;
    LISTINDEX _selectedIndex;
    std::function<void(View*, LISTINDEX index)> _onItemTapDelegate;
    typedef struct {
        int top;
        int headerHeight;
        int totalHeight;
    } SECTION_METRICS;
    vector<SECTION_METRICS> _sectionMetrics;
	
	// Public API
    ListView();
	virtual void setAdapter(IListAdapter* adapter);
	virtual void removeAllItemViews();
	virtual void onItemTap(View* itemView, LISTINDEX index);
    virtual void reload();
    
	// Overrides
	virtual void measure(float parentWidth, float parentHeight);
	virtual void updateContentSize(float parentWidth, float parentHeight);
    virtual void setContentOffset(POINT contentOffset);
	virtual bool onTouchEvent(int eventType, int finger, POINT pt);
    virtual void removeSubview(View* subview);
	
	
protected:
	virtual View* indexToView(LISTINDEX index);
	virtual void setSelectedIndex(LISTINDEX index);
    virtual void updateVisibleItems();
    virtual pair<LISTINDEX,View*> createItemView(LISTINDEX index, bool atFront, float itemHeight, float top);
    LISTINDEX offsetIndex(LISTINDEX index, int offset);
};



class SimpleListAdapter : public Object, public IListAdapter {
public:
    class Item : public Object {
    public:
        virtual string getTitle() = 0;
        virtual string getSubtitle() = 0;
        virtual string getImageUrl() = 0;
    };
    
    class ItemView : public View {
    public:
        ImageView* _imageView;
        Label* _titleLabel;
        Label* _subtitleLabel;
        
        ItemView();
    };
    
    
    // API
    virtual void addItem(Item* item);
    virtual string getSectionTitle(int section);
    virtual void setFilter(const string& filterText);
    
    // Overrides
    virtual void setListView(ListView* listView);
    virtual int getSectionCount();
    virtual float getHeaderHeight(int section);
    virtual int getItemCount(int section);
    virtual float getItemHeight(LISTINDEX index);
    virtual View* createItemView(LISTINDEX index);
    virtual View* createHeaderView(int section);
    virtual void bindItemView(ItemView* itemView, Object* item);
    virtual Object* getItem(LISTINDEX index);

protected:
    ListView* _adapterView;
    vector<ObjPtr<Item>> _items;
    vector<ObjPtr<Item>> _itemsFiltered;
    string _filterText;
};

