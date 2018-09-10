//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_IOS

#import "AppDelegate.h"


extern dispatch_queue_t oakQueue;



@implementation OaknutView

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        _renderNeeded = YES;
    }
    return self;
}


- (void)didMoveToWindow {
    [super didMoveToWindow];
    //dispatch_async(oakQueue, ^{
        glLayer = (CAEAGLLayer *)self.layer;
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
        
    //});

    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render)];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

}


- (void)render {
    if (!_renderNeeded) {
        return;
    }
    _renderNeeded = NO;
    //dispatch_async(oakQueue, ^{
        
    
    if (!_calledMain) {
        app._window->_safeAreaInsets.top = [UIApplication sharedApplication].statusBarFrame.size.height * [UIScreen mainScreen].scale;
        app.main();
        _calledMain = true;
    }
    app._window->draw();
    
    [glContext presentRenderbuffer:GL_RENDERBUFFER];
    
    // GLKView does this for us
    //GLenum discards = GL_COLOR_ATTACHMENT0;
    //glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, &discards); // aka glInvalidateFramebuffer in ES 3.0
    //});
}

- (BOOL)canBecomeFirstResponder {
    return YES;
}

- (void)layoutSubviews {
	[super layoutSubviews];
	int scale = [UIScreen mainScreen].scale;
    CGRect bounds = self.bounds;
    //dispatch_async(oakQueue, ^{
        app._window->resizeSurface(bounds.size.width * scale, bounds.size.height * scale, scale);
    //});
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
            inputEvent.pt.x = pt.x*app._window->_scale;
            inputEvent.pt.y = pt.y*app._window->_scale;
            inputEvent.time = touch.timestamp*1000;
            app._window->dispatchInputEvent(inputEvent);
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
	[self handleTouches:touches eventType:INPUT_EVENT_CANCEL remove:YES];
}




/*
- (void)encodeWithCoder:(nonnull NSCoder *)aCoder {
}

+ (nonnull instancetype)appearance {
    return (OaknutView*)[UIApplication sharedApplication].delegate.window.rootViewController.view;
}

+ (nonnull instancetype)appearanceForTraitCollection:(nonnull UITraitCollection *)trait {
    return (OaknutView*)[UIApplication sharedApplication].delegate.window.rootViewController.view;
}

+ (nonnull instancetype)appearanceForTraitCollection:(nonnull UITraitCollection *)trait whenContainedIn:(nullable Class<UIAppearanceContainer>)ContainerClass, ... {
    return (OaknutView*)[UIApplication sharedApplication].delegate.window.rootViewController.view;
}

+ (nonnull instancetype)appearanceForTraitCollection:(nonnull UITraitCollection *)trait whenContainedInInstancesOfClasses:(nonnull NSArray<Class<UIAppearanceContainer>> *)containerTypes {
    return (OaknutView*)[UIApplication sharedApplication].delegate.window.rootViewController.view;
}

+ (nonnull instancetype)appearanceWhenContainedIn:(nullable Class<UIAppearanceContainer>)ContainerClass, ... {
    return (OaknutView*)[UIApplication sharedApplication].delegate.window.rootViewController.view;
}

+ (nonnull instancetype)appearanceWhenContainedInInstancesOfClasses:(nonnull NSArray<Class<UIAppearanceContainer>> *)containerTypes {
    return (OaknutView*)[UIApplication sharedApplication].delegate.window.rootViewController.view;
}

- (void)traitCollectionDidChange:(nullable UITraitCollection *)previousTraitCollection {
}

- (CGPoint)convertPoint:(CGPoint)point fromCoordinateSpace:(nonnull id<UICoordinateSpace>)coordinateSpace {
    return point;
}

- (CGPoint)convertPoint:(CGPoint)point toCoordinateSpace:(nonnull id<UICoordinateSpace>)coordinateSpace {
    return point;
}

- (CGRect)convertRect:(CGRect)rect fromCoordinateSpace:(nonnull id<UICoordinateSpace>)coordinateSpace {
    return rect;
}

- (CGRect)convertRect:(CGRect)rect toCoordinateSpace:(nonnull id<UICoordinateSpace>)coordinateSpace {
    return rect;
}

- (void)didUpdateFocusInContext:(nonnull UIFocusUpdateContext *)context withAnimationCoordinator:(nonnull UIFocusAnimationCoordinator *)coordinator {
}

- (void)setNeedsFocusUpdate {
}

- (BOOL)shouldUpdateFocusInContext:(nonnull UIFocusUpdateContext *)context {
    return NO;
}

- (void)updateFocusIfNeeded {
}*/

@end

static OaknutView* s_oaknutView;

void App::requestRedraw() {
    if (!s_oaknutView->_renderNeeded) {
        s_oaknutView->_renderNeeded = YES;
        //dispatch_async(dispatch_get_main_queue(), ^{
        [s_oaknutView setNeedsDisplay];
        //});
    }
}


@interface OaknutViewController : UIViewController
@end
@implementation OaknutViewController
- (UIStatusBarStyle) preferredStatusBarStyle {
    return UIStatusBarStyleLightContent;
}
@end



@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

	self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    s_oaknutView = [[OaknutView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	UIViewController * viewController = [[OaknutViewController alloc] initWithNibName:nil bundle:nil]; // 1
    viewController.view = s_oaknutView;
    self.window.rootViewController = viewController;
 
	app._window = new Window();
	app._window->_scale = s_oaknutView.contentScaleFactor;
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onKeyboardFrameChanged:) name:UIKeyboardWillChangeFrameNotification object:nil];
    
	
    self.window.backgroundColor = [UIColor whiteColor];
    [self.window makeKeyAndVisible];
	
    return YES;
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
