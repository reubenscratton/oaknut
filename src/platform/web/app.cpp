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


ByteBuffer* App::loadAsset(const char* assetPath) {
    
    string str = "/assets/";
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

class WebGLRenderer : public GLRenderer {
public:
    WebGLRenderer(Window* window) : GLRenderer(window) {
    }
    void bindToNativeWindow(long nativeWindowHandle) override {
        // todo: move EAGL setup code here
    }
    void commit() override {
        // todo: 
    }
};

Renderer* Renderer::create(Window* window) {
    return new WebGLRenderer(window);
}

#endif

