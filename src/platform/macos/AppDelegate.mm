//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_MACOS

#import "AppDelegate.h"




@interface GLView : NSOpenGLView {
    NSTouch* _touches[10];
}

@end

static GLView* s_oaknutView;
static bool _calledMain = false;
static bool _renderNeeded;
static bool s_mouseIsDown;

@implementation GLView

- (void)handleTouches:(NSEvent*)event eventType:(int)eventType remove:(BOOL)remove {
    CGPoint pt = event.locationInWindow;
    pt.y = self.frame.size.height - pt.y;
    app._window->dispatchInputEvent(eventType, MAKE_SOURCE(INPUT_SOURCE_TYPE_MOUSE, 0), event.timestamp*1000, pt.x*app._window->_scale, pt.y*app._window->_scale);
    [self setNeedsDisplay:YES];
}
- (void)mouseDown:(NSEvent *)event {
    s_mouseIsDown = true;
    [self handleTouches:event eventType:INPUT_EVENT_DOWN remove:NO];
}
- (void)mouseUp:(NSEvent *)event {
    s_mouseIsDown = false;
    [self handleTouches:event eventType:INPUT_EVENT_UP remove:YES];
}
- (void)mouseMoved:(NSEvent *)event {
    [self handleTouches:event eventType:INPUT_EVENT_MOVE remove:NO];
}

//- (void)touchesBeganWithEvent:(NSEvent*)event {
//}
- (void)touchesMovedWithEvent:(NSEvent*)event {
    if (s_mouseIsDown) {
        [self handleTouches:event eventType:INPUT_EVENT_MOVE remove:NO];
    }
}
//- (void)touchesEndedWithEvent:(NSEvent*)event {
//}
- (void)touchesCancelledWithEvent:(NSEvent*)event {
    [self handleTouches:event eventType:INPUT_EVENT_CANCEL remove:YES];
}



- (void)awake {
    
    self.acceptsTouchEvents = YES;
    
    s_oaknutView = self;

    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 0,
        NSOpenGLPFAOpenGLProfile,  NSOpenGLProfileVersionLegacy,//NSOpenGLProfileVersion3_2Core,
        0
    };

    NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    if (!pf) {
        NSLog(@"No OpenGL pixel format");
    }
    
    NSOpenGLContext* context = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];

#if ESSENTIAL_GL_PRACTICES_SUPPORT_GL3 && defined(DEBUG)
    CGLEnable([context CGLContextObj], kCGLCECrashOnRemovedFunctions);
#endif
    
    [self setPixelFormat:pf];
    [self setOpenGLContext:context];
    [self setWantsBestResolutionOpenGLSurface:YES];
    

}



- (void) prepareOpenGL {
    [super prepareOpenGL];
    [[self openGLContext] makeCurrentContext];
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                            selector:@selector(windowWillClose:)
                                                name:NSWindowWillCloseNotification
                                                    object:[self window]];

    [self setNeedsDisplay:YES];
}


- (void)windowWillClose:(NSNotification*)notification {
}


- (void)reshape {
    [super reshape];
    CGLLockContext([[self openGLContext] CGLContextObj]);

    NSRect viewRectPoints = [self bounds];
    NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
    
    CGFloat scale = viewRectPixels.size.width / viewRectPoints.size.width;
    
    app._window->resizeSurface(viewRectPixels.size.width, viewRectPixels.size.height, scale);

    CGLUnlockContext([[self openGLContext] CGLContextObj]);
}


- (void)renewGState {
    [[self window] disableScreenUpdatesUntilFlush];
    [super renewGState];
}

- (void)drawRect:(NSRect)dirtyRect {
    NSOpenGLContext* context = [self openGLContext];
    [context makeCurrentContext];
    CGLLockContext([context CGLContextObj]);
    if (!_calledMain) {
        _calledMain = true;
    }
    _renderNeeded = NO;
    app._window->draw();
    CGLFlushDrawable([context CGLContextObj]);
    CGLUnlockContext([context CGLContextObj]);
}

- (void)setNeedsDisplay2 {
    if (!_renderNeeded) {
        _renderNeeded = YES;
        dispatch_async(dispatch_get_main_queue(), ^{
            [self setNeedsDisplay:YES];
        });
    }
}

@end


void App::requestRedraw() {
    [s_oaknutView setNeedsDisplay2];
}

@interface AppDelegate ()

@end


@implementation AppDelegate


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    GLView* view =  [GLView new];
    self.window.contentView = view;
    [view awake];

}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

@end

#endif
