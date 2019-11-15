//
// Copyright © 2019 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

DECLARE_DYNCREATE(ViewPager);

ViewPager::ViewPager() : View() {
}

ViewPager::Adapter* ViewPager::getAdapter() const {
    return _adapter;
}

void ViewPager::setAdapter(Adapter* adapter) {
    if (adapter != _adapter) {
        _adapter = adapter;
        removeAllSubviews();
        updateScrollViewContentSize();
        
        _pageCenter = _adapter->viewForPage(0);
        _pageCenter->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Fill());
        _pageCenter->setLayoutOrigin({NULL,0,0,0}, {NULL,0,0,0});
        addSubview(_pageCenter);
    }
}

/*void ViewPager::layoutSubviews(RECT constraint) {
    float w = constraint.size.width;
    float h =constraint.size.height;
    // _pageLeft->setLayoutSize(MEASURESPEC::Abs(w),MEASURESPEC::Abs(h));
    // = CGRectMake((self.centerIndex-1) * w, self.pageLeft.frame.origin.y, w, h);
    // _pageCenter.frame = CGRectMake((self.centerIndex+0) * w, self.pageCenter.frame.origin.y, w, h);
    // _pageRight.frame = CGRectMake((self.centerIndex+1) * w, self.pageRight.frame.origin.y, w, h);
}*/

void ViewPager::setSelectedIndex(uint32_t selectedIndex, bool animated) {
    if (_selectedIndex == selectedIndex) {
        return;
    }
    _selectedIndex = selectedIndex;
    setContentOffset({selectedIndex * _rect.size.width, 0}, animated);
    //if (_delegate) {
    //    _delegate->onPageSelected(selectedIndex);
    //}
}
 

void ViewPager::updateScrollViewContentSize() {
    if (!_adapter) {
        return;
    }
    invalidateContentSize();
}

void ViewPager::updateContentSize(SIZE constrainingSize) {
    _contentSize.width = constrainingSize.width * _adapter->numberOfPages();
    _contentSize.height = constrainingSize.height;
    // setContentOffset(POINT(_selectedIndex * constrainingSize.width, 0), animated:NO];
}
