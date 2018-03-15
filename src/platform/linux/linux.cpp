//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_LINUX

#include "oaknut.h"
#include <sys/time.h>


string oakGetAppHomeDir() {
    return string("."); // will this work in AppImage?
}



Data* App::loadAsset(const char* assetPath) {
    
    string str = "assets/";
    str.append(assetPath);
    FILE* asset = fopen(str.data(), "rb");
    if (!asset) {
        app.log("Failed to open asset: %s", assetPath);
        return NULL;
    }
    
    Data* data = new Data();
    fseek (asset, 0, SEEK_END);
    data->cb = ftell(asset);
    data->data = (uint8_t*) malloc (sizeof(char)*data->cb);
    fseek ((FILE*)asset, 0, SEEK_SET);
    size_t read = fread(data->data, 1, data->cb, (FILE*)asset);
    assert(read == data->cb);
    fclose(asset);
    return data;
    
}



#endif