//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_IOS

#include <oaknut.h>
#include "SoftKeyboardTracker.h"


@implementation SoftKeyboardTracker

+ (SoftKeyboardTracker*)runForNotification:(NSNotification*)notification {
    NSNumber *duration = [notification.userInfo objectForKey:UIKeyboardAnimationDurationUserInfoKey];
    
    SoftKeyboardTracker* tracker = [SoftKeyboardTracker new];
    tracker->firstFrame = 0;
    tracker->duration = duration.doubleValue;
    tracker->curve = ((NSNumber*)[notification.userInfo objectForKey: UIKeyboardAnimationCurveUserInfoKey]).integerValue;
    tracker->fromRect = [((NSValue*)notification.userInfo[UIKeyboardFrameBeginUserInfoKey]) CGRectValue];
    tracker->toRect = ((NSValue*)notification.userInfo[UIKeyboardFrameEndUserInfoKey]).CGRectValue;
    tracker->displayLink = [CADisplayLink displayLinkWithTarget:tracker selector:@selector(tick:)];
    [tracker->displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
    
    return tracker;
}

- (void)tick:(CADisplayLink *)sender {
    auto now = CACurrentMediaTime();
    if (firstFrame == 0) {
        firstFrame = now;
    }
    
    auto elapsedTime = now  - firstFrame;
    bool finished = false;
    if (elapsedTime >= duration) {
        elapsedTime = duration;
        finished = true;
    }
    CGFloat val = elapsedTime / duration;

    // TODO: reverse engineer the private interpolator iOS uses here
    float keyboardTop = fromRect.origin.y + val * (toRect.origin.y-fromRect.origin.y);
    keyboardTop *= app->_defaultDisplay->_scale;
    float keyboardHeight = app->_window->getHeight() - keyboardTop;
    
    app->_window->setSoftKeyboardInsets(EDGEINSETS(0,0,0,keyboardHeight));
    
    if (finished) {
        [displayLink invalidate];
        displayLink = nil;
    }
}
- (void)cancel {
    if (displayLink) {
        [displayLink invalidate];
        displayLink = nil;
    }
}

@end


#endif

