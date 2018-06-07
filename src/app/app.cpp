//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

App app;

App::App() {
    string DEFAULT_STYLES=
#include "styles.res"
    ;
    Utf8Iterator it(DEFAULT_STYLES);
    _styles.parse(it);
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
    return dp*_window->_scale;
}
float App::idp(float pix) {
    return pix/_window->_scale;
}


void App::loadStyleAsset(const string& assetPath) {
    ObjPtr<ByteBuffer> data = app.loadAsset(assetPath.data());
    if (!data) {
        return;
    }
    Utf8Iterator it(data);
    _styles.parse(it);
}


static map<string, Font*> s_loadedFonts;


Font* App::getStyleFont(const string &key) {
    string fontName = getStyleString(key + ".font-name");
    float fontSize = getStyleFloat(key + ".font-size");
    char ach[260];
    sprintf(ach, "%f-%s", fontSize, fontName.data());
    string fkey(ach);
    auto it = s_loadedFonts.find(fkey);
    if (it != s_loadedFonts.end()) {
        return it->second;
    }
    Font* font = new Font(fontName, fontSize);
    s_loadedFonts[fkey] = font;
    return font;
}

StyleValue* App::getStyleValue(const string& keypath) {
    StyleValue* value = _styles.getValue(keypath);
    while (value && value->type == StyleValue::Type::Reference) {
        value = getStyleValue(value->str);
    }
    return value;
}

string App::getStyleString(const string& keypath) {
    StyleValue* value = getStyleValue(keypath);
    if (!value) {
        app.warn("Missing string style info '%s'", keypath.data());
        return "";
    }
    if (value->type==StyleValue::Type::String) return value->str;
    else if (value->type==StyleValue::Type::Int) return stringFromInt(value->i);
    else if (value->type==StyleValue::Type::Double) return stringFromDouble(value->d);
    return "";
}

float App::getStyleFloat(const string& keypath) {
    StyleValue* value = getStyleValue(keypath);
    if (!value) {
        app.warn("Missing float style info '%s'", keypath.data());
        return 0;
    }
    return value->getAsFloat();
}
COLOUR App::getStyleColour(const string& key) {
    StyleValue* value = getStyleValue(key);
    if (!value) {
        app.warn("Missing colour style info '%s'", key.data());
        return 0;
    }
    if (value->type==StyleValue::Type::Int) return value->i;
    return 0;
}

static View* inflateFromResource(pair<string, StyleValue*> r, View* parent) {
    string viewClassName = r.first;
    View* view = (View*)Object::createByName(viewClassName);
    if (parent) {
        parent->addSubview(view);
    }
    
    // Extract the attributes from the uber
    StyleValue* props = r.second;
    assert(props->type == StyleValue::Type::StyleMap);
    vector<pair<string, StyleValue*>> attribs;
    vector<pair<string, StyleValue*>> subviews;
    for (auto i : props->styleMap->_valuesList)  {
        StyleValue* val = i.second->select();
        if (val) {
            auto a = make_pair(i.first, val);
            if (val->type == StyleValue::Type::StyleMap && i.first != "style") {
                subviews.push_back(a);
            } else {
                attribs.push_back(a);
            }
        }
    }
    
    // Apply the attributes to the inflated view
    view->applyStyleValues(attribs);
    
    // Inflate the subviews
    for (auto j : subviews) {
        inflateFromResource(j, view);
    }
    
    return view;
}


View* App::layoutInflate(const string& assetPath) {
    ObjPtr<ByteBuffer> data = loadAsset(assetPath.data());
    if (!data) {
        return NULL;
    }
    Utf8Iterator it(data);
    StyleMap layoutRoot;
    bool parsed = layoutRoot.parse(it);
    assert(parsed);
    assert(layoutRoot._values.size()==1);
    auto i = layoutRoot._values.begin();
    return inflateFromResource(make_pair(i->first, i->second->select()), NULL);
}



/**
 Settings is implemented by a single LocalStore containing a single VariantMap
 */

static LocalStore* s_localstore;
static VariantMap s_userdefaults;


static void ensureLoaded() {
    if (!s_localstore) {
        s_localstore = LocalStore::create("_userdefaults", "unused");
    }
    if (s_localstore->moveFirst()) {
        s_localstore->readAndAdvance(s_userdefaults);
    }
    s_userdefaults.set("unused", 0); // set a primary key value although it's not used

}


int App::getIntSetting(const char *key, const int defaultValue) {
    ensureLoaded();
    if (s_userdefaults.hasValue(key)) {
        return (int32_t)s_userdefaults.get(key);
    }
    return defaultValue;
}


void App::setIntSetting(const char* key, const int value) {
    ensureLoaded();
    s_userdefaults.set(key, value);
}

string App::getStringSetting(const char* key, const char* defaultValue) {
    ensureLoaded();
    if (s_userdefaults.hasValue(key)) {
        return (string)s_userdefaults.get(key);
    }
    return defaultValue;
}

void App::setStringSetting(const char* key, const char* value) {
    ensureLoaded();
    s_userdefaults.set(key, value);
}

void App::saveSettings() {
    //s_localstore->save();
    /*FileStream stream(getUserDefaultsPath());
     if (stream.openForWrite()) {
     s_userdefaults->writeSelfToStream(&stream);
     stream.close();
     }*/
}
