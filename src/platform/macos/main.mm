//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_MACOS

#import <Cocoa/Cocoa.h>
#include <oaknut.h>
#include "NativeView.h"


@interface NativeWindow : NSWindow {
@public
    Window* _window;
}
@end

@implementation NativeWindow

- (void)handleKey:(KeyboardInputEventType)eventType event:(NSEvent*)event {
    if (_window->_keyboardHandler) {
        char32_t charCode = event.characters.length ? [event.characters characterAtIndex:0] : 0;
        KeyboardInputSpecialKeyCode sk = SpecialKeyNone;
        if (event.keyCode == 123) sk = SpecialKeyCursorLeft;
        else if (event.keyCode == 124) sk = SpecialKeyCursorRight;
        else if (event.keyCode == 125) sk = SpecialKeyCursorDown;
        else if (event.keyCode == 126) sk = SpecialKeyCursorUp;
        else if (event.keyCode == 51) sk = SpecialKeyDelete;
        app->_window->_keyboardHandler->keyInputEvent(eventType, sk, event.keyCode, charCode);
    }
}
- (void)keyDown:(NSEvent *)event {
    [self handleKey:KeyDown event:event];
    //NSLog(@"keyDown: %@", event);
}
- (void)keyUp:(NSEvent *)event {
    [self handleKey:KeyUp event:event];
}
- (void)flagsChanged:(NSEvent *)event {
    if (_window->_keyboardHandler) {
        if (event.keyCode == 55 || event.keyCode == 54) { // Cmd or CmdRight
            bool down = (event.modifierFlags & NSEventModifierFlagCommand);
            _window->_keyboardHandler->keyInputEvent(down?KeyDown:KeyUp, SpecialKeyCommand, event.keyCode, 0);
        }
        if (event.keyCode == 56 || event.keyCode == 60) { // Shift & ShiftRt. Gawd knows where this enum is hiding...
            bool shiftDown = (event.modifierFlags & NSEventModifierFlagShift);
            _window->_keyboardHandler->keyInputEvent(shiftDown?KeyDown:KeyUp, SpecialKeyShift, event.keyCode, 0);
        } else if (event.keyCode == 57) { // Caps Lock
            // We don't get separate down/up events for CapsLock so lets synthesize them
            _window->_keyboardHandler->keyInputEvent(KeyDown, SpecialKeyCapsLock, event.keyCode, 0);
            _window->_keyboardHandler->keyInputEvent(KeyUp, SpecialKeyCapsLock, event.keyCode, 0);
        }
    }
}
@end

class WindowOSX : public Window {
public:
    WindowOSX() {
        _nativeView =  [[NativeView alloc] initWithWindow:this];
        [_nativeView awake];
        _surface->bindToNativeWindow((long)(__bridge void*)_nativeView);
    }
    
    void show() override {
        Window::show();
        float width = app->getStyleFloat("window.default-width") / app->_defaultDisplay->_scale;
        float height = app->getStyleFloat("window.default-height") / app->_defaultDisplay->_scale;
        _nativeWindow = [[NativeWindow alloc] initWithContentRect:NSMakeRect(0, 0, width, height)
                                                        styleMask:NSWindowStyleMaskTitled
                                                          backing:NSBackingStoreBuffered
                                                            defer:NO];
        _nativeWindow->_window = this;
        [_nativeWindow setStyleMask:[_nativeWindow styleMask] | NSWindowStyleMaskResizable];
        [_nativeWindow cascadeTopLeftFromPoint:NSMakePoint(20,20)];
        [_nativeWindow setTitle: [[NSProcessInfo processInfo] processName]];
        _nativeWindow.contentView = _nativeView;

        [_nativeWindow makeFirstResponder:_nativeWindow];
        [_nativeWindow makeKeyAndOrderFront:nil];
    }
    
    void requestRedrawNative() override {
        dispatch_async(dispatch_get_main_queue(), ^{
            _redrawNeeded = NO;
            draw();
        });
    }
    
    NativeView* _nativeView;
    NativeWindow* _nativeWindow;
};


@interface AppDelegate : NSObject <NSApplicationDelegate>

@end


Window* Window::create() {
    return new WindowOSX();
}

@implementation AppDelegate


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    
    // Create the default Display
    NSScreen* screen = [NSScreen mainScreen];
    NSDictionary *description = [screen deviceDescription];
    NSSize displayPixelSize = [[description objectForKey:NSDeviceSize] sizeValue];
    CGSize displayPhysicalSize = CGDisplayScreenSize(
        [[description objectForKey:@"NSScreenNumber"] unsignedIntValue]
    );
    float inchesX = displayPhysicalSize.width / 25.4f; // there being 25.4 mm in an inch
    float inchesY = displayPhysicalSize.height / 25.4f;
    float dpiX = (displayPixelSize.width * screen.backingScaleFactor) / inchesX;
    float dpiY = (displayPixelSize.height * screen.backingScaleFactor) / inchesY;
    app->_defaultDisplay = new Display(screen.frame.size.width,
                                       screen.frame.size.height,
                                       dpiX, dpiY,
                                       screen.backingScaleFactor);
    
    

    // Got to create app object before we create any native elements so we can get at style system
    app->_window = Window::create();
    app->loadStyleAssetSync("styles.res");
    app->_window->show();
    app->main();
    
    // Create app menu
    NSMenu* menubar = [NSMenu new];
    NSMenuItem* appMenuItem = [NSMenuItem new];
    [menubar addItem:appMenuItem];
    [NSApp setMainMenu:menubar];
    id appName = [[NSProcessInfo processInfo] processName];
    id quitTitle = [@"Quit " stringByAppendingString:appName];
    id quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle action:@selector(terminate:) keyEquivalent:@"q"];
    NSMenu* appMenu = [NSMenu new];
    [appMenu addItem:quitMenuItem];
    [appMenuItem setSubmenu:appMenu];
    
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

@end


int main(int argc, const char * argv[]) {
    NSApplication * application = [NSApplication sharedApplication];
    AppDelegate* del = [AppDelegate new];
    [application setDelegate:del];
    [application run];
    return EXIT_SUCCESS;
}

#endif
