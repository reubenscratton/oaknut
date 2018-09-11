//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_IOS

#import "AppDelegate.h"
#import "NativeView.h"

extern dispatch_queue_t oakQueue;

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
    return UIStatusBarStyleLightContent;
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

class WindowIOS : public Window {
public:
    
    WindowIOS() {
        _nativeWindow = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
        _nativeWindow.backgroundColor = [UIColor whiteColor];
        _nativeView = [[NativeView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
        UIViewController * viewController = [[NativeViewController alloc] initWithNibName:nil bundle:nil];
        viewController.view = _nativeView;
        _nativeWindow.rootViewController = viewController;
        _scale = _nativeView.contentScaleFactor;
    }
    
    void show() override {
        [_nativeWindow makeKeyAndVisible];
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


    UIWindow* _nativeWindow;
    NativeView* _nativeView;
    
};





@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
	app._window = new WindowIOS();
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
