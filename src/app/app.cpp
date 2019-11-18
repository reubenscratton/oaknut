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

#ifndef ANDROID
void App::log(char const* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    printf("\n");
}
void App::warn(char const* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("Warning: ");
    vprintf(fmt, args);
    printf("\n");
}
#endif


float App::dp(float dp) {
    return dp*_defaultDisplay->_scale;
}
float App::idp(float pix) {
    return pix/_defaultDisplay->_scale;
}


void App::loadStyleAsset(const string& assetPath) {
    bytearray data;
    if (!loadAsset(assetPath, data)) {
        return;
    }
    string str = data.toString();
    style s;
    s.parse(str);
    assert(s.isCompound());
    _styles.importNamedValues(*s.compound);
}


const style* App::getStyle(const string& keypath) {
    return _styles.get(keypath);
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
        app->warn("Missing string style info '%s'", keypath.c_str());
        return "";
    }
    return value->stringVal();
}

float App::getStyleFloat(const string& keypath) {
    auto value = getStyle(keypath);
    if (!value) {
        app->warn("Missing float style info '%s'", keypath.c_str());
        return 0;
    }
    return value->floatVal();
}
COLOR App::getStyleColor(const string& key) {
    auto value = getStyle(key);
    if (!value) {
        app->warn("Missing color style info '%s'", key.c_str());
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
    bytearray data;
    if (!loadAsset(assetPath, data)) {
        return;
    }
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
    bytearray data;
    if (!loadAsset(assetPath, data)) {
        return NULL;
    }
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



// Generic file handling
bool App::fileLoad(const string& path, bytearray& fileContents) const {
    string rpath = path;
    if (!fileResolve(rpath)) {
        return false;
    }
    FILE* file = fopen(rpath.c_str(), "rb");
    if (!file) {
        app->warn("Failed to open %s", path.c_str());
        return false;
    }
    fseek (file, 0, SEEK_END);
    uint64_t cb = ftell(file);
    fileContents.resize(cb);
    fseek (file, 0, SEEK_SET);
    size_t read = fread(fileContents.data(), 1, cb, (FILE*)file);
    assert(read == cb);
    fclose(file);
    return true;
}

bool App::loadAsset(const string& assetPath, bytearray& data) {
    string path = "//assets/";
    path += assetPath;
    return fileLoad(path, data);
}

void App::subscribe(const char* notificationName, Object* observer, std::function<void(const char*, void*, variant&)> callback, void* source/*=NULL*/) {
    
}
void App::notify(const char* notificationName, void* source/*=NULL*/, variant data/*=variant()*/) {
    
}
void App::unsubscribe(const char* notificationName, Object* observer) {
    
}
