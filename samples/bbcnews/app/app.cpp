//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "app.h"
#include "model/_module.h"
#include "policy/policy.h"
#include "ui/common/contentview.h"

static string URLBASE_CONTENT = "https://trevor-producer-cdn.api.bbci.co.uk/content";

static BBCNewsApp the_app;

BNViewPagerController::BNViewPagerController() {
    _view = _viewPager = new ViewPager();
    _view->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Fill());
}

void BNViewPagerController::applySafeInsets(const EDGEINSETS& insets) {
    _view->setPadding(insets);
    _view->setScrollInsets(insets);
}

bool BNViewPagerController::navigateToUrl(const string& url, bool animated) {

    // Find a URL that matches
    auto adapter = _viewPager->getAdapter();
    for (auto i=0 ; i<adapter->numberOfPages() ; i++) {
        string pageUrl = adapter->urlForPage(i);
        
        // Don't care about querystring etc matches...
        if (url.split("?")[0] == pageUrl.split("?")[0]) {
            // Reset page state (i.e. scroll-to-top)
            // TODO _viewPager.savedPageState removeObjectForKey:[self.viewPager.adapter keyForPage:i]];
            _viewPager->setCurrentPage(i, animated);
            /*if (anim) {
                [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(scrollAnimFinished) name:BNNotificationViewPagerScrollAnimationFinished  object:nil];
            }*/
            
            return true;
        }
    }
    
    return false;
}

class BNContentAdapter : public ViewPager::Adapter {
public:
    vector<BNContent::stub>& _array;

    BNContentAdapter(vector<BNContent::stub>& array) : _array(array) {
        
    }
    uint32_t numberOfPages() override {
        return (uint32_t)_array.size();
    }
    View* viewForPage(uint32_t page) override {
        BNContentView* view = new BNContentView();
        view->setContentStub(_array[page]);
        return view;
    }
    string titleForPage(uint32_t page) override {
        return "Todo: title";
    }
    string keyForPage(uint32_t page) override {
        return _array[page].modelId;
    }
    string urlForPage(uint32_t page) override {
        return _array[page].url();
    }

};

void BNContentViewPagerController::setContent(BNContent::stub stub, vector<BNContent::stub>& collection) {
    _contentArray = collection;
    _viewPager->setAdapter(new BNContentAdapter(_contentArray));
    auto it = std::find(_contentArray.begin(), _contentArray.end(), stub);
    int32_t index = (int32_t)std::distance(_contentArray.begin(), it);
    _viewPager->setCurrentPage(index, false);
}

class BNPhotosViewPagerController : public ViewController {
public:
    BNPhotosViewPagerController(vector<BNImage*>& images, BNImage* currentImage) {
        
    }
};

#define LAYOUT_SETTING_KEY "layoutDir2"

void BNNavController::openPhotoGallery(vector<BNImage*>& images, BNImage* currentImage) {
    BNPhotosViewPagerController* pagingVC = new BNPhotosViewPagerController(images, currentImage);
    pushViewController(pagingVC);
}

void BNNavController::openItem(BNContent* item, const vector<BNBaseModel*>& withinCollection, const string& title) {
    if (item->_modelType == BNModelTypeLiveEventExt) {
        //presentViewController:[[BBCNWebBrowser alloc] initWithURL:[NSURL URLWithString:item.shareUrl]] animated:YES completion:nil];
        app->warn("TODO: integrate web browser nav");
        return;
    }
    openURL(item->url(), withinCollection, title);
}

void BNNavController::openURL(const string& url, const vector<BNBaseModel*>& collection, const string& title, OpenFlags flags/*=None*/) {
    
    // Open a BBC News content item...
    vector<string> urlparts = url.split(":", 1);
    if (urlparts.size() < 2 || urlparts[0].hasPrefix("bbcnewsapp")) {

        auto stub = BNContent::stub::fromURL(url);
        
        bool animation= false;
        // If destroying back stack, do that now
        if (flags & DestroyBackStack) {
            animation = (flags & NoAnimation)==0;
            while (_navStack.size() > 1) {
                popViewController(); // todo: add animation flag
            }
        }
    
        // If content is in the current top paging vc, show it and exit. No nav history gets created.
        auto pagingVC = dynamic_cast<BNContentViewPagerController*>(_currentViewController._obj);
        if (pagingVC) {
            if (pagingVC->navigateToUrl(url, animation)) {
                
                /*if(flags & ShowAddTopicTip) {
                    
                    self.autoAddHintVC = [[BNAutoAddTopicsHintViewController alloc] init];
                    self.autoAddHintVC.delegate = self;
                    self.autoAddHintVC.view.frame = self.view.frame;
                    
                    self.autoAddHintVC.view.alpha = 0.0f;
                    [self.view addSubview:self.autoAddHintVC.view];
                    
                    [UIView animateWithDuration:0.2 delay:0 options:UIViewAnimationOptionCurveEaseIn animations:^{
                        self.autoAddHintVC.view.alpha = 1.0f;
                        
                    } completion:^(BOOL finished){
                    }];

                }*/
                return;
            }
        }
        
        // If navigating to a followed topic, push a new ribbon VC.
        /*if ([BNCollections isFollowed:stub]) {
            pagingVC = [BNRibbonViewController new];
            pagingVC.viewPager.selectedIndex = [pagingVC.contentArray indexOfObject:stub];
        } else {*/
        
            // Otherwise create an ad-hoc collection
            vector<BNContent::stub> stubs;
            bool containsItem = false;
            for (BNBaseModel* baseModel : collection) {
                if (!baseModel->isContent()) {
                    continue;
                }
                BNContent* content = (BNContent*)baseModel;
                stubs.push_back(content->getStubWithOverriddenTitle(title));
                containsItem |= stub.modelId == content->_modelId;
            }
            if (!containsItem) {
                stubs.insert(stubs.begin(), stub);
            }
            
            pagingVC = new BNContentViewPagerController();
            pagingVC->setTitle(title);
            pagingVC->setContent(stub, stubs);
        //}
        
        // Push the new pageable VC
        pushViewController(pagingVC); // TODO:, !(flags & NoAnimation));
    }
}


class BNTabBarButton : public View {
public:
    BNTabBarButton() {
        inflate("tab_button.res");
        bind(_imageView, "image");
        bind(_label, "label");
    }
    bool applySingleStyle(const string& name, const style& value) override {
        if (name=="text") {
            _label->setText(value.stringVal());
            return true;
        }
        if (name=="image") {
            _imageView->setImageAsset(value.stringVal());
            return true;
        }
        return View::applySingleStyle(name, value);
    }

    ImageView* _imageView;
    Label* _label;
};

DECLARE_DYNCREATE(BNTabBarButton);

class MainViewController : public ViewController {
public:
    BNContentView* _topStories;
    BNContentView* _myNews;
    BNContentView* _popular;
    BNContentView* _videos;
    View* _search;
    TabBar* _tabBar;
    
    MainViewController() {
        inflate("main_vc.res");
        bind(_topStories, "top-stories");
        bind(_myNews, "my-news");
        bind(_popular, "popular");
        bind(_videos, "video");
        bind(_search, "search");
        bind(_tabBar, "tabbar");
        
        
        RenderOp* op = new BlurRenderOp();
        op->setColor(0xFFFFFF);
        _tabBar->setBackground(op);
        _tabBar->onSelectedIndexSet = [=](int index) {
            _topStories->setVisibility((index==0)?Visibility::Visible : Visibility::Gone);
            _myNews->setVisibility((index==1)?Visibility::Visible : Visibility::Gone);
            _popular->setVisibility((index==2)?Visibility::Visible : Visibility::Gone);
            _videos->setVisibility((index==3)?Visibility::Visible : Visibility::Gone);
            _search->setVisibility((index==4)?Visibility::Visible : Visibility::Gone);
        };
        _tabBar->setSelectedIndex(0);

        ImageView* logo = new ImageView();
        logo->setImageAsset("images/logo.png");
        setTitleView(logo);
        
        _topStories->setContentStub(BNContent::stub::fromID("/cps/news/front_page", "Front Page"));
        _popular->setContentStub(BNContent::stub::fromID(BNModelIdMostPopular, "Popular"));
        
    }
    
    void applySafeInsets(const EDGEINSETS& safeInsets) override {
        EDGEINSETS insets = {0, safeInsets.top, 0, safeInsets.bottom + app->dp(60)};
        _topStories->setPadding(insets);
        _topStories->setScrollInsets(insets);
        _tabBar->setPadding({0,0,0, safeInsets.bottom});
    }

};




void BBCNewsApp::main() {
    
    _compactMode = getBoolSetting("compactMode", false);
    _carouselsMode = getBoolSetting("carouselsMode", false);
    _hasEverUsedCarouselsMode = getBoolSetting("hasEverUsedCarouselsMode", false);
    loadLayouts();
    
    NavigationController* nav = new NavigationController();
    nav->getNavBar()->setBlurEnabled(true);
    nav->pushViewController(new MainViewController());
    _window->setRootViewController(nav);
}

void BBCNewsApp::setCompactMode(bool compactMode) {
    if (compactMode == _compactMode || app->_defaultDisplay->sizeDiagonalInches()>=7) {
        return;
    }
    _compactMode = compactMode;
    setBoolSetting("compactMode", compactMode);
    loadLayouts();
    notify("NewLayouts");
}

void BBCNewsApp::setCarouselsMode(bool carouselsMode) {
    if (carouselsMode == _carouselsMode) {
        return;
    }
    _carouselsMode = carouselsMode;
    setBoolSetting("carouselsMode", carouselsMode);
    setBoolSetting("hasEverUsedCarouselsMode", true);
    loadLayouts();
    notify("NewLayouts");
}

void BBCNewsApp::loadLayouts() {
    
    bool requestRemoteLayouts = true;
#if DEBUG
    requestRemoteLayouts = false;
#endif
    
    
    // Get current layout dir, fall back to built-in layouts if none are downloaded
    string layoutDir = "//assets/layout";
    string serverlayoutsLocation = getStringSetting(LAYOUT_SETTING_KEY);
    if (serverlayoutsLocation.length() && requestRemoteLayouts) {
        layoutDir = "//data/" + serverlayoutsLocation;
    }
    
    
    
    _currentLayouts = new BNLayoutsGroup(layoutDir);
    
    string urlstr = BNPolicy::current()->_endpointLayout->getHREF();
    urlstr += "&AppVersion=";
    app->log("todo: download remote layouts");
    /*urlstr += BNDeviceSpec::appVersion();
    if (requestRemoteLayouts) {
        [[BNURLRequestManager sharedInstance] requestURL:url delegate:self flags:0 priority:BNDownloadPriorityLow ttl:60*60 creatorBlock:^BNURLRequest *{
         return [[BNLayoutsZipURLRequest alloc] initWithURL:url];
         }];
    } else {
        [[BNURLRequestManager sharedInstance] unrequestURL:url delegate:self];
    }*/
}


BNLayout* BBCNewsApp::layoutWithContent(const string& modelId,
                                        const string& site,
                                        const string& format,
                                        const string& orientation) {
    return _currentLayouts->layoutWithContent(modelId, site, format, orientation);
}



