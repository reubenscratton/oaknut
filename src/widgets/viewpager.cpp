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
        updateScrollViewContentSize();
        
        _pageCenter = _adapter->viewForPage(1);
        _pageCenter->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Fill());
        _pageCenter->setLayoutOrigin({NULL,0,0,0}, {NULL,0,0,0});
        addSubview(_pageCenter);

        _pageRight = _adapter->viewForPage(0);
        //_pageRight->setBackgroundColor(0xFFFF8080);
        _pageRight->setLayoutSize(MEASURESPEC::Match(this), MEASURESPEC::Fill());
        _pageRight->setLayoutOrigin({_pageCenter,1,0,0}, {NULL,0,0,0});
        addSubview(_pageRight);

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
    invalidateIntrinsicSize();
}

void ViewPager::updateIntrinsicSize(SIZE constrainingSize) {
    _intrinsicSize.width = constrainingSize.width * _adapter->numberOfPages();
    _intrinsicSize.height = constrainingSize.height;
    // setContentOffset(POINT(_selectedIndex * constrainingSize.width, 0), animated:NO];
}
