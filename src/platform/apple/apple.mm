//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#import <oaknut.h>

string nsstr(NSString* s) {
    auto sz = [s cStringUsingEncoding:NSUTF8StringEncoding];
    return string(sz, (uint32_t)strlen(sz));
}

int App::getIntSetting(const string& key, const int defaultValue) {
    id val = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithCString:key.c_str() encoding:NSUTF8StringEncoding]];
    return val ? [val intValue] : defaultValue;
}
void App::setIntSetting(const string& key, const int value) {
    [[NSUserDefaults standardUserDefaults] setObject:@(value) forKey:[NSString stringWithCString:key.c_str() encoding:NSUTF8StringEncoding]];
}

bool App::getBoolSetting(const string& key, const bool defaultValue) {
    id val = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithCString:key.c_str() encoding:NSUTF8StringEncoding]];
    return val ? [val boolValue] : defaultValue;
}
void App::setBoolSetting(const string& key, const bool value) {
    [[NSUserDefaults standardUserDefaults] setObject:@(value) forKey:[NSString stringWithCString:key.c_str() encoding:NSUTF8StringEncoding]];
}

string App::getStringSetting(const string& key, const string& defaultValue) {
    id val = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithCString:key.c_str() encoding:NSUTF8StringEncoding]];
    if (!val) {
        return defaultValue;
    }
    return nsstr((NSString*)val);
}
void App::setStringSetting(const string& key, const string& value) {
    NSString* val = [NSString stringWithCString:value.c_str() encoding:NSUTF8StringEncoding];
    [[NSUserDefaults standardUserDefaults] setObject:val forKey:[NSString stringWithCString:key.c_str() encoding:NSUTF8StringEncoding]];
}

TIMESTAMP App::currentMillis() {
    //return CACurrentMediaTime()*1000;
    using namespace std::chrono;
    milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    return ms.count();
}

string App::currentCountryCode() const {
    auto sz = [NSLocale currentLocale].countryCode.UTF8String;
    return string(sz, (int32_t)strlen(sz));
}



void App::postToMainThread(std::function<void(void)> func, int delay) {
    if (delay <= 0) {
        dispatch_async(dispatch_get_main_queue(), ^{
            func();
        });
    } else {
        float delayInSeconds = (float)delay / 1000.f;
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            func();
        });
    }
}



bool App::fileResolve(string& path) const {
    if (!path.hasPrefix("//"_S)) {
        return true;
    }
    
    //assets/...
    if (path.hasPrefix("//assets/"_S)) {
        path.erase(0, 1); // remove just the leading slash, leaving the path as '/assets/...'
        string bundlepath = nsstr([NSBundle bundleForClass:NSClassFromString(@"NativeView")].bundlePath);
#if TARGET_OS_OSX
        bundlepath.append("/Contents/Resources/"_S);
#endif
        path.prepend(bundlepath);
        return true;
    }
    
    //data/... : ~/Library/Application Support/<bundleid>/...
    if (path.hadPrefix("//data/"_S)) {
        NSURL* url = [[[NSFileManager defaultManager] URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask] lastObject];
        url = [url URLByAppendingPathComponent:[[NSBundle mainBundle] bundleIdentifier]];
        auto sz = url.fileSystemRepresentation;
        path.prepend(string(sz, uint32_t(strlen(sz))));
        return true;
    }
    
    //cache/... : ~/Library/Caches/<bundleid>/...
    if (path.hadPrefix("//cache/"_S)) {
        NSURL* url = [[[NSFileManager defaultManager] URLsForDirectory:NSCachesDirectory inDomains:NSUserDomainMask] lastObject];
        url = [url URLByAppendingPathComponent:[[NSBundle mainBundle] bundleIdentifier]];
        auto sz = url.fileSystemRepresentation;
        path.prepend(string(sz, uint32_t(strlen(sz))));
        return true;
    }
    
    //docs/... : ~/Documents/...
    if (path.hadPrefix("//docs/"_S)) {
        NSURL* url = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
        auto sz = url.fileSystemRepresentation;
        path.prepend(string(sz, uint32_t(strlen(sz))));
        return true;
    }

    //tmp/... : ~/Documents/...
    if (path.hadPrefix("//tmp/"_S)) {
        auto sz = NSTemporaryDirectory().fileSystemRepresentation;
        path.prepend(string(sz, uint32_t(strlen(sz))));
        return true;
    }

    // :-(
    app->warn("Unknown path: %s", path.c_str());
    return false;
}
    
bool App::fileEnsureFolderExists(string& path) const {
    if (!fileResolve(path)) {
        return false;
    }
    [[NSFileManager defaultManager] createDirectoryAtPath:[NSString stringWithUTF8String:path.c_str()] withIntermediateDirectories:YES attributes:nil error:nil];
    return true;
}


bool Task::isMainThread() {
    return [NSThread isMainThread] != 0;
}


vector<string> App::fileList(string& dir) const {
    fileResolve(dir);
    NSError* error = nil;
    NSArray<NSString*>* foo = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:[NSString stringWithUTF8String:dir.c_str()]  error:(NSError * _Nullable *)&error];
    assert(!error);
    vector<string> files;
    for (NSString* e : foo) {
        files.push_back(nsstr(e));
    }
    return files;

}





string string::uuid() {
    CFUUIDRef udid = CFUUIDCreate(NULL);
    CFStringRef uuidstr = CFUUIDCreateString(NULL, udid);
    auto sz = CFStringGetCStringPtr(uuidstr,kCFStringEncodingUTF8);
    string str(sz, (int32_t)strlen(sz));
    CFRelease(uuidstr);
    CFRelease(udid);
    return str;
}

#endif
