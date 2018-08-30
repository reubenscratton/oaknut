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


static string getPath(NSSearchPathDirectory spd) {
    // TODO: Create a 'appname' subdirectory for app support and cache options rather than blart all over the root dir
    NSURL* url = [[[NSFileManager defaultManager] URLsForDirectory:spd inDomains:NSUserDomainMask] lastObject];\
    string str = url.fileSystemRepresentation;
    [[NSFileManager defaultManager] createDirectoryAtPath:[NSString stringWithCString:str.data() encoding:NSUTF8StringEncoding] withIntermediateDirectories:YES attributes:nil error:nil];
    return str;
}

string App::getPathForGeneralFiles() {
    return getPath(NSApplicationSupportDirectory);
}
string App::getPathForUserDocuments() {
    return getPath(NSDocumentDirectory);
}
string App::getPathForCacheFiles() {
    return getPath(NSCachesDirectory);
}


#if TARGET_OS_IOS
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

void Task::nextTick(TASKFUNC func) {
    dispatch_async(dispatch_get_main_queue(), ^{
        func();
    });
}

class AppleTask : public Task {
public:
    NSBlockOperation* _op;

    AppleTask(TASKFUNC func) : Task(func) {
        _op = [NSBlockOperation blockOperationWithBlock:^() {
            _func();
        }];
    }
};

Task* Task::create(TASKFUNC func) {
    return new AppleTask(func);
}

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

    void enqueueTask(Task* task) {
        [_queue addOperation:((AppleTask*)task)->_op];
    }
    bool cancelTask(Task* atask) {
        AppleTask* task = (AppleTask*)atask;
        if (!task->_op.isCancelled && !task->_op.isFinished && !task->_op.isExecuting) {
            [task->_op cancel];
            return task->_op.isCancelled;
        }
        return false;
    }

};

TaskQueue* TaskQueue::create(const string& name) {
    return new AppleTaskQueue(name);
}




ByteBuffer* App::loadAsset(const char* assetPath) {

    NSString* path = [NSBundle mainBundle].bundlePath;
    string str = string([path UTF8String]);
#if TARGET_OS_OSX
    str.append("/Contents/Resources/");
#endif
    str.append("/assets/");
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


string string::uuid() {
    CFUUIDRef udid = CFUUIDCreate(NULL);
    CFStringRef uuidstr = CFUUIDCreateString(NULL, udid);
    string str(CFStringGetCStringPtr(uuidstr,kCFStringEncodingUTF8));
    CFRelease(uuidstr);
    CFRelease(udid);
    return str;
}

#endif
