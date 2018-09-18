//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_IOS

#import <UIKit/UIKit.h>
#include "oaknut.h"

@interface NativeView : UIView {
@public
    CAEAGLLayer* glLayer;
    EAGLContext* glContext;
    CADisplayLink* displayLink;
    GLuint renderbuffer;
    UITouch* _touches[10];
    bool _renderNeeded;
    id<UITextInputDelegate> _textInputDelegate;
    Window* _window;
}
@end
@interface NativeView (TextInput) <UITextInput, UITextInputTraits>
@end


#endif
