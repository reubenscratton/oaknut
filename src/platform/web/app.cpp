//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>


static val ls = val::global("localStorage");
static val lsGetInt = val::global("lsGetInt");

/*EDGEINSETS App::getWindowSafeAreaInsets() {
    return {0,0,0,0};
}*/

string App::currentCountryCode() const {
    return "GB"; // todo
}

int App::getIntSetting(const char *key, const int defaultValue) {
    val value = lsGetInt(val(key), val(defaultValue));
    return value.as<int>();
}
void App::setIntSetting(const char* key, const int value) {
    ls.call<void>("setItem", val(key), val(value));
}

string App::getStringSetting(const char* key, const char* defaultValue) {
    val value = ls.call<val>("getItem");
    if (value.isUndefined() || value.isNull()) {
        return defaultValue;
    }
    return value.as<string>();
}

void App::setStringSetting(const char* key, const char* value) {
    ls.call<void>("setItem", val(key), val(value));
}

#endif

