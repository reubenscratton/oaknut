
#import "AppDelegate.h"
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import "oaknut.h"

#if TARGET_IOS
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#define glBindVertexArray glBindVertexArrayOES
#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#else
#import <OpenGL/OpenGL.h>
#if CGL_VERSION_1_3
#define ESSENTIAL_GL_PRACTICES_SUPPORT_GL3 1
#else
#define ESSENTIAL_GL_PRACTICES_SUPPORT_GL3 0
#endif
#import <OpenGL/gl.h>
#if ESSENTIAL_GL_PRACTICES_SUPPORT_GL3
#define glBindVertexArray glBindVertexArray
#define glGenVertexArrays glGenVertexArrays
#define glGenerateMipmap glGenerateMipmap
#define glDeleteVertexArrays glDeleteVertexArrays
#else
#define glBindVertexArray glBindVertexArrayAPPLE
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glGenerateMipmap glGenerateMipmapEXT
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif
#endif




@interface GLView : NSOpenGLView {
    NSTouch* _touches[10];
}

@end

static GLView* s_oaknutView;
static bool _calledMain = false;
static bool _renderNeeded;

#define SUPPORT_RETINA_RESOLUTION 1

@implementation GLView

- (void)handleTouches:(NSEvent*)event eventType:(int)eventType remove:(BOOL)remove {
    CGPoint pt = event.locationInWindow;
    pt.y = self.frame.size.height - pt.y;
    mainWindow->dispatchTouchEvent(eventType, 0, event.timestamp*1000, pt.x, pt.y);
    [self setNeedsDisplay:YES];
    /*NSSet<NSTouch*>* touches = event.allTouches;
    for (NSTouch* touch in touches) {
        oakLog("touch %d %f,%f", eventType, pt.x, pt.y);
        
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
        mainWindow->dispatchTouchEvent(eventType, touchSlot, event.timestamp*1000, pt.x, pt.y);
        [self setNeedsDisplay:YES];
        //});
    }*/
}

static bool s_mouseIsDown;

- (void)mouseDown:(NSEvent *)event {
    s_mouseIsDown = true;
    [self handleTouches:event eventType:TOUCH_EVENT_DOWN remove:NO];
}
- (void)mouseUp:(NSEvent *)event {
    s_mouseIsDown = false;
    [self handleTouches:event eventType:TOUCH_EVENT_UP remove:YES];
}
- (void)mouseMoved:(NSEvent *)event {
    [self handleTouches:event eventType:TOUCH_EVENT_MOVE remove:NO];
}

//- (void)touchesBeganWithEvent:(NSEvent*)event {
//}
- (void)touchesMovedWithEvent:(NSEvent*)event {
    if (s_mouseIsDown) {
//    oakLog("move!");
        [self handleTouches:event eventType:TOUCH_EVENT_MOVE remove:NO];
    }
}
//- (void)touchesEndedWithEvent:(NSEvent*)event {
//}

- (void)touchesCancelledWithEvent:(NSEvent*)event {
    [self handleTouches:event eventType:TOUCH_EVENT_CANCEL remove:YES];
}



- (void)awake {
    
    self.acceptsTouchEvents = YES;
    
    s_oaknutView = self;
    mainWindow = new Window();
    mainWindow->_scale = [NSScreen mainScreen].backingScaleFactor;

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
    
    mainWindow->resizeSurface(viewRectPixels.size.width, viewRectPixels.size.height, scale);

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
        oakMain();
        _calledMain = true;
    }
    _renderNeeded = NO;
    mainWindow->draw();
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


void oakRequestRedraw() {
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
