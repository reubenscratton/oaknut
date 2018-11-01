//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_IOS

#import <UIKit/UIKit.h>
#include "oaknut.h"

@interface SoftKeyboardTracker : NSObject {
    double duration;
    NSInteger curve;
    CGRect fromRect;
    CGRect toRect;
    CADisplayLink* displayLink;
    NSTimeInterval firstFrame;
    //NativeView* view;
}
+ (SoftKeyboardTracker*)runForNotification:(NSNotification*)notification;
- (void)cancel;
@end

#endif

