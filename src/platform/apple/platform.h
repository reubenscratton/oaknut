#include <TargetConditionals.h>
#if __OBJC__
#include <CoreFoundation/CoreFoundation.h>
#endif
#if TARGET_OS_IOS
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
#include <objc/runtime.h>
#include <objc/message.h>
