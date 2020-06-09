//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

App* oak::app;

App::App() {
    assert(!app);
    app = this;
    string stylesres(
#include "styles.res"
    );
    _styles.parse(stylesres);
}



float App::dp(float dp) {
    return dp*_defaultDisplay->_scale;
}
float App::idp(float pix) {
    return pix/_defaultDisplay->_scale;
}


void App::loadStyleAssetSync(const string& assetPath) {
    variant v = loadAssetSync(assetPath);
    if (v.isError()) {
    } else {
        auto& data = v.bytearrayRef();
        string str = data.toString();
        style s;
        s.parse(str);
        assert(s.isCompound());
        _styles.importNamedValues(*s.compound);
    }
}


const style* App::getStyle(const string& keypath) {
    static map<string,const style*> styleCache;
    auto cacheVal = styleCache.find(keypath);
    if (cacheVal != styleCache.end()) {
        return cacheVal->second;
    }
    const style* val = _styles.get(keypath);
    styleCache.insert(std::make_pair(keypath, val));
    return val;
}

Font* App::defaultFont() {
    return Font::get("", 17.0);
}

string App::getStyleString(const string& keypath, const string& defaultString) {
    auto value = getStyle(keypath);
    if (!value) {
        if (defaultString) {
            return defaultString;
        }
        log_warn("Missing string style info '%s'", keypath.c_str());
        return "";
    }
    return value->stringVal();
}

float App::getStyleFloat(const string& keypath) {
    auto value = getStyle(keypath);
    if (!value) {
        log_warn("Missing float style info '%s'", keypath.c_str());
        return 0;
    }
    return value->floatVal();
}
COLOR App::getStyleColor(const string& key) {
    auto value = getStyle(key);
    if (!value) {
        log_warn("Missing color style info '%s'", key.c_str());
        return 0;
    }
    return value->colorVal();
}

static View* inflateFromResource(const style& value, View* parent) {
    string className = value.stringVal("class");
    if (className.length() == 0) {
        className = "View";
    }
    View* view = (View*)Object::createByName(className);
    if (parent) {
        parent->addSubview(view);
    }
    
    // Apply the attributes to the inflated view.
    view->applyStyle(value);
    
    // Inflate the subviews
    auto& subviews = value.arrayVal("subviews");
    for (auto& subview : subviews) {
        inflateFromResource(subview, view);
    }
    
    return view;
}


void App::layoutInflateExistingView(View* view, const string& assetPath) {
    variant v = loadAssetSync(assetPath);
    if (v.isError()) {
        return;
    }
    auto& data = v.bytearrayRef();
    string str = data.toString();
    style layoutRoot;
    layoutRoot.parse(str);
    assert(layoutRoot.isCompound());

    // Apply the attributes to the inflated view.
    view->applyStyle(layoutRoot);
    
    // Inflate the subviews
    auto& subviews = layoutRoot.arrayVal("subviews");
    for (auto& subview : subviews) {
        inflateFromResource(subview, view);
    }
}

View* App::layoutInflate(const string& assetPath) {
    variant v = loadAssetSync(assetPath);
    if (v.isError()) {
        return NULL;
    }
    auto& data = v.bytearrayRef();
    string str = data.toString();
    style layoutRoot;
    layoutRoot.parse(str);
    assert(layoutRoot.isCompound());
    return inflateFromResource(layoutRoot, NULL);
}


string App::friendlyTimeString(TIMESTAMP timestamp) {
    TIMESTAMP now = currentMillis();
    int age = (int)(now - timestamp);
    int seconds = age / 1000;
    int minutes = seconds / 60;
    seconds = seconds % 60;
    int hours = minutes / 60;
    minutes = minutes % 60;
    
    if (minutes<1) {
        return "Just now";
    }
    if (hours <1) {
        return string::format("%d min", minutes);
    }
    char buff[256];
    time_t time = timestamp / 1000;
    struct tm* tm = localtime(&time);
    strftime(buff, sizeof(buff), "%d %b %Y", tm);
    return buff;
}




static bool qualifierIsScale(const string& q) {
    return (q=="1x" || q=="2x" || q=="3x");
}



Task* App::loadAsset(const string& assetPath, std::function<void(variant&)> callback) {
    return Task::enqueue({
        {Task::IO, [=](variant&) -> variant {
            return loadAssetSync(assetPath);
        }},
        {Task::MainThread, [=](variant& r) -> variant {
            callback(r);
            return variant();
        }}
    });
}
Task* App::loadBitmapAsset(const string& assetPath, std::function<void(Bitmap*)> callback) {
    return Task::enqueue({
        {Task::IO, [=](variant&) -> variant {
            return loadAssetSync(assetPath);
        }},
        {Task::Background, [=](variant& result) -> variant {
            if (result.isError()) {
                return result;
            }
            Bitmap* bitmap = Bitmap::createFromData(result.bytearrayRef());
            return variant(bitmap);
        }},
        {Task::MainThread, [=](variant& r) -> variant {
            if (r.isError()) {
                callback(nullptr);
            } else {
                callback(r.objectVal<Bitmap>());
            }
            return variant();
        }}
    });
}



variant App::loadAssetSync(const string& assetPath) {
    
    // TODO: asset cache needed

    // If the file exists as named then that's the easy path
    string path = "//assets/" + assetPath;
    variant data = File::load_sync(path);
    if (data.isError()) {
        
        // Oh dear, let's try to find a config-specific asset. First get the list of files.
        auto dirpos = path.findLast("/");
        string dirname = path.substr(0, dirpos);
        string filename = path.substr(dirpos+1);
        vector<string> files = File::dir_sync(dirname);

        // Filter out irrelevant files
        auto extpos = filename.findLast(".");
        string ext = filename.substr(extpos);
        filename = filename.substr(0, extpos) + "@";
        for (int i=0 ; i<files.size() ; i++) {
            string& f = files[i];
            if (!f.hadSuffix(ext)) {
                files.erase(files.begin()+i--);
                continue;
            }
            if (!f.hadPrefix(filename)) {
                files.erase(files.begin()+i--);
                continue;
            }
        }
        
        // Weed out qualifiers that don't apply
        // TODO: share code with styles here
        for (int i=0 ; i<files.size() ; i++) {
            string& f = files[i];
            bool qualFail = false;
            qualFail |= (f == "3x" && app->_defaultDisplay->_scale < 3);
            qualFail |= (f == "2x" && app->_defaultDisplay->_scale < 2);
            if (qualFail) {
                files.erase(files.begin()+i--);
            }
        }
        
        // If there's still multiple options, filter down to a single "best" or most-specific qualified file
        while (files.size() > 1) {
            for (int i=1 ; i<files.size() ; i++) {
                if (qualifierIsScale(files[0]) && qualifierIsScale(files[i])) {
                    bool keepFirst = files[0].charAt(0) > files[i].charAt(0);
                    files.erase(files.begin()+ (keepFirst?i:0));
                    break;
                } else {
                    assert(0); // TODO: finish tie-breaker code
                }
            }
        }

        // Load the winning qualified file
        path = dirname + "/" + filename + files[0] + ext;
        data = File::load_sync(path);
    }
    
    return data;
}

void App::subscribe(const char* notificationName, Object* observer, std::function<void(const char*, void*, variant&)> callback, void* source/*=NULL*/) {
    
}
void App::notify(const char* notificationName, void* source/*=NULL*/, variant data/*=variant()*/) {
    
}
void App::unsubscribe(const char* notificationName, Object* observer) {
    
}
