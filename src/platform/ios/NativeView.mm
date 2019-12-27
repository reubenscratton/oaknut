//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_IOS

#import "NativeView.h"


@implementation NativeView

+ (Class)layerClass {
#if RENDERER_GL
    return [CAEAGLLayer class];
#endif
#if RENDERER_METAL
    return [CAMetalLayer class];
#endif
}

#if RENDERER_GL
- (void)swapBuffers {
    [glContext presentRenderbuffer:GL_RENDERBUFFER];
    // GLKView does this for us
    //GLenum discards = GL_COLOR_ATTACHMENT0;
    //glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, &discards); // aka glInvalidateFramebuffer in ES 3.0
}
#endif

- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        _renderNeeded = YES;
#if RENDERER_GL
        glLayer = (CAEAGLLayer*)self.layer;
        glLayer.opaque = YES;
        glLayer.contentsScale = [UIScreen mainScreen].scale;//self.contentScaleFactor;
        glLayer.drawableProperties = @{
                                       kEAGLDrawablePropertyColorFormat:kEAGLColorFormatRGBA8,
                                       kEAGLDrawablePropertyRetainedBacking:@NO // todo: experiment with YES
                                       };
        
        glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        if (!glContext) {
            NSLog(@"Unable to create EAGLContext");
            exit(1);
        }
        if (![EAGLContext setCurrentContext:glContext]) {
            NSLog(@"Unable to set current EAGLContext");
            exit(1);
        }
        glGenRenderbuffers(1, &renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
        [glContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:glLayer];
        GLuint framebuffer;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer);
#endif
    }
    return self;
}



- (BOOL)canBecomeFirstResponder {
    return YES;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    int scale = [UIScreen mainScreen].scale;
    CGRect bounds = self.bounds;
    _window->resizeSurface(bounds.size.width * scale, bounds.size.height * scale);
}

- (void)handleTouches:(NSSet<UITouch *> *)touches eventType:(int)eventType remove:(BOOL)remove {
    for (UITouch* touch in touches) {
        CGPoint pt = [touch locationInView:self];
        
        int touchSlot = 9;
        for (int i=0 ; i<10 ; i++) {
            if (touch == _touches[i]) {
                if (remove) {
                    _touches[i] = nil;
                }
                touchSlot = i;
                break;
            }
            if  (i<touchSlot && !_touches[i]) {
                touchSlot = i;
            }
        }
        if (!remove) {
            _touches[touchSlot] = touch;
        }
        
        //dispatch_async(oakQueue, ^{
        INPUTEVENT inputEvent;
        inputEvent.deviceType = INPUTEVENT::Mouse;
        inputEvent.deviceIndex = 0;
        inputEvent.type = eventType;
        inputEvent.pt.x = pt.x*app->_defaultDisplay->_scale;
        inputEvent.pt.y = pt.y*app->_defaultDisplay->_scale;
        inputEvent.time = touch.timestamp*1000;
        _window->dispatchInputEvent(inputEvent);
        [self setNeedsDisplay];
        //});
    }
}


- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self handleTouches:touches eventType:INPUT_EVENT_DOWN remove:NO];
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self handleTouches:touches eventType:INPUT_EVENT_MOVE remove:NO];
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self handleTouches:touches eventType:INPUT_EVENT_UP remove:YES];
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self handleTouches:touches eventType:INPUT_EVENT_TAP_CANCEL remove:YES];
}


@end



#endif
