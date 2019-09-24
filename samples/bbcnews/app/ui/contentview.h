//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include "../app.h"
#include "../model/_module.h"
//#import "BNCollections.h"
//#import "BNContentRequest.h"
//#import "BNViewPagerController.h"
#include "layout.h"
#include "../data/network/urlrequest.h"


class BNContentView : public View, public IBNModuleHolder { //BNViewPagerPage
public:
    
    BNContentView();
    void setContentStub(const BNContent::stub& contentStub, bool onlyShowPlaceholder=false);
    //- (void)recordAnalyticForPageView;

    float _topInset; // only set after construction

    void setCurrentLayout(BNLayout* layout);
    
    void attachToWindow(Window *window) override;
    void detachFromWindow() override;
    void layout(RECT constraint) override;
    
protected:
    BNContent::stub _contentStub;
    sp<BNContent> _contentObject;
    int _requestPriority;
    bool _isOnScreen;
    sp<View> _emptyView;
    sp<BNLayout> _layout;

    RECT _cachedFrame;
    sp<BNLayout> _currentLayout;

    
    vector<BNModule*> _modules;
    ProgressSpinner* _activityIndicatorView;
    sp<ImageView> _loadingImageView;
    //BNHUDView* _loadingHUD;
    
    //UIRefreshControl* refreshControl;
    bool _onlyShowPlaceholder, _hasARequestError;
    float _savedScrollOffset;
    bool _isInitialContent, _hasRecordedView;
    BNContent* _pendingContentObject;
    View* _scrollView;
    //BNViewPagerController* owningViewController;
    
    void presentEmptyView();
    
    // IBNModuleHolder
    void invalidateModuleSize(BNModule* module) override;

    void requestContent(bool request);
    BNURLRequest* _req;
    
    void updateWithNewContentObject(BNContent* content);
};
