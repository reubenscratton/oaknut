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
    return string(sz, strlen(sz));
}


/*#if TARGET_OS_IOS
static EAGLSharegroup* s_mainEAGLSharegroup;
#else
static CGLContextObj s_mainContext;
#endif

void Task::ensureSharedGLContext() {
#if TARGET_OS_IOS
    if (![EAGLContext currentContext]) {
        EAGLContext* eaglcontext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup: s_mainEAGLSharegroup];
        assert(eaglcontext);
        [EAGLContext setCurrentContext:eaglcontext];
    }
#else
    if (!CGLGetCurrentContext()) {
        CGLContextObj ctx;
        CGLCreateContext(CGLGetPixelFormat(s_mainContext), s_mainContext, &ctx);
        CGLSetCurrentContext(ctx);
    }
#endif
}
*/
Task* App::postToMainThread(std::function<void(void)> func, int delay) {
    Task* task = new Task(func);
    if (delay <= 0) {
        dispatch_async(dispatch_get_main_queue(), ^{
            task->complete();
        });
    } else {
        float delayInSeconds = (float)delay / 1000.f;
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            task->complete();
        });
    }
    return task;
}

/*
class AppleTask : public Task {
public:
    NSBlockOperation* _op;

    AppleTask(std::function<void(void)> func) : Task(func) {
        _op = [NSBlockOperation blockOperationWithBlock:^() {
            _func();
        }];
    }
    
    bool cancel() override {
        if (!_op.isCancelled && !_op.isFinished && !_op.isExecuting) {
            [_op cancel];
            return _op.isCancelled;
        }
        return false;
    }

};

class AppleTaskQueue : public TaskQueue {
public:

    NSOperationQueue* _queue;

    AppleTaskQueue(const string& name) : TaskQueue(name) {
#if TARGET_OS_IOS
        if (!s_mainEAGLSharegroup) {
            s_mainEAGLSharegroup = [EAGLContext currentContext].sharegroup;
        }
#else
        if (!s_mainContext) {
             s_mainContext = CGLGetCurrentContext();
        }
#endif
        _queue = [NSOperationQueue new];
        _queue.name = [[NSString alloc] initWithUTF8String:name.c_str()];
    }
    ~AppleTaskQueue() {
        _queue = nil;
    }

    Task* enqueueTask(std::function<void(void)> func) {
        AppleTask* task = new AppleTask(func);
        [_queue addOperation:task->_op];
        return task;
    }

};

TaskQueue* TaskQueue::create(const string& name) {
    return new AppleTaskQueue(name);
}
*/


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


bool App::fileExists(string& path) const {
    if (!fileResolve(path)) {
        return false;
    }
    return [[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithUTF8String:path.c_str()]];
}
uint64_t App::fileSize(string& path) const {
    if (!fileResolve(path)) {
        return 0;
    }
    NSError* err = nil;
    NSDictionary* fileAttributes = [[NSFileManager defaultManager] attributesOfItemAtPath:[NSString stringWithUTF8String:path.c_str()] error:&err];
    if (err) {
        app->warn("Error getting attributes for file at %s", path.c_str());
        return 0;
    }
    
    NSNumber *fileSizeNumber = [fileAttributes objectForKey:NSFileSize];
    return [fileSizeNumber longLongValue];
}
//    return fopen(str.c_str(), "rb");

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
    string str(sz, strlen(sz));
    CFRelease(uuidstr);
    CFRelease(udid);
    return str;
}

#endif
