//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#import <UIKit/UIKit.h>
#include "oaknut.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;
@end

@interface OaknutView : UIView {
    CAEAGLLayer* glLayer;
    EAGLContext* glContext;
    CADisplayLink* displayLink;
    GLuint renderbuffer;
    UITouch* _touches[10];
    bool _calledMain;
    bool _renderNeeded;
}
@end

