#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <emscripten/val.h>
#include <emscripten/threading.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#include <libc/pthread.h>
#include <AL/al.h>
#include <AL/alc.h>
using namespace emscripten;
