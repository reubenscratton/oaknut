//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_LINUX

#include "oaknut.h"
#include <sys/time.h>

string App::getPathForGeneralFiles() {
    return "~"; // todo: 
}

ByteBuffer* App::loadAsset(const char* assetPath) {
    
    string str = "assets/";
    str.append(assetPath);
    FILE* asset = fopen(str.data(), "rb");
    if (!asset) {
        app.log("Failed to open asset: %s", assetPath);
        return NULL;
    }
    
    ByteBuffer* data = new ByteBuffer();
    fseek (asset, 0, SEEK_END);
    data->cb = ftell(asset);
    data->data = (uint8_t*) malloc (sizeof(char)*data->cb);
    fseek ((FILE*)asset, 0, SEEK_SET);
    size_t read = fread(data->data, 1, data->cb, (FILE*)asset);
    assert(read == data->cb);
    fclose(asset);
    return data;
    
}


static variant g_settings;
static bool g_settingsLoaded;

static void ensureSettingsLoaded() {
    if (!g_settingsLoaded) {
        g_settingsLoaded = true;

    }

}

/** Gets a named integer setting */
int App::getIntSetting(const char* key, const int defaultValue) {
    ensureSettingsLoaded();
    auto val = g_settings.get(key);
    return val ? val->intVal() : defaultValue;
}

/** Sets a named integer setting. */
void App::setIntSetting(const char* key, int value) {
    ensureSettingsLoaded();
    g_settings.set(key, value);
}

/** Gets a named string setting */
string App::getStringSetting(const char* key, const char* defaultValue) {
    ensureSettingsLoaded();
    auto val = g_settings.get(key);
    return val ? val->stringVal() : defaultValue;
}

/** Sets a named string setting. NB: call saveSettings() when finished updating settings. */
void App::setStringSetting(const char* key, const char* value) {
    ensureSettingsLoaded();
    g_settings.set(key, value);
}



#endif
