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

#ifdef PLATFORM_WEB
#include <platform/web/platform.h>
#endif
#ifdef PLATFORM_ANDROID
#include <platform/android/platform.h>
#endif
#ifdef PLATFORM_APPLE
#include <platform/apple/platform.h>
#endif
#ifdef PLATFORM_LINUX
#include <platform/linux/platform.h>
#endif

// These defs need removing
#ifdef __LP64__
typedef double CGFloat;
#else
typedef float CGFloat;
#endif
#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif
#ifndef MIN
#define MIN(X,Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X,Y) (((X) > (Y)) ? (X) : (Y))
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



void oakKeyboardShow(bool show);
void oakKeyboardNotifyTextChanged();


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
#include "base/task.h"

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
#include "graphics/canvas.h"
#include "view/canvasview.h"


extern Window* mainWindow;



#endif 
