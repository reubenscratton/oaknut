//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_MACOS

#include "oaknut.h"

@interface NativeView :
#if RENDERER_GL
    NSOpenGLView
#else
    NSView //MTKView
#endif
{
    Window* _oaknutWindow;
    NSTouch* _touches[10];
#if RENDERER_METAL
    @public
    CAMetalLayer* _metalLayer;
    CVDisplayLinkRef _displayLink;
#endif
    POINT _deltaAcc;
}

- (id)initWithWindow:(Window*)window;
- (void)awake;

@end

#endif

