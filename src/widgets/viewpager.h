//
// Copyright © 2019 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class ViewPager : public View {
public:
    
    // Must be implemented by whatever provides pages
    class Adapter : public Object {
    public:
        virtual uint32_t numberOfPages()=0;
        virtual View* viewForPage(uint32_t page)=0;
        virtual string titleForPage(uint32_t page)=0;
        virtual string keyForPage(uint32_t page)=0;
        virtual string urlForPage(uint32_t page)=0;
    };
    
    ViewPager();
    Adapter* getAdapter() const;
    void setAdapter(Adapter* adapter);
    void setCurrentPage(int32_t pageIndex, bool animated);


    void onPageInserted(uint32_t index);
    void onPageRemoved(uint32_t index);
    void onPageMoved(uint32_t indexTo, uint32_t indexFrom);

    void onViewDidAppear(bool viewControllerIsMovingToParent);
    void onViewDidDisappear(bool viewControllerIsMovingFromParent);

    void layout(RECT constraint) override;
    void setContentOffset(POINT contentOffset, bool animated=false) override;
    void updateIntrinsicSize(SIZE constrainingSize) override;

protected:
    void updateScrollViewContentSize();
    sp<Adapter> _adapter;
    void ensureFillRectFilled();
    
    //Delegate* _delegate;
    //int32_t _currentPageIndex;
    int32_t _pendingCurrentPageIndex;
    //TODO savedPageState;
    
    map<uint32_t, View*> _loadedPageViews;


};
