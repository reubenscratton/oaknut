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

bool App::fileResolve(string& path) const {
    if (!path.hasPrefix("//"_S)) {
        return true;
    }
    //assets/...
    if (path.hasPrefix("//assets/"_S)) {
        path.erase(0, 1); // remove just the leading slash, leaving the path as '/assets/...'
        return true;
    }

    warn("Could not resolve file path: %s", path.c_str());
    return false;
}
bool App::fileExists(string& path) const {
    if (!fileResolve(path)) {
        return false;
    }
    if (access(path.c_str(), F_OK) != -1) {
        return true;
    }
    return false;
}
vector<string> App::fileList(string& dir) const {
    fileResolve(dir);
    vector<string> files;
    DIR* fd;
    struct dirent* in_file;
    if (fd = opendir(dir.c_str())) {
        while (in_file = readdir(fd)) {
            if (!strcmp (in_file->d_name, "."))
                continue;
            if (!strcmp (in_file->d_name, ".."))
                continue;
            files.push_back(string(in_file->d_name, -1));
        }
        closedir(fd);
    }
    return files;

}
//fseek(fp, 0L, SEEK_END);
//sz = ftell(fp);

int App::getIntSetting(const string& key, const int defaultValue) {
    val value = lsGetInt(val(key.c_str()), val(defaultValue));
    return value.as<int>();
}
void App::setIntSetting(const string& key, const int value) {
    ls.call<void>("setItem", val(key.c_str()), val(value));
}
bool App::getBoolSetting(const string& key, const bool defaultValue) {
    val value = lsGetInt(val(key.c_str()), val(defaultValue));
    return value.as<bool>();
}

string App::getStringSetting(const string& key, const string& defaultValue) {
    val value = ls.call<val>("getItem", val(key.c_str()));
    if (value.isUndefined() || value.isNull()) {
        return defaultValue;
    }
    return value.as<string>();
}

void App::setStringSetting(const string& key, const string& value) {
    ls.call<void>("setItem", val(key.c_str()), val(value.c_str()));
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

