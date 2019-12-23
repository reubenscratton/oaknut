//
// Copyright © 2019 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(ViewPager);

ViewPager::ViewPager() : View() {
    _directionalLockEnabled = true;
}

ViewPager::Adapter* ViewPager::getAdapter() const {
    return _adapter;
}

void ViewPager::setAdapter(Adapter* adapter) {
    if (adapter != _adapter) {
        _adapter = adapter;
        removeAllSubviews();
        _loadedPageViews.clear();
        updateScrollViewContentSize();
        //_currentPageIndex = -1;
        ensureFillRectFilled();
    }
}

void ViewPager::setContentOffset(POINT contentOffset, bool animated/*=false*/) {
    POINT d = _contentOffset - contentOffset;
    if (d.isZero()) {
        return;
    }
    int currentPageIndex = (_contentOffset.x / (float)_rect.size.width);
    int newPageIndex = (contentOffset.x / (float)_rect.size.width);
    View::setContentOffset(contentOffset, animated);
    if (currentPageIndex != newPageIndex) {
        ensureFillRectFilled();
    }
}

void ViewPager::layout(RECT constraint) {
    View::layout(constraint);
    
    ensureFillRectFilled();
}

void ViewPager::ensureFillRectFilled() {
    
    // Calculate the 'fill' rect, i.e. the client area that must be filled with pages
    RECT fillRect = getVisibleRect();
    fillRect.origin.x -= fillRect.size.width;
    fillRect.size.width *= 3;
    if (fillRect.isEmpty()) {
        return;
    }

    // Remove any pages that don't intersect the fill rect
    for (auto it = _loadedPageViews.begin() ; it!=_loadedPageViews.end() ; it++) {
        if (!it->second->getRect().intersects(fillRect)) {
            it->second->removeFromParent();
            it = _loadedPageViews.erase(it);
            if (it == _loadedPageViews.end()) {
                break;
            }
        }
    }

    // Load any missing pages, starting with the center one (highest priority), then immediate right,
    // then immediate left, etc.
    uint32_t index = _contentOffset.x / _rect.size.width;
    RECT pageRect = getVisibleRect();
    while (index < _adapter->numberOfPages()) {
        pageRect.origin.x = index * _rect.size.width;
        if (!pageRect.intersects(fillRect)) {
            break;
        }
        if (_loadedPageViews.find(index) == _loadedPageViews.end()) {
            View* page = _adapter->viewForPage(index);
            page->setLayoutSize(MEASURESPEC::Match(this), MEASURESPEC::Fill());
            page->setLayoutOrigin({NULL,0,0,_rect.size.width * index}, {NULL,0,0,0});
            addSubview(page);
            _loadedPageViews.insert(make_pair(index, page));
        }
        index++;
    }
    
    
}

void ViewPager::setCurrentPage(int32_t pageIndex, bool animated) {
    setContentOffset({pageIndex * _rect.size.width, 0}, animated);
}
 

void ViewPager::updateScrollViewContentSize() {
    if (!_adapter) {
        return;
    }
    invalidateIntrinsicSize();
}

void ViewPager::updateIntrinsicSize(SIZE constrainingSize) {
    _intrinsicSize.width = constrainingSize.width * _adapter->numberOfPages();
    _intrinsicSize.height = constrainingSize.height;
    // setContentOffset(POINT(_selectedIndex * constrainingSize.width, 0), animated:NO];
}
