//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_MACOS

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"

int main(int argc, const char * argv[]) {
    NSApplication * application = [NSApplication sharedApplication];
    
    NSMenu* menubar = [NSMenu new];
    NSMenuItem* appMenuItem = [NSMenuItem new];
    [menubar addItem:appMenuItem];
    [NSApp setMainMenu:menubar];
    id appName = [[NSProcessInfo processInfo] processName];
    id quitTitle = [@"Quit " stringByAppendingString:appName];
    id quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle
                                                action:@selector(terminate:) keyEquivalent:@"q"];
    NSMenu* appMenu = [NSMenu new];
    [appMenu addItem:quitMenuItem];
    [appMenuItem setSubmenu:appMenu];
    AppDelegate* del = [AppDelegate new];
    del.window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 480, 720)
                                             styleMask:NSWindowStyleMaskTitled
                                               backing:NSBackingStoreBuffered
                                                 defer:NO];
    [del.window setStyleMask:[del.window styleMask] | NSWindowStyleMaskResizable];
    [del.window cascadeTopLeftFromPoint:NSMakePoint(20,20)];
    [del.window setTitle: [[NSProcessInfo processInfo] processName]];
    [del.window makeKeyAndOrderFront:nil];

    
    [application setDelegate:del];
    [application run];
    
    return EXIT_SUCCESS;
}

#endif
