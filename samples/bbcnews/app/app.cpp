//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "app.h"
#include "model/_module.h"
#include "policy/policy.h"
#include "ui/contentview.h"

static string URLBASE_CONTENT = "https://trevor-producer-cdn.api.bbci.co.uk/content";

static BBCNewsApp the_app;


#define LAYOUT_SETTING_KEY "layoutDir2"

class MainViewController : public ViewController {
public:
    BNContentView* _contentView;
    
    MainViewController() {
        inflate("main_vc.res");
        bind(_contentView, "content");
        
        ImageView* logo = new ImageView();
        logo->setImageAsset("images/logo.png");
        setTitleView(logo);
        
        auto stub = BNContent::stub::fromID("/cps/news/front_page", "Front Page");
        _contentView->setContentStub(stub);
        
        /*auto req = URLRequest::get(URLBASE_CONTENT + "/cps/news/front_page");
        req->handleJson([=] (URLRequest* req, const variant& data) {
            if (req->didError()) {
                app->log("boo!");
            } else {
                auto content = BNBaseModel::createModelObjectFromJson(data);
                app->log("yay!");
                
            }
        });*/
    }
};



static std::map<string, Object* (*)()> s_register;

template<typename T, typename ...ARGS>
class FooRegistrar {
private: static Object* createT(ARGS... args) {return new T(args...); }
public:
    FooRegistrar(const string& className) {
        s_register.insert(std::make_pair(className, reinterpret_cast<Object*(*)()>(&createT)));
    }
};


template<class ...ARGS>
static Object* s_createByName(const string& className, ARGS... args) {
    const auto& constructor = s_register.find(className);
    assert(constructor != s_register.end()); // oops!
    auto real_constructor = reinterpret_cast<Object*(*)(ARGS...)>(constructor->second);
    return real_constructor(args...);
}

class Foo : public Object {
public:
    //Foo() {
        
    //}
    Foo(int f) {
        
    }
};

#define DYNCREATE(X, ...) static FooRegistrar<X,##__VA_ARGS__> s_classReg##X(STRINGIFY(X))

DYNCREATE(Foo, int);

//static FooRegistrar<Foo, int> s_classReg("Foo");

void BBCNewsApp::main() {
    
    Foo* foo = (Foo*)s_createByName("Foo", 1234);
    
    _compactMode = getBoolSetting("compactMode", false);
    _carouselsMode = getBoolSetting("carouselsMode", false);
    _hasEverUsedCarouselsMode = getBoolSetting("hasEverUsedCarouselsMode", false);
    loadLayouts();
    
    NavigationController* nav = new NavigationController();
    nav->pushViewController(new MainViewController());
    _window->setRootViewController(nav);
}

void BBCNewsApp::setCompactMode(bool compactMode) {
    if (compactMode == _compactMode || app->_defaultDisplay->size()>=7) {
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



