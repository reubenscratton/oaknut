//
// Copyright © 2014-2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_MACOS

#include "NativeView.h"


static bool s_mouseIsDown;


@implementation NativeLayer

- (id)initWithWindow:(Window*)window {
    if (self=[super init]) {
        _window = window;
        self.contentsScale = app->_defaultDisplay->_scale;
        self.opaque = YES;
        
#if RENDERER_GL
        // Create and set a CGL context immediately
        CGLPixelFormatAttribute attributes[] = {
            kCGLPFAAccelerated,
            kCGLPFADoubleBuffer,
            kCGLPFADepthSize, CGLPixelFormatAttribute(0),
            kCGLPFAOpenGLProfile, CGLPixelFormatAttribute(kCGLOGLPVersion_Legacy),
            CGLPixelFormatAttribute(0)
        };
        CGLPixelFormatObj pixelFormat = NULL;
        GLint numPixelFormats = 0;
        CGLChoosePixelFormat(attributes, &pixelFormat, &numPixelFormats);
        assert(pixelFormat);
        CGLContextObj ctx = NULL;
        CGLCreateContext(pixelFormat, NULL, &ctx);
        assert(ctx);
        GLint swapInt = 1;
        CGLSetParameter(ctx, kCGLCPSwapInterval, &swapInt);
        CGLSetCurrentContext(ctx);
#endif
    }
    return self;
}

#if RENDERER_GL
/*- (CGLPixelFormatObj)copyCGLPixelFormatForDisplayMask:(uint32_t)mask {
    CGLPixelFormatAttribute attributes[] = {
        kCGLPFADisplayMask, CGLPixelFormatAttribute(mask),
        kCGLPFAAccelerated,
        kCGLPFADoubleBuffer,
        kCGLPFADepthSize, CGLPixelFormatAttribute(0),
        kCGLPFAOpenGLProfile, CGLPixelFormatAttribute(kCGLOGLPVersion_Legacy),
        CGLPixelFormatAttribute(0)
    };
    CGLPixelFormatObj pixelFormatObj = NULL;
    GLint numPixelFormats = 0;
    CGLChoosePixelFormat(attributes, &pixelFormatObj, &numPixelFormats);
    assert(pixelFormatObj);
    return pixelFormatObj;
}*/

- (CGLContextObj)copyCGLContextForPixelFormat:(CGLPixelFormatObj)pf {
    //CGLContextObj ctx = [super copyCGLContextForPixelFormat:pf];
    return  CGLGetCurrentContext();
}

- (BOOL)isAsynchronous {
    return YES;
}

- (BOOL)canDrawInCGLContext:(CGLContextObj)ctx
                pixelFormat:(CGLPixelFormatObj)pf
               forLayerTime:(CFTimeInterval)t
                displayTime:(const CVTimeStamp *)ts {
    return _window->_redrawNeeded;
}

- (void)drawInCGLContext:(CGLContextObj)ctx
 pixelFormat:(CGLPixelFormatObj)pf
forLayerTime:(CFTimeInterval)t
             displayTime:(const CVTimeStamp *)ts {
    _window->draw();
    [super drawInCGLContext:ctx pixelFormat:pf forLayerTime:t displayTime:ts];
}
#endif

@end



@implementation NativeView

- (id)initWithWindow:(Window*)window {
    if (self = [super initWithFrame:CGRectMake(0,0,300,300)]) {
        _oaknutWindow = window;
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                selector:@selector(windowWillClose:)
                                                    name:NSWindowWillCloseNotification
                                                        object:[self window]];
        
        [self setWantsBestResolutionOpenGLSurface:YES];
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
        //inputEvent.delta = delta;
        inputEvent.pt += _deltaAcc;
    } else {
        inputEvent.deviceType = INPUTEVENT::Mouse;
    }
    _oaknutWindow->dispatchInputEvent(inputEvent);
    // [self setNeedsDisplay:YES];
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
        type = INPUT_EVENT_TAP_CANCEL;
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
    [self dispatchInputEvent:event type:INPUT_EVENT_TAP_CANCEL isScrollwheel:NO];

}

- (void)windowWillClose:(NSNotification*)notification {
}

- (void)awake {
    
    self.allowedTouchTypes = NSTouchTypeIndirect;

    // This tells AppKit to not interfere with or otherwise try to manage our layer. It won't waste time
    // clearing, or scaling, etc.
    [self setLayerContentsRedrawPolicy:NSViewLayerContentsRedrawNever];
    self.wantsLayer = YES;
    
}

- (void)setFrameSize:(NSSize)newSize {
    [super setFrameSize:newSize];

    NSRect viewRectPoints = [self bounds];
    NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
    
    self.layer.bounds = self.bounds;
    _nativeLayer.bounds = self.bounds;
#if RENDERER_METAL
    _nativeLayer.drawableSize = viewRectPixels.size;
#endif
    _oaknutWindow->resizeSurface(viewRectPixels.size.width, viewRectPixels.size.height);
    _oaknutWindow->setNeedsFullRedraw();
}


- (CALayer*)makeBackingLayer {
    if (!_nativeLayer) {
        _nativeLayer = [[NativeLayer alloc] initWithWindow:_oaknutWindow];
    }
    return _nativeLayer;
}

- (void)_windowWillClose:(NSNotification*)notification {
}


- (void)renewGState {
    [[self window] disableScreenUpdatesUntilFlush];
    [super renewGState];
}



@end



#endif
