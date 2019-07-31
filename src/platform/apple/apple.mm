//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#import <oaknut.h>

int App::getIntSetting(const char *key, const int defaultValue) {
    id val = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithCString:key encoding:NSUTF8StringEncoding]];
    return val ? [val intValue] : defaultValue;
}
void App::setIntSetting(const char* key, const int value) {
    [[NSUserDefaults standardUserDefaults] setObject:@(value) forKey:[NSString stringWithCString:key encoding:NSUTF8StringEncoding]];
}

bool App::getBoolSetting(const char *key, const bool defaultValue) {
    id val = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithCString:key encoding:NSUTF8StringEncoding]];
    return val ? [val boolValue] : defaultValue;
}
void App::setBoolSetting(const char* key, const bool value) {
    [[NSUserDefaults standardUserDefaults] setObject:@(value) forKey:[NSString stringWithCString:key encoding:NSUTF8StringEncoding]];
}

string App::getStringSetting(const char *key, const char* defaultValue) {
    id val = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithCString:key encoding:NSUTF8StringEncoding]];
    if (!val) {
        return defaultValue;
    }
    NSString* nsstring = val;
    return string([nsstring cStringUsingEncoding:NSUTF8StringEncoding]);
}
void App::setStringSetting(const char* key, const char* value) {
    NSString* val = [NSString stringWithCString:value encoding:NSUTF8StringEncoding];
    [[NSUserDefaults standardUserDefaults] setObject:val forKey:[NSString stringWithCString:key encoding:NSUTF8StringEncoding]];
}

TIMESTAMP App::currentMillis() {
    //return CACurrentMediaTime()*1000;
    using namespace std::chrono;
    milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    return ms.count();
}

string App::currentCountryCode() const {
    return [NSLocale currentLocale].countryCode.UTF8String;
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
        _queue.name = [[NSString alloc] initWithUTF8String:name.data()];
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
    if (!path.hasPrefix("//")) {
        return true;
    }
    
    //assets/...
    if (path.hasPrefix("//assets/")) {
        path.erase(0); // remove just the leading slash, leaving the path as '/assets/...'
        string bundlepath = [NSBundle bundleForClass:NSClassFromString(@"NativeView")].bundlePath.UTF8String;
#if TARGET_OS_OSX
        bundlepath.append("/Contents/Resources/");
#endif
        path.insert(0, bundlepath);
        return true;
    }
    
    //data/... : ~/Library/Application Support/<bundleid>/...
    if (path.hadPrefix("//data/")) {
        NSURL* url = [[[NSFileManager defaultManager] URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask] lastObject];
        url = [url URLByAppendingPathComponent:[[NSBundle mainBundle] bundleIdentifier]];
        path.insert(0, url.fileSystemRepresentation);
        return true;
    }
    
    //cache/... : ~/Library/Caches/<bundleid>/...
    if (path.hadPrefix("//cache/")) {
        NSURL* url = [[[NSFileManager defaultManager] URLsForDirectory:NSCachesDirectory inDomains:NSUserDomainMask] lastObject];
        url = [url URLByAppendingPathComponent:[[NSBundle mainBundle] bundleIdentifier]];
        path.insert(0, url.fileSystemRepresentation);
        return true;
    }
    
    //docs/... : ~/Documents/...
    if (path.hadPrefix("//docs/")) {
        NSURL* url = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
        path.insert(0, url.fileSystemRepresentation);
        return true;
    }

    //tmp/... : ~/Documents/...
    if (path.hadPrefix("//tmp/")) {
        path.insert(0, NSTemporaryDirectory().fileSystemRepresentation);
        return true;
    }

    // :-(
    app->warn("Unknown path: %s", path.data());
    return false;
}
    
bool App::fileEnsureFolderExists(string& path) const {
    if (!fileResolve(path)) {
        return false;
    }
    [[NSFileManager defaultManager] createDirectoryAtPath:[NSString stringWithUTF8String:path.data()] withIntermediateDirectories:YES attributes:nil error:nil];
    return true;
}


bool App::fileExists(string& path) const {
    if (!fileResolve(path)) {
        return false;
    }
    return [[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithUTF8String:path.data()]];
}
uint64_t App::fileSize(string& path) const {
    if (!fileResolve(path)) {
        return 0;
    }
    NSError* err = nil;
    NSDictionary* fileAttributes = [[NSFileManager defaultManager] attributesOfItemAtPath:[NSString stringWithUTF8String:path.data()] error:&err];
    if (err) {
        app->warn("Error getting attributes for file at %s", path.data());
        return 0;
    }
    
    NSNumber *fileSizeNumber = [fileAttributes objectForKey:NSFileSize];
    return [fileSizeNumber longLongValue];
}
//    return fopen(str.data(), "rb");

vector<string> App::fileList(string& dir) const {
    fileResolve(dir);
    NSError* error = nil;
    NSArray<NSString*>* foo = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:[NSString stringWithUTF8String:dir.data()]  error:(NSError * _Nullable *)&error];
    assert(!error);
    vector<string> files;
    for (NSString* e : foo) {
        files.push_back(e.UTF8String);
    }
    return files;

}





string string::uuid() {
    CFUUIDRef udid = CFUUIDCreate(NULL);
    CFStringRef uuidstr = CFUUIDCreateString(NULL, udid);
    string str(CFStringGetCStringPtr(uuidstr,kCFStringEncodingUTF8));
    CFRelease(uuidstr);
    CFRelease(udid);
    return str;
}

#endif
