//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(ListView);

ListView::ListView() {
    _dividerHeight = 1;//dp(1);
    _dividerColour = Styles::getColour("listview.selected-bkgnd-colour");
	_selectedIndex = LISTINDEX_NONE;
}

void ListView::setAdapter(IListAdapter* adapter) {
	_adapter = adapter;
    adapter->setListView(this);
    reload();
}

void ListView::removeAllItemViews() {
	while (_itemViews.size() > 0) {
        auto it = _itemViews.begin();
		View* view = it->second;
		removeSubview(view);
	}
    while (_headerViews.size() > 0) {
        auto it = _headerViews.begin();
        View* view = it->second;
        removeSubview(view);
    }
}

void ListView::reload() {
    removeAllItemViews();
    setNeedsLayout();
}

void ListView::measure(float parentWidth, float parentHeight) {
	
	invalidateContentSize();
	View::measure(parentWidth, parentHeight);

	IListAdapter* adapter = _adapter;
	if (!adapter) {
		removeAllItemViews();
		return;
	}
    
    updateVisibleItems();
}

void ListView::updateContentSize(float parentWidth, float parentHeight) {
	_contentSize.width = parentWidth;
	_contentSize.height = _scrollInsets.top;
    IListAdapter* adapter = _adapter;
    _sectionMetrics.clear();
    if (adapter) {
        int numSections = adapter->getSectionCount();
        for (int j=0 ; j<numSections ; j++) {
            SECTION_METRICS sectionMetrics;
            sectionMetrics.top = _contentSize.height;
            sectionMetrics.headerHeight = adapter->getHeaderHeight(j);
            sectionMetrics.totalHeight = sectionMetrics.headerHeight;
            size_t count = adapter->getItemCount(j);
            for (int i=0 ; i<count ; i++) {
                float itemHeight = adapter->getItemHeight(LISTINDEX_MAKE(j, i));
                sectionMetrics.totalHeight += itemHeight;
            }
            _contentSize.height += sectionMetrics.totalHeight;
            _sectionMetrics.push_back(sectionMetrics);
        }
    }
    _contentSize.height += _scrollInsets.bottom;
}


View* ListView::indexToView(LISTINDEX index) {
    for (auto it=_itemViews.begin() ; it != _itemViews.end() ; it++) {
        if (index == it->first) {
            return it->second;
        }
    }
    return NULL;
}

void ListView::setSelectedIndex(LISTINDEX index) {
	if (_selectedIndex != LISTINDEX_NONE) {
		View* itemView = indexToView(_selectedIndex);
		if (itemView) {
            itemView->setBackground(NULL);
		}
		_selectedIndex = LISTINDEX_NONE;
	}
	if (index != LISTINDEX_NONE) {
		_selectedIndex = index;
		View* itemView = indexToView(index);
		if (itemView) {
            // TODO: Am not wild about poking at item view backgrounds like this, perhaps
            // we need an ItemView type which has a "background overlay" renderop.
            itemView->setBackgroundColour(Styles::getColour("listview.selected-bkgnd-colour"));
		}
	}
}


void ListView::onItemTap(View* itemView, LISTINDEX index) {
	if (_onItemTapDelegate) {
		_onItemTapDelegate(itemView, index);
	}
}

bool ListView::onTouchEvent(int eventType, int finger, POINT pt) {
    View::onTouchEvent(eventType, finger, pt);
	if (eventType == INPUT_EVENT_DOWN) {
		
        pt.x += _contentOffset.x;
        pt.y += _contentOffset.y;
        
        for (auto it = _itemViews.begin() ; it!= _itemViews.end() ; it++) {
            View* itemView = it->second;
            if (itemView->_frame.contains(pt)) {
                setSelectedIndex(it->first);
                break;
            }
        }
		return true;
		
	}
	if (eventType == INPUT_EVENT_DRAG) {
		setSelectedIndex(LISTINDEX_NONE);
		return true;
	}
	if (eventType == INPUT_EVENT_UP) {
		return true;
	}
	if (eventType == INPUT_EVENT_TAP) {
		if (_selectedIndex != LISTINDEX_NONE) {
			onItemTap(indexToView(_selectedIndex), _selectedIndex);
			setSelectedIndex(LISTINDEX_NONE);
		}
		return true;
	}

	return false;
}


void ListView::setContentOffset(POINT contentOffset) {
    float dy = _contentOffset.y - contentOffset.y;
    View::setContentOffset(contentOffset);
    if (dy) {
        updateVisibleItems();
    }
}

pair<LISTINDEX,View*> ListView::createItemView(LISTINDEX index, bool atFront, float itemHeight, float top) {
    //app.log("creating index=%d", index);
    View* itemView = _adapter->createItemView(index);
    assert(itemView); // dude, where's my itemview??
    _adapter->bindItemView(itemView, index, _adapter->getItem(index));
    itemView->setMeasureSpecs(MEASURESPEC_FillParent, MEASURESPEC_Abs(itemHeight));
    insertSubview(itemView, (int)_itemViews.size());
    itemView->measure(_frame.size.width, itemHeight);
    itemView->layout();
    itemView->setFrameOrigin(POINT_Make(itemView->_frame.origin.x, top));
    pair<LISTINDEX,View*> result(index,itemView);
    _itemViews.insert(atFront ? _itemViews.begin() : _itemViews.end(), result);
    ColorRectFillRenderOp* dividerOp = new ColorRectFillRenderOp(itemView);
    dividerOp->setColour(_dividerColour);
    RECT dividerRect = itemView->getBounds();
    dividerRect.origin.y = dividerRect.bottom() - _dividerHeight;
    dividerRect.size.height = _dividerHeight;
    dividerOp->_rect = dividerRect;
    itemView->addRenderOp(dividerOp);
    return result;
 }


LISTINDEX ListView::offsetIndex(LISTINDEX index, int offset) {
    int32_t s = LISTINDEX_SECTION(index);
    int32_t i = LISTINDEX_ITEM(index);
    int32_t sc = _adapter->getSectionCount();
    i += offset;
    while (i<0 && s>0) {
        i += _adapter->getItemCount(--s);
    }
    while (s<sc && i >= _adapter->getItemCount(s)) {
        i -= _adapter->getItemCount(s++);
    }
    if (i<0 || s>=sc) return LISTINDEX_NONE;
    return LISTINDEX_MAKE(s, i);
}

void ListView::removeSubview(View *subview) {
    View::removeSubview(subview);
    for (auto it=_itemViews.begin() ; it!=_itemViews.end() ; it++) {
        if (it->second == subview) {
            _itemViews.erase(it);
            return;
        }
    }
    for (auto it=_headerViews.begin() ; it!=_headerViews.end() ; it++) {
        if (it->second == subview) {
            _headerViews.erase(it);
            break;
        }
    }
}

void ListView::updateVisibleItems() {
    int sectionCount = _adapter->getSectionCount();

    // Remove items that have been scrolled out of sight
    removeSubviewsNotInVisibleArea();

    // If there are no visible items then we need to repopulate from scratch which means
    // walking the items list to find the first visible item
    pair<LISTINDEX,View*> item = {0,NULL};
    if (_itemViews.size() == 0) {
        float bottom = _contentOffset.y + _frame.size.height;
        float y = _scrollInsets.top;
        for (int s=0 ; s<sectionCount && !item.second ; s++) {
            for (int i=0 ; i<_adapter->getItemCount(s) ; i++) {
                LISTINDEX index = LISTINDEX_MAKE(s,i);
                if (i==0) {
                    y += _adapter->getHeaderHeight(s);
                }
                float itemHeight = _adapter->getItemHeight(index);
                if ((y >= _contentOffset.y && y<bottom) || ((y+itemHeight)>=_contentOffset.y && (y+itemHeight) < bottom)) {
                    item = createItemView(index, false, itemHeight, y);
                    break;
                }
                y += itemHeight;
            }
        }
    }
    
    // List is empty, nothing more to do
    if (_itemViews.size() == 0) {
        return;
    }
    
    // Fill the listview upwards from the current top itemview
    item = _itemViews.at(0);
    float prevTop = item.second->_frame.origin.y;
    while (prevTop > _contentOffset.y && item.first>0) {
        LISTINDEX newIndex = offsetIndex(item.first, -1);
        if (newIndex == LISTINDEX_NONE) break;
        if (LISTINDEX_ITEM(item.first)==0) { // first in section
            prevTop -= _adapter->getHeaderHeight(LISTINDEX_SECTION(item.first));
        }
        float itemHeight = _adapter->getItemHeight(newIndex);
        prevTop -= itemHeight;
        item = createItemView(newIndex, true, itemHeight, prevTop);
    }
    
    // Fill listview downwards from the current bottom itemview
    item = _itemViews.at(_itemViews.size()-1);
    while (item.second->_frame.bottom() < _frame.bottom() + _contentOffset.y) {
        float prevBottom = item.second->_frame.bottom();
        LISTINDEX newIndex = offsetIndex(item.first, 1);
        if (newIndex == LISTINDEX_NONE) break;
        if (LISTINDEX_ITEM(newIndex)==0) { // first in section
            prevBottom += _adapter->getHeaderHeight(LISTINDEX_SECTION(newIndex));
        }
        float itemHeight = _adapter->getItemHeight(newIndex);
        item = createItemView(newIndex, false, itemHeight, prevBottom);
    }
    
    // Ensure all header items present. The header for the topmost item floats and is always present.
    auto headerViewIt = _headerViews.begin();
    float floatingHeaderAnchorY = _contentOffset.y  + _scrollInsets.top;
    float bottom = _contentOffset.y + _frame.size.height;
    int section = LISTINDEX_SECTION(_itemViews.begin()->first);
    for (auto it = _sectionMetrics.begin() + section ; it!=_sectionMetrics.end() ; it++, section++) {
        if (it->headerHeight <= 0) {
            continue;
        }
        
        // If at least part of the section is visible
        if (bottom >= it->top || _contentOffset.y >= (it->top+it->totalHeight)) {
            float headerTop;
            if (it->top > floatingHeaderAnchorY) {
                headerTop = it->top;
            } else {
                headerTop = fminf(it->top+it->totalHeight-it->headerHeight, floatingHeaderAnchorY);
            }
            if (headerViewIt==_headerViews.end() || section != headerViewIt->first) {
                View* headerView = _adapter->createHeaderView(section);
                assert(headerView);
                headerView->setMeasureSpecs(MEASURESPEC_FillParent, MEASURESPEC_Abs(it->headerHeight));
                addSubview(headerView);
                headerView->measure(_frame.size.width, it->headerHeight);
                headerView->layout();
                headerView->setFrameOrigin(POINT_Make(headerView->_frame.origin.x, headerTop));
                pair<int,View*> result(section,headerView);
                headerViewIt = _headerViews.insert(headerViewIt, result);
            } else {
                headerViewIt->second->setFrameOrigin(POINT_Make(0,headerTop));
            }
            headerViewIt++;
        }
    }
}



