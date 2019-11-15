//
// Copyright © 2019 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#pragma once
#include <oaknut.h>
#include "ui/common/layoutsgroup.h"



class BNViewPagerController : public ViewController {
public:
    
    ViewPager* _viewPager;

    BNViewPagerController();
    void invalidateNav();
    void updateNav();
    bool navigateToUrl(const string& url, bool animated);
    bool isRootVC();
    void onCannotRenderContentPleaseShowInBrowserKThxBai(const string& contentUrl);
};

class BNContentViewPagerController : public BNViewPagerController {
public:
    // extern BOOL globalSwipedViewPager;

    vector<BNContent::stub> _contentArray;
    // View* _editButton;

    void setContent(BNContent::stub stub, vector<BNContent::stub>& collection);
    // - (void)onFollowStateChanged:(NSNotification*)notification;
    // - (void)setSelectedModelId:(NSString*)modelId;
    // - (NSString*)selectedModelId;

};


class BNNavController : public NavigationController {
public:
    
    enum OpenFlags {
        None = 0,
        DestroyBackStack = 1,
        ShowAddTopicTip = 2,
        NoAnimation = 4
    };

    void openPhotoGallery(vector<BNImage*>& images, BNImage* currentImage);
    void openItem(BNContent* item, const vector<BNBaseModel*>& withinCollection, const string& title);
    void openURL(const string& url, const vector<BNBaseModel*>& withinCollection, const string& title, OpenFlags flags=None);
    
};

class BBCNewsApp : public App {
public:
    
    void main() override;

    BNLayout* layoutWithContent(const string& modelId,
                                const string& site,
                                const string& format,
                                const string& orientation);

    void setCompactMode(bool compactMode);
    void setCarouselsMode(bool carouselsMode);


    void loadLayouts();
    
    BNLayoutsGroup* _currentLayouts;
    bool _compactMode;
    bool _carouselsMode;
    bool _hasEverUsedCarouselsMode;
};





