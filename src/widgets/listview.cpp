//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


DECLARE_DYNCREATE(ListView);

ListView::ListView() {
    applyStyle("ListView");
	_selectedIndex = LISTINDEX_NONE;
    _deleteConfirmIndex = LISTINDEX_NONE;
}

bool ListView::applySingleStyle(const string &name, const style& value) {
    if (name=="divider-height") {
        _dividerHeight = value.floatVal();
        return true;
    }
    if (name=="divider-color") {
        _dividerColor = value.colorVal();
        return true;
    }
    return View::applySingleStyle(name, value);
}

IListAdapter* ListView::getAdapter() const {
    return _adapter;
}

void ListView::setAdapter(IListAdapter* adapter) {
	_adapter = adapter;
    adapter->setListView(this);
    reload();
}

void ListView::setHeaderView(View *headerView) {
    if (_headerView == headerView) {
        return;
    }
    if (_headerView) {
        removeSubview(_headerView);
    }
    _headerView = headerView;
    if (_headerView) {
        addSubview(_headerView);
    }
    setNeedsLayout();
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

void ListView::layout(RECT constraint) {
    invalidateIntrinsicSize();
    View::layout(constraint);

    IListAdapter* adapter = _adapter;
	if (!adapter) {
		removeAllItemViews();
		return;
	}
    
    updateVisibleItems();
}

void ListView::updateIntrinsicSize(SIZE constrainingSize) {
	_intrinsicSize.width = constrainingSize.width;
	_intrinsicSize.height = _scrollInsets.top;
    IListAdapter* adapter = _adapter;
    _sectionMetrics.clear();
    if (_headerView) {
        _headerView->setLayoutOrigin(ALIGNSPEC::Top(), ALIGNSPEC(NULL, 0, 0, _intrinsicSize.height));
        _intrinsicSize.height += _headerView->getHeight();
    }
    if (adapter) {
        int numSections = adapter->getSectionCount();
        for (int j=0 ; j<numSections ; j++) {
            SECTION_METRICS sectionMetrics;
            sectionMetrics.top = _intrinsicSize.height;
            sectionMetrics.headerHeight = adapter->getHeaderHeight(j);
            sectionMetrics.totalHeight = sectionMetrics.headerHeight;
            size_t count = adapter->getItemCount(j);
            for (int i=0 ; i<count ; i++) {
                float itemHeight = adapter->getItemHeight(LISTINDEX_MAKE(j, i));
                sectionMetrics.totalHeight += itemHeight;
            }
            _intrinsicSize.height += sectionMetrics.totalHeight;
            _sectionMetrics.push_back(sectionMetrics);
        }
    }
    _intrinsicSize.height += _scrollInsets.bottom;
}


ListView::ItemView* ListView::indexToView(LISTINDEX index) {
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
            itemView->setBackgroundColor(app->getStyleColor("listview.selected-bkgnd-color"));
		}
	}
}


void ListView::onItemTap(View* itemView, LISTINDEX index) {
	if (_onItemTapDelegate) {
		_onItemTapDelegate(itemView, index);
	}
}

bool ListView::handleInputEvent(INPUTEVENT* event) {
    View::handleInputEvent(event);
	if (event->type == INPUT_EVENT_DOWN) {
		
        POINT pt = event->ptLocal;
        
        for (auto it = _itemViews.begin() ; it!= _itemViews.end() ; it++) {
            View* itemView = it->second;
            if (itemView->getRect().contains(pt)) {
                setSelectedIndex(it->first);
                break;
            }
        }
		return true;
		
	}
	if (event->type == INPUT_EVENT_DRAG) {
		setSelectedIndex(LISTINDEX_NONE);
		return true;
	}
	if (event->type == INPUT_EVENT_UP) {
		return true;
	}
	if (event->type == INPUT_EVENT_TAP) {
		if (_selectedIndex != LISTINDEX_NONE) {
			onItemTap(indexToView(_selectedIndex), _selectedIndex);
			setSelectedIndex(LISTINDEX_NONE);
		}
		return true;
	}
    if (event->type == INPUT_EVENT_LONG_PRESS) {
        if (_selectedIndex != LISTINDEX_NONE) {
            _onItemLongPressDelegate(indexToView(_selectedIndex), _selectedIndex);
            //setSelectedIndex(LISTINDEX_NONE);
        }
        return true;
    }

	return false;
}


void ListView::setContentOffset(POINT contentOffset, bool animated) {
    float dy = _contentOffset.y - contentOffset.y;
    View::setContentOffset(contentOffset, animated);
    if (dy) {
        updateVisibleItems();
    }
}


ListView::ItemView::ItemView(ListView* listView, LISTINDEX listIndex, View* contentView) {
    _listView = listView;
    _listIndex = listIndex;
    _contentView = contentView;
    contentView->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Fill());
    addSubview(contentView);
}

void ListView::ItemView::attachToWindow(Window *window) {
    View::attachToWindow(window);
    updateDeleteButton(false);
}
void ListView::ItemView::updateIntrinsicSize(SIZE constrainingSize) {
    
}

void ListView::ItemView::updateDeleteButton(bool animate) {
    if (_listView->_editMode && !_deleteButton) {
        float deleteButtonWidth = 16+45+16; // TODO: styles mofo, have you heard of them??
        _deleteButton = new ImageView();
        _deleteButton->setLayoutSize(MEASURESPEC::Abs(deleteButtonWidth), MEASURESPEC::Abs(8+45+8));
        _deleteButton->setPadding(EDGEINSETS(16, 8, 16, 8));
        _deleteButton->setLayoutOrigin(ALIGNSPEC(NULL, 0, animate?-1:0, 0), ALIGNSPEC::Center());
        if (!_listView->_bmpDelete) {
            bytearray data;
            app->loadAsset("images/delete.png", data); // TODO: need a different way to do platform assets
            Bitmap::createFromData(data, [=](Bitmap* bitmap) {
                _listView->_bmpDelete = bitmap;
                _deleteButton->setBitmap(bitmap);
            });
        } else {
            _deleteButton->setBitmap(_listView->_bmpDelete);
        }
        _deleteButton->onInputEvent = [&] (View* view, INPUTEVENT* event)  -> bool {
            if (event->type == INPUT_EVENT_TAP) {
                _listView->showDeleteConfirmButton(this);
            }
            return true;
        };
        addSubview(_deleteButton);
        
        LayoutAnimation::startHorizontal(_deleteButton, ALIGNSPEC(NULL,0,0,0), animate?300:0);
        LayoutAnimation::startHorizontal(_contentView, ALIGNSPEC(NULL,0,0,deleteButtonWidth), animate?300:0);
    }
    else if (!_listView->_editMode && _deleteButton) {
        removeSubview(_deleteButton);
        _deleteButton = NULL;
    }
}

void ListView::ItemView::showDeleteConfirmButton(bool show) {
    const int CONFIRM_BUTTON_WIDTH = 160;
    if (show) {
        if (!_deleteConfirmButton) {
            _deleteConfirmButton = new Label();
            _deleteConfirmButton->setBackgroundColor(0xFFf1453d);
            _deleteConfirmButton->setTextColor(0xFFFFFFFF);
            _deleteConfirmButton->setText("Delete");
            _deleteConfirmButton->setGravity({GRAVITY_CENTER, GRAVITY_CENTER});
            _deleteConfirmButton->setLayoutSize(MEASURESPEC::Abs(CONFIRM_BUTTON_WIDTH), MEASURESPEC::Fill());
            _deleteConfirmButton->setLayoutOrigin(ALIGNSPEC(NULL,1,0,0), ALIGNSPEC::Top());
            _deleteConfirmButton->onInputEvent = [&] (View* view, INPUTEVENT* event)  -> bool {
                if (event->type == INPUT_EVENT_TAP) {
                    _listView->deleteRow(_listIndex);
                }
                return true;
            };
            addSubview(_deleteConfirmButton);
        }
        LayoutAnimation::startHorizontal(_deleteButton, ALIGNSPEC(NULL,0,0,-CONFIRM_BUTTON_WIDTH-_deleteButton->getWidth()),300);
        LayoutAnimation::startHorizontal(_contentView, ALIGNSPEC(NULL,0,0,-CONFIRM_BUTTON_WIDTH),300);
        LayoutAnimation::startHorizontal(_deleteConfirmButton, ALIGNSPEC(NULL,1,-1,0),300);
    } else {
        LayoutAnimation::startHorizontal(_deleteButton, ALIGNSPEC(NULL,0,0,0),300);
        LayoutAnimation::startHorizontal(_contentView, ALIGNSPEC(NULL,0,0,_deleteButton->getWidth()),300);
        if (_deleteConfirmButton) {
            LayoutAnimation::startHorizontal(_deleteConfirmButton, ALIGNSPEC(NULL,1,0,0),300);
        }
    }
}

void ListView::deleteRow(LISTINDEX index) {
    ItemView* itemView = indexToView(index);
    if (itemView) {
        itemView->animateAlpha(0, 300);
    }
    
    // Every visible item below the deleted one must move up
    float dy = _adapter->getItemHeight(index);
    for (auto it : _itemViews) {
        if (it.first > index) {
            it.second->animateTranslate({0,-dy}, 300);
        }
    }
    
    // Remove item from data store
    Timer::start([=]() {
        _adapter->deleteItem(index);
        reload();
    }, 300, false);
}

pair<LISTINDEX,View*> ListView::createItemView(LISTINDEX index, bool atFront, float itemHeight, float top) {
    View* contentView = _adapter->createItemView(index);
    assert(contentView); // dude, where's my item content view??
    _adapter->bindItemView(contentView, index);
    ItemView* itemView = new ItemView(this, index, contentView);
    itemView->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Abs(itemHeight));
    insertSubview(itemView, (int)_itemViews.size());
    itemView->layout({0,0,_rect.size.width, itemHeight});
    RECT rect = itemView->getRect();
    rect.origin.y = top;
    itemView->setRectOrigin(rect.origin);
    pair<LISTINDEX,ItemView*> result(index,itemView);
    _itemViews.insert(atFront ? _itemViews.begin() : _itemViews.end(), result);
    RectRenderOp* dividerOp = new RectRenderOp();
    dividerOp->setFillColor(_dividerColor);
    RECT dividerRect = itemView->getOwnRect();
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
        float bottom = _contentOffset.y + _rect.size.height;
        float y = _scrollInsets.top;
        if (_headerView) {
            y += _headerView->getHeight();
        }
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
    float prevTop = item.second->getRect().origin.y;
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
    while (item.second->getRect().bottom() < _rect.bottom() + _contentOffset.y) {
        float prevBottom = item.second->getRect().bottom();
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
    float bottom = _contentOffset.y + _rect.size.height;
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
                headerView->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Abs(it->headerHeight));
                addSubview(headerView);
                headerView->layout(RECT(0,headerTop,_rect.size.width, it->headerHeight));
                pair<int,View*> result(section,headerView);
                headerViewIt = _headerViews.insert(headerViewIt, result);
            } else {
                headerViewIt->second->setRectOrigin({0,headerTop});
            }
            headerViewIt++;
        }
    }
}

void ListView::startEditing(ViewController* editingViewController) {
    assert(_adapter->getSectionCount()==1); // Oops! multisection data is not editable, yet.
    setSelectedIndex(LISTINDEX_NONE);
    _editingViewController = editingViewController;
    _editMode = true;
    for (auto it : _itemViews) {
        ItemView* itemView = it.second;
        itemView->updateDeleteButton(true);
    }
}

void ListView::showDeleteConfirmButton(ItemView* itemView) {
    if (_deleteConfirmIndex != -1) {
        ItemView* currentDeleteConfirmItemView = indexToView(_deleteConfirmIndex);
        if (currentDeleteConfirmItemView) {
            currentDeleteConfirmItemView->showDeleteConfirmButton(false);
        }
        // hide existing confirm button
    }
    _deleteConfirmIndex = itemView->_listIndex;
    itemView->showDeleteConfirmButton(true);
}

