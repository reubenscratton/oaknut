//
// Copyright © 2014-2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_MACOS

#include "oaknut.h"

@interface NativeLayer :
#if RENDERER_METAL
    CAMetalLayer
#elif RENDERER_GL
    CAOpenGLLayer
#endif
{
    class Window* _window;
}
- (id)initWithWindow:(Window*)window;
@end

@interface NativeView : NSView {
    @public
    Window* _oaknutWindow;
    NSTouch* _touches[10];
    @public
    NativeLayer* _nativeLayer;
    POINT _deltaAcc;
}

- (id)initWithWindow:(Window*)window;
- (void)awake;

@end

#endif

