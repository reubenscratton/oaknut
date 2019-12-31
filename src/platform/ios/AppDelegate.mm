//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_IOS

#import "AppDelegate.h"
#import <sys/utsname.h>
#import "NativeView.h"
#import "SoftKeyboardTracker.h"

#define USE_DISPLAY_LINK 1

@interface NativeViewController : UIViewController {
    SoftKeyboardTracker* currentKeyboardTracker;
}
@end
@implementation NativeViewController
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self=[super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onKeyboardWillChange:) name:UIKeyboardWillChangeFrameNotification object:nil];
        
    }
    return self;
}
- (UIStatusBarStyle) preferredStatusBarStyle {
    auto val = app->getStyle("window.status-bar-light");
    if (!val) {
        return UIStatusBarStyleDefault;
    }
    return val->boolVal() ? UIStatusBarStyleLightContent : UIStatusBarStyleDefault;
}
- (void)onKeyboardWillChange:(NSNotification*)notification {
    if (currentKeyboardTracker) {
        [currentKeyboardTracker cancel];
    }
    currentKeyboardTracker = [SoftKeyboardTracker runForNotification:notification];
}

@end

static UIWindow* _nativeWindow;
static bool _nativeWindowOwnedByOaknut;

void ensureNativeWindowExists() {
    if (!_nativeWindow) {
        _nativeWindow = [UIApplication sharedApplication].keyWindow;
        if (!_nativeWindow) {
            _nativeWindowOwnedByOaknut = true;
            _nativeWindow = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
            _nativeWindow.backgroundColor = [UIColor whiteColor];
            [_nativeWindow makeKeyAndVisible];
        }

    }
}

#if USE_DISPLAY_LINK
@interface DisplayLinkHelper : NSObject {
    CADisplayLink* _displayLink;
    Window* _window;
}
- (id)initWithWindow:(Window*)window;
@end
#endif


class WindowIOS : public Window {
public:
#if USE_DISPLAY_LINK
    DisplayLinkHelper* _displayLinkHelper;
#endif
    WindowIOS() {
        ensureNativeWindowExists();
        _nativeView = [[NativeView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
        _nativeView->_window = this;
        UIEdgeInsets osInsets = _nativeWindow.safeAreaInsets;
        _viewController = [[NativeViewController alloc] initWithNibName:nil bundle:nil];
        _viewController.view = _nativeView;
        setSafeInsets(EDGEINSETS(osInsets.left * app->_defaultDisplay->_scale,
                                 osInsets.top * app->_defaultDisplay->_scale,
                                 osInsets.right * app->_defaultDisplay->_scale,
                                 osInsets.bottom * app->_defaultDisplay->_scale));
        _renderer->bindToNativeWindow((long)(__bridge void*)_nativeView);
#if USE_DISPLAY_LINK
        _displayLinkHelper = [[DisplayLinkHelper alloc] initWithWindow:this];
#endif
    }
    
    void show() override {
        Window::show();
        if (_nativeWindowOwnedByOaknut) {
            _nativeWindow.rootViewController = _viewController;
        } else {
            [_nativeWindow.rootViewController presentViewController:_viewController animated:YES completion:nil];
        }
    }
    
    void requestRedrawNative() override {
#if USE_DISPLAY_LINK
        // TODO: start link if not active and stop when inactive. At present link runs continually, wasting CPU.
#else
        dispatch_async(dispatch_get_main_queue(), ^{
            draw();
#if RENDERER_GL
            [_nativeView swapBuffers];
#endif
        });
#endif
    }
    

    
    void keyboardShow(bool show) override {
        if (show) {
            [_nativeView becomeFirstResponder];
        } else {
            [_nativeView resignFirstResponder];
        }
    }
    void keyboardNotifyTextChanged() override {
        if (_nativeView->_textInputDelegate) {
            [_nativeView->_textInputDelegate selectionWillChange: _nativeView];
            [_nativeView->_textInputDelegate textWillChange: _nativeView];
            [_nativeView->_textInputDelegate selectionDidChange: _nativeView];
            [_nativeView->_textInputDelegate textDidChange: _nativeView];
        }
    }
    
    bool setFocusedView(View* view) override {
        if (Window::setFocusedView(view)) {
            if ([_nativeView isFirstResponder]) {
                [_nativeView resignFirstResponder];
                [_nativeView becomeFirstResponder];
            }
            return true;
        }
        return false;
    }
    
    /**
     Permissions
     */
    virtual bool hasPermission(Permission permission) override {
        if (permission == PermissionCamera) {
            AVAuthorizationStatus status = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
            return status == AVAuthorizationStatusAuthorized;
        }
        if (permission == PermissionMic) {
            AVAuthorizationStatus status = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio];
            return status == AVAuthorizationStatusAuthorized;
        }
        assert(0); // unknown permission
        return false;
    }
    
    class PermissionRequest {
    public:
        vector<Permission> _permissions;
        std::function<void(vector<bool>)> _callback;
        int _index;
        vector<bool> _results;
        PermissionRequest(const vector<Permission>& permissions, const std::function<void(vector<bool>)>& callback) {
            _permissions = permissions;
            _callback = callback;
            _index = 0;
        }
        void addResult(BOOL result) {
            _results.push_back(result);
            if (_results.size() >= _permissions.size()) {
                _callback(_results);
                delete this;
            } else {
                drain();
            }
        }
        void drain() {
            Permission permission = _permissions[_index++];
            if (permission == PermissionCamera) {
                [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo completionHandler:^(BOOL granted) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        addResult(granted);
                    });
                }];
            }
            else if (permission == PermissionMic) {
                [AVCaptureDevice requestAccessForMediaType:AVMediaTypeAudio completionHandler:^(BOOL granted) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        addResult(granted);
                    });
                }];
            }
            else {
                assert(0); // unknown permission
            }
        }
    };
    
    virtual void runWithPermissions(vector<Permission> permissions, std::function<void(vector<bool>)> callback) override {
        PermissionRequest* req = new PermissionRequest(permissions, callback);
        req->drain();
    }

    UIViewController* _viewController;
    NativeView* _nativeView;
    
};


Window* Window::create() {
    return new WindowIOS();
}


@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    float ppi = 326;
    
    string machine;
#if TARGET_OS_SIMULATOR
    machine = getenv("SIMULATOR_MODEL_IDENTIFIER");
#else
    struct utsname systemInfo;
    uname(&systemInfo);
    machine = systemInfo.machine;
#endif
    bool iPhone = machine.hadPrefix("iPhone");
    bool iPad = machine.hadPrefix("iPad");
    if (iPhone || iPad) {
        auto verstr = machine.split(",");
        int ver = (verstr[0].asInt() << 8) | verstr[1].asInt();
        if (iPhone) {
            switch (ver) {
                case 0x701: ppi = 401; break; // iPhone 6+
                case 0x802: ppi = 401; break; // iPhone 6s+
                case 0x902: ppi = 401; break; // iPhone 7+
                case 0x904: ppi = 401; break; // iPhone 7+
                case 0xA02: ppi = 401; break; // iPhone 8+
                case 0xA05: ppi = 401; break; // iPhone 8+
                case 0xA03: ppi = 458; break; // iPhone X
                case 0xA06: ppi = 458; break; // iPhone X
                case 0xB02: ppi = 458; break; // iPhone XS
                case 0xB04: ppi = 458; break; // iPhone XS Max
                case 0xB06: ppi = 458; break; // iPhone XS Max
                case 0xC03: ppi = 458; break; // iPhone 11 Pro
                case 0xC05: ppi = 458; break; // iPhone 11 Pro Max
                default:
                    ppi = 326;
                    break;
            }
        }
        if (iPad) {
            switch (ver) {
                case 0x101:                         // iPad 1
                case 0x201: case 0x202: case 0x203: case 0x204: // iPad 2
                    ppi = 132; break;
                case 0x205: case 0x206: case 0x207: // iPad Mini
                    ppi = 163; break;
                case 0x404: case 0x405: case 0x406: // iPad Mini 2
                case 0x407: case 0x408: case 0x409: // iPad Mini 3
                case 0x501: case 0x502:             // iPad Mini 4
                case 0xB01: case 0xB02:             // iPad Mini 5
                    ppi = 326; break;
                default:
                    ppi = 264;
                    break;
            }

        }
    }
    
    // Create the default Display
    UIScreen* screen = [UIScreen mainScreen];
    //float dpiX = screen.nativeBounds.size.width / screen.bounds.size.width;
    //float dpiY = screen.nativeBounds.size.height / screen.bounds.size.height;
    app->_defaultDisplay = new Display(screen.bounds.size.width,
                                       screen.bounds.size.height,
                                       ppi, ppi,
                                       screen.scale);
    

    app->_window = Window::create();
    app->loadStyleAsset("styles.res");
    app->main();
    app->_window->show();
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
}

- (void)applicationWillTerminate:(UIApplication *)application {
}

@end

#if USE_DISPLAY_LINK
@implementation DisplayLinkHelper
- (id)initWithWindow:(Window*)window {
    if (self = [super init]) {
        _window = window;
        _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(tick)];
        [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
    return self;
}
- (void)tick {
    if (_window->_redrawNeeded) {
        _window->_redrawNeeded = false;
        _window->draw();
#if RENDERER_GL
        [((WindowIOS*)_window)->_nativeView swapBuffers];
#endif
    }
}
@end
#endif

#endif
