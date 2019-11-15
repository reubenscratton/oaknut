//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_MACOS

#include "NativeView.h"


static bool s_mouseIsDown;



@implementation NativeView

- (id)initWithWindow:(Window*)window {
    if (self = [super initWithFrame:CGRectMake(0,0,300,300)]) {
        _oaknutWindow = window;
        CVDisplayLinkCreateWithActiveCGDisplays(&_displayLink);
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                selector:@selector(windowWillClose:)
                                                    name:NSWindowWillCloseNotification
                                                        object:[self window]];

    }
    return self;
}

- (void)dispatchInputEvent:(NSEvent*)event type:(int)type isScrollwheel:(BOOL)isScrollWheel {
    INPUTEVENT inputEvent;
    inputEvent.type = type;
    inputEvent.deviceIndex = 0;
    float scale = app->_defaultDisplay->_scale;
    CGPoint pt = event.locationInWindow;
    pt.y = self.frame.size.height - pt.y;
    inputEvent.pt.x = pt.x * scale;
    inputEvent.pt.y = pt.y * scale;
    inputEvent.time = event.timestamp*1000;
    if (isScrollWheel) {
        
        /** NB: Two-fingered drags on the Macbook touchpad result in "scrollwheel" events
         which makes sense. The "locationInWindow" property remains constant, as the mouse
         pointer is not moving, and the "scrollingDeltaX/Y" properties are relative to
         the previous event. We use this data to trivially derive the apparent mouse
         position, as if it were a touch event. We don't really have to care where the
         real mouse pointer is. */        
        
        POINT delta = {
            (float)(event.scrollingDeltaX * scale),
            (float)(event.scrollingDeltaY * scale)
        };
        inputEvent.deviceType = INPUTEVENT::ScrollWheel;
        if (inputEvent.type == INPUT_EVENT_DOWN) {
            _deltaAcc = {0,0};
        }
        _deltaAcc += delta;
        inputEvent.delta = delta;
        inputEvent.pt += _deltaAcc;
    } else {
        inputEvent.deviceType = INPUTEVENT::Mouse;
    }
    _oaknutWindow->dispatchInputEvent(inputEvent);
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
    } else if (event.phase == NSEventPhaseCancelled) {
        type = INPUT_EVENT_CANCEL;
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

- (void)windowWillClose:(NSNotification*)notification {
}

#if RENDERER_METAL

// This is the renderer output callback function
static CVReturn DisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
    NativeView* view = (__bridge NativeView*)displayLinkContext;
    dispatch_async(dispatch_get_main_queue(), ^() {
        [view->_metalLayer setNeedsDisplay];
    });
    return noErr;
}



- (void)awake {
    
    self.acceptsTouchEvents = YES;
    
    CVDisplayLinkSetOutputCallback(_displayLink, &DisplayLinkCallback, (__bridge void*)self);
    
    // NSViewLayerContentsRedrawOnSetNeedsDisplay
    [self setLayerContentsRedrawPolicy:NSViewLayerContentsRedrawNever];

    CVDisplayLinkStart(_displayLink);
}


- (CALayer*)makeBackingLayer {
    return _metalLayer;
}

- (void)displayLayer:(CALayer *)layer {
    _oaknutWindow->draw();
}

- (void)setFrameSize:(NSSize)newSize {
    [super setFrameSize:newSize];

    NSRect viewRectPoints = [self bounds];
    NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
    
    //CGFloat scale = viewRectPixels.size.width / viewRectPoints.size.width;
    
    self.layer.bounds = self.bounds;
    _metalLayer.bounds = self.bounds;
    _metalLayer.drawableSize = viewRectPixels.size;

    _oaknutWindow->resizeSurface(viewRectPixels.size.width, viewRectPixels.size.height);
    
    [self setNeedsDisplay:YES];
}

- (void)_windowWillClose:(NSNotification*)notification {
    CVDisplayLinkStop(_displayLink);
}
#endif


#if RENDERER_GL
- (void)awake {
    
    self.acceptsTouchEvents = YES;
    
    [self setLayerContentsRedrawPolicy:NSViewLayerContentsRedrawNever];

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
    
    [context makeCurrentContext];

}


- (void) prepareOpenGL {
    [super prepareOpenGL];
    [[self openGLContext] makeCurrentContext];
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

    [self setNeedsDisplay:YES];
}



- (void)reshape {
    [super reshape];
    CGLLockContext([[self openGLContext] CGLContextObj]);

    NSRect viewRectPoints = [self bounds];
    NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
    
    CGFloat scale = viewRectPixels.size.width / viewRectPoints.size.width;
    
    _oaknutWindow->resizeSurface(viewRectPixels.size.width, viewRectPixels.size.height);

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
    _oaknutWindow->draw();
    CGLFlushDrawable([context CGLContextObj]);
    CGLUnlockContext([context CGLContextObj]);
}
#endif


@end



#endif
