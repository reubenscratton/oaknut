
#include <TargetConditionals.h>

// Use Metal renderer on h/w, OpenGL in simulator only
#if !TARGET_OS_SIMULATOR
#undef RENDERER_GL
#undef RENDERER_METAL
#undef RENDERER_VULKAN
#undef RENDERER_DX
#define RENDERER_GL     0
#define RENDERER_METAL  1
#define RENDERER_VULKAN 0
#define RENDERER_DX     0
#endif

#if __OBJC__
 #include <CoreFoundation/CoreFoundation.h>
 #include <Foundation/Foundation.h>
 #include <QuartzCore/QuartzCore.h>
 #include <AVFoundation/AVFoundation.h>
 #if TARGET_OS_IOS
  #include <UIKit/UIKit.h>
 #else
  #include <AppKit/AppKit.h>
 #endif
#endif
#include <CoreText/CoreText.h>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreMedia/CoreMedia.h>
#include <CoreVideo/CoreVideo.h>

#if RENDERER_GL
 #if TARGET_OS_IOS
  #define GLES_SILENCE_DEPRECATION 1
  #include <OpenGLES/gltypes.h>
  #include <OpenGLES/ES3/gl.h>
  #include <OpenGLES/ES3/glext.h>
  #if __OBJC__
   #include <OpenGLES/EAGL.h>
  #endif
 #else
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <OpenGL/glext.h>
  #define glGenVertexArrays glGenVertexArraysAPPLE
  #define glBindVertexArray glBindVertexArrayAPPLE
 #endif
#endif

#if RENDERER_METAL
#if __OBJC__
#include <Metal/Metal.h>
#endif
#endif

#if TARGET_OS_IOS
#import <OpenAl/al.h>
#import <OpenAl/alc.h>
#define USE_NATIVE_IOS_AUDIO
#import <AudioUnit/AudioUnit.h>
#else
#import <OpenAL/OpenAL.h>
#endif
#include <objc/runtime.h>
#include <objc/message.h>


