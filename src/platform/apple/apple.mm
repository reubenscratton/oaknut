//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#import <oaknut.h>

string ns2str(NSString* s) {
    auto sz = [s cStringUsingEncoding:NSUTF8StringEncoding];
    return string(sz, (uint32_t)strlen(sz));
}
NSString* str2ns(const string& s) {
    return [NSString stringWithCString:s.c_str() encoding:NSUTF8StringEncoding];
}
error nserr(NSError* e) {
    if (!e) {
        return error::none();
    }
    return error((int)e.code, ns2str(e.localizedDescription));
}

int App::getIntSetting(const string& key, const int defaultValue) {
    id val = [[NSUserDefaults standardUserDefaults] objectForKey:str2ns(key)];
    return val ? [val intValue] : defaultValue;
}
void App::setIntSetting(const string& key, const int value) {
    [[NSUserDefaults standardUserDefaults] setObject:@(value) forKey:str2ns(key)];
}

bool App::getBoolSetting(const string& key, const bool defaultValue) {
    id val = [[NSUserDefaults standardUserDefaults] objectForKey:str2ns(key)];
    return val ? [val boolValue] : defaultValue;
}
void App::setBoolSetting(const string& key, const bool value) {
    [[NSUserDefaults standardUserDefaults] setObject:@(value) forKey:str2ns(key)];
}

string App::getStringSetting(const string& key, const string& defaultValue) {
    id val = [[NSUserDefaults standardUserDefaults] objectForKey:str2ns(key)];
    if (!val) {
        return defaultValue;
    }
    return ns2str((NSString*)val);
}
void App::setStringSetting(const string& key, const string& value) {
    NSString* val = str2ns(value);
    [[NSUserDefaults standardUserDefaults] setObject:val forKey:str2ns(key)];
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



void Task::postToMainThread(std::function<void(void)> func, int delay) {
    assert(func);
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



bool File::resolve(string& path) {
    if (!path.hasPrefix("//"_S)) {
        return true;
    }
    
    //assets/...
    if (path.hasPrefix("//assets/"_S)) {
        path.erase(0, 1); // remove just the leading slash, leaving the path as '/assets/...'
        string bundlepath = ns2str([NSBundle bundleForClass:NSClassFromString(@"NativeView")].bundlePath);
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
        string fsr(url.fileSystemRepresentation, -1);
        fsr.append("/");
        path.prepend(fsr);
        return true;
    }
    
    //cache/... : ~/Library/Caches/<bundleid>/...
    if (path.hadPrefix("//cache/"_S)) {
        NSURL* url = [[[NSFileManager defaultManager] URLsForDirectory:NSCachesDirectory inDomains:NSUserDomainMask] lastObject];
        url = [url URLByAppendingPathComponent:[[NSBundle mainBundle] bundleIdentifier]];
        string fsr(url.fileSystemRepresentation, -1);
        fsr.append("/");
        path.prepend(fsr);
        return true;
    }
    
    //docs/... : ~/Documents/...
    if (path.hadPrefix("//docs/"_S)) {
        NSURL* url = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
        string fsr(url.fileSystemRepresentation, -1);
        fsr.append("/");
        path.prepend(fsr);
        return true;
    }

    //tmp/... : ~/Documents/...
    if (path.hadPrefix("//tmp/"_S)) {
        string fsr(NSTemporaryDirectory().fileSystemRepresentation, -1);
        fsr.append("/");
        path.prepend(fsr);
        return true;
    }

    // :-(
    log_warn("Unknown path: %s", path.c_str());
    return false;
}
    
bool File::mkdirs_sync(const string& apath, bool lastElementIsFile) {
    string path = apath;
    if (!resolve(path)) {
        return false;
    }
    [[NSFileManager defaultManager] createDirectoryAtPath:[NSString stringWithUTF8String:path.c_str()] withIntermediateDirectories:YES attributes:nil error:nil];
    return true;
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
