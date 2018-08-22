//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
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

- (void)dispatchInputEvent:(NSEvent*)event type:(int)type isScrollwheel:(BOOL)isScrollWheel {
    INPUTEVENT inputEvent;
    inputEvent.type = type;
    inputEvent.deviceIndex = 0;
    CGPoint pt = event.locationInWindow;
    pt.y = self.frame.size.height - pt.y;
    inputEvent.pt.x = pt.x*app._window->_scale;
    inputEvent.pt.y = pt.y*app._window->_scale;
    inputEvent.time = event.timestamp*1000;
    if (isScrollWheel) {
        inputEvent.deviceType = INPUTEVENT::ScrollWheel;
        inputEvent.delta = {
            (float)(event.scrollingDeltaX*app._window->_scale),
            (float)(event.scrollingDeltaY*app._window->_scale)};
    } else {
        inputEvent.deviceType = INPUTEVENT::Mouse;
    }
    app._window->dispatchInputEvent(inputEvent);
    [self setNeedsDisplay:YES];
}
- (void)mouseDown:(NSEvent *)event {
    s_mouseIsDown = true;
    [self dispatchInputEvent:event type:INPUT_EVENT_DOWN isScrollwheel:NO];
}
- (void)mouseUp:(NSEvent *)event {
    s_mouseIsDown = false;
    [self dispatchInputEvent:event type:INPUT_EVENT_UP isScrollwheel:NO];
}
- (void)mouseMoved:(NSEvent *)event {
    [self dispatchInputEvent:event type:INPUT_EVENT_MOVE isScrollwheel:NO];
}
- (void)scrollWheel:(NSEvent *)event {
    int type;
    // NB: OS X generates momentum scroll events which clashes with Oaknut's 'fling' implementation.
    // Easiest thing is to filter them out and only handle events from while the user is touching the touchpad.
    if (event.phase == NSEventPhaseBegan) {
        type = INPUT_EVENT_DOWN;
    } else if (event.phase == NSEventPhaseEnded) {
        type = INPUT_EVENT_UP;
    } else if (event.phase == NSEventPhaseChanged) {
        type = INPUT_EVENT_MOVE;
    } else {
        return;
    }
    [self dispatchInputEvent:event type:type isScrollwheel:YES];
}

// TODO: Do we ever want mouseover events? I imagine we do so rework this...
//- (void)touchesBeganWithEvent:(NSEvent*)event {
//}
- (void)touchesMovedWithEvent:(NSEvent*)event {
    if (s_mouseIsDown) {
        [self dispatchInputEvent:event type:INPUT_EVENT_MOVE isScrollwheel:NO];
    }
}
//- (void)touchesEndedWithEvent:(NSEvent*)event {
//}
- (void)touchesCancelledWithEvent:(NSEvent*)event {
    [self dispatchInputEvent:event type:INPUT_EVENT_CANCEL isScrollwheel:NO];

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
