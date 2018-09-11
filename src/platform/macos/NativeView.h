//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_MACOS

#import <Cocoa/Cocoa.h>
#include "oaknut.h"

@interface NativeView : NSOpenGLView {
    NSTouch* _touches[10];
}

- (void)awake;

@end

#endif

