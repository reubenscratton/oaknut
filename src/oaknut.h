// MIT License
//
// Copyright (c) 2017 Sandcastle Software Ltd
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef _OAKNUT_H_INCLUDED_
#define _OAKNUT_H_INCLUDED_


#include <stdio.h>
#ifdef __cplusplus
#include <cmath>
#else
#include <math.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <cstdarg>
#include <typeinfo>
#include <map>
#include <unordered_map>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <stack>
#include <algorithm>
#include <set>
#include <functional>
#include <assert.h>

using namespace std;

#ifdef EMSCRIPTEN
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/threading.h>
//#include <GL/glut.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#include <libc/pthread.h>
using namespace emscripten;
#else
#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif __APPLE__
#include <TargetConditionals.h>
#if __OBJC__
 #include <CoreFoundation/CoreFoundation.h>
#endif
 #if TARGET_OS_IOS
  #include <OpenGLES/ES3/gl.h>
  #include <OpenGLES/ES3/glext.h>
  #if __OBJC__
  #include <OpenGLES/EAGL.h>
  #endif
 #else
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <OpenGL/glext.h>
 #endif
 #include <objc/runtime.h>
 #include <objc/message.h>
#endif
#ifdef __LP64__
typedef double CGFloat;
#else
typedef float CGFloat;
#endif
#endif
#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

#ifdef PLATFORM_LINUX
// NB: Your CPATH variable should include all the ridiculous number
// of places these headers can be found
//#include	<gdk/gdk.h>
//#include	<gdk/gdkkeysyms.h>
#include	<gtk/gtk.h>
//#include	<gtkgl/gtkglarea.h>
#define GL_GLEXT_PROTOTYPES
#include	<GL/gl.h>
#include	<GL/glext.h>
#include	<GL/glu.h>
#endif

// Configuration
//#define CONFIG_SLOW_ANIMATIONS 1


void oakMain();
long oakCurrentMillis(); // millis
void oakLog(char const* fmt, ...);
void oakRequestRedraw();
string oakGetAppHomeDir();

float dp(float dp); // dp to pixels
float idp(float pixels); // pixels to dp

#ifndef MIN
#define MIN(X,Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X,Y) (((X) > (Y)) ? (X) : (Y))
#endif


void oakKeyboardShow(bool show);
void oakKeyboardNotifyTextChanged();

// Async
void* oakAsyncQueueCreate(const char* queueName);
void oakAsyncQueueDestroy(void* osobj);
void oakAsyncQueueEnqueueItem(void* osobj, std::function<void(void)> func);
void oakAsyncRunOnMainThread(std::function<void(void)> func);

// Camera
#if OAKNUT_WANT_CAMERA
typedef std::function<void (class Bitmap* cameraFrame, float brightness)> CameraPreviewDelegate;
void* oakCameraOpen(int cameraId);
void oakCameraPreviewStart(void* osobj, CameraPreviewDelegate delegate);
void oakCameraPreviewStop(void* osobj);
void oakCameraClose(void* osobj);
#endif

// Audio recording
#if OAKNUT_WANT_AUDIOINPUT
typedef std::function<int (int numSamples, int16_t* samples)> AudioInputDelegate;
const void* oakAudioInputOpen(int sampleRate); // samples are always int16_t, always record in mono.
void oakAudioInputStart(const void* osobj, AudioInputDelegate delegate);
void oakAudioInputStop(const void* osobj);
void oakAudioInputClose(const void* osobj);
#endif

// Face detection
void* oakFaceDetectorCreate();
int oakFaceDetectorDetectFaces(void* osobj, class Bitmap* bitmap);
void oakFaceDetectorClose(void* osobj);

// The order here matters!
#include "graphics/matrix.h"
#include "graphics/geom.h"
#include "text/utf8.h"
#include "base/object.h"
#include "data/data.h"
#include "base/timer.hpp"

Data* oakLoadAsset(const char* assetPath);


#include "graphics/quadbuffer.h"
#include "graphics/glhelp.h"
#include "graphics/region.h"
#include "graphics/bitmap.h"
#include "graphics/textureatlas.h"
#include "app/styles.h"
#include "graphics/font.h"
#include "graphics/renderop.h"
#include "graphics/renderop_fillrect.h"
#include "graphics/renderop_roundrect.h"
#include "graphics/renderop_text.h"
#include "graphics/renderop_texture.h"
#include "graphics/renderbatch.h"
#include "graphics/canvas.h"
#include "graphics/surface.h"
#include "graphics/renderop_blur.h"
#include "graphics/window.h"
#include "view/scrollbar.h"
#include "view/view.h"
#include "graphics/textrenderer.h"
#include "view/label.h"
#include "graphics/anim.h"
#include "view/edittext.h"
#include "text/json.h"
#include "util/cache.h"
#include "app/urlrequest.h"
#include "view/imageview.h"
#include "view/listview.h"
#include "view/button.h"
#include "app/navigationitem.h"
#include "view/navigationbar.h"
#include "app/viewcontroller.h"
#include "view/linearlayout.h"
#include "view/segmentedcontrol.h"
#include "view/searchbox.h"
#include "app/userdefaults.h"
#include "util/circularbuffer.h"
#include "media/audiooutput.h"
#include "view/canvasview.h"

// Canvas
void* oakCanvasCreate();
void oakCanvasResize(void* oscanvas, int width, int height);
Bitmap* oakCanvasGetBitmap(void* oscanvas);
void oakCanvasClear(void* oscanvas, COLOUR colour);
void oakCanvasSetFillColour(void* oscanvas, COLOUR colour);
void oakCanvasSetStrokeColour(void* oscanvas, COLOUR colour);
void oakCanvasSetStrokeWidth(void* oscanvas, float strokeWidth);
void oakCanvasSetAffineTransform(void* oscanvas, AffineTransform* t);
void oakCanvasDrawRect(void* oscanvas, RECT rect);
void oakCanvasDrawOval(void* oscanvas, RECT rect);
void oakCanvasDrawPath(void* oscanvas, void* path);
void oakCanvasRelease(void* oscanvas);

void* oakCanvasPathCreate();
void oakCanvasPathMoveTo(void* ospath, POINT pt);
void oakCanvasPathLineTo(void* ospath, POINT pt);
void oakCanvasPathCurveTo(void* ospath, POINT ctrl1, POINT ctrl2, POINT pt);
void oakCanvasPathRelease(void* ospath);

extern Window* mainWindow;



#endif 
