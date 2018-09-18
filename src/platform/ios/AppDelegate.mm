//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_IOS

#import "AppDelegate.h"
#import "NativeView.h"


@interface NativeViewController : UIViewController
@end
@implementation NativeViewController
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self=[super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onKeyboardFrameChanged:) name:UIKeyboardWillChangeFrameNotification object:nil];

    }
    return self;
}
- (UIStatusBarStyle) preferredStatusBarStyle {
    return UIStatusBarStyleDefault;
}
- (void)onKeyboardFrameChanged:(NSNotification*)notification {
    CGRect keyboardFrame = ((NSValue*)notification.userInfo[UIKeyboardFrameEndUserInfoKey]).CGRectValue;
    RECT rect;
    float scale = [UIScreen mainScreen].scale;
    rect.origin.x = keyboardFrame.origin.x * scale;
    rect.origin.y = keyboardFrame.origin.y * scale;
    rect.size.width = keyboardFrame.size.width * scale;
    rect.size.height = keyboardFrame.size.height * scale;
    app._window->setSoftKeyboardRect(rect);
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

class WindowIOS : public Window {
public:
    
    WindowIOS() {
        ensureNativeWindowExists();
        _nativeView = [[NativeView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
        _nativeView->_window = this;
        _viewController = [[NativeViewController alloc] initWithNibName:nil bundle:nil];
        _viewController.view = _nativeView;
        _scale = [UIScreen mainScreen].scale;
        _safeAreaInsets.top = [UIApplication sharedApplication].statusBarFrame.size.height * _scale;

    }
    
    void show() override {
        if (_nativeWindowOwnedByOaknut) {
            _nativeWindow.rootViewController = _viewController;
        } else {
            [_nativeWindow.rootViewController presentViewController:_viewController animated:YES completion:nil];
        }
    }
    
    void requestRedrawNative() override {
        if (!_nativeView->_renderNeeded) {
            _nativeView->_renderNeeded = YES;
            [_nativeView setNeedsDisplay];
        }
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
            [_nativeView->_textInputDelegate textWillChange: _nativeView];
            [_nativeView->_textInputDelegate textDidChange: _nativeView];
        }
    }

    UIViewController* _viewController;
    NativeView* _nativeView;
    
};


Window* Window::create() {
    return new WindowIOS();
}


@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    app._window = Window::create();
    app.loadStyleAsset("styles.res");
    app.main();
    app._window->show();
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

#endif
