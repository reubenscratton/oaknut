// MIT License
//
// Copyright (c) 2018 Sandcastle Software Ltd
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
#include <cstdarg>
#include <cfloat>
#include <cctype>
#else
#include <math.h>
#include <stdarg.h>
#endif
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#ifdef __cplusplus
#include <typeinfo>
#include <cstring>
#include <map>
#include <unordered_map>
#include <iterator>
#include <vector>
#include <list>
#include <stack>
#include <algorithm>
#include <set>
#include <functional>
#include <assert.h>
#include <chrono>

#ifndef MIN
#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#define MAX(a,b) (((a)>(b)) ? (a) : (b))
#endif

using std::map;
using std::unordered_map;
using std::list;
using std::vector;
using std::pair;
using std::make_pair;
using std::set;
using std::stack;
using std::function;

// Platform headers
#ifdef PLATFORM_WEB
#define PLATFORM web
#endif
#ifdef PLATFORM_ANDROID
#define PLATFORM android
#endif
#ifdef PLATFORM_APPLE
#define PLATFORM apple
#endif
#ifdef PLATFORM_LINUX
#define PLATFORM linux
#endif
#define __xincstr(x) #x
#define __incstr(x) __xincstr(x)

// Timestamp type is double in JS and uint64_t everywhere else
#ifdef PLATFORM_WEB
typedef double TIMESTAMP;
#else
typedef uint64_t TIMESTAMP;
#endif
namespace oak {
#include "base/bytearray.h"
#include "base/hash.h"
#include "base/string_.h"
#include "base/object.h"
#include "base/timer.h"
#include "base/task.h"
}

// Include platform headers
using namespace oak;
#include __incstr(platform/PLATFORM/platform.h)

// Oaknut types
namespace oak {
#include "data/stream.h"
#include "data/base64.h"
#include "data/filestream.h"
#include "data/serializable.h"
#include "data/bytebuffer.h"
#include "data/bytebufferstream.h"
#include "text/stringprocessor.h"
#include "text/regex.h"
#include "base/variant.h"
#include "data/localstorage.h"
#include "graphics/bitmap.h"
#include "media/camera.h"
#include "media/audioinput.h"
#include "util/facedetector.h"
#include "graphics/color.h"
#include "graphics/vectors.h"
#include "graphics/matrix.h"
#include "graphics/geom.h"
#include "graphics/quadbuffer.h"
#include "graphics/glhelp.h"
#include "graphics/region.h"
#include "graphics/textureatlas.h"
#include "graphics/font.h"
#include "graphics/renderop.h"
#include "graphics/renderop_rect.h"
#include "graphics/renderop_text.h"
#include "graphics/renderop_texture.h"
#include "graphics/renderbatch.h"
#include "graphics/surface.h"
#include "graphics/renderop_blur.h"
#include "media/videorecorder.h"
#include "media/videoplayer.h"
#include "text/attributedstring.h"
#include "app/styles.h"
#include "graphics/textrenderer.h"
#include "graphics/canvas.h"
#include "app/app.h"
#include "app/urlrequest.h"
#include "util/cache.h"
#include "util/circularbuffer.h"
#include "media/audiooutput.h"
#include "view/inputevent.h"
#include "view/scrollbar.h"
#include "view/alignspec.h"
#include "view/measurespec.h"
#include "view/ikeyboardinputhandler.h"
#include "view/anim.h"
#include "view/view.h"
#include "view/window.h"
#include "app/viewcontroller.h"
#include "app/navigationcontroller.h"
#include "app/actionsheet.h"
#include "widgets/cameraview.h"
#include "widgets/imageview.h"
#include "widgets/label.h"
#include "widgets/button.h"
#include "widgets/edittext.h"
#include "widgets/listview.h"
#include "widgets/simplelistadapter.h"
#include "widgets/navigationbar.h"
#include "widgets/linearlayout.h"
#include "widgets/segmentedcontrol.h"
#include "widgets/searchbox.h"
#include "widgets/canvasview.h"
#include "widgets/checkbox.h"
#include "widgets/pincodeview.h"
#include "widgets/progressspinner.h"
#include "widgets/textfield.h"
}


// Platform-specific types
#include __incstr(platform/PLATFORM/bitmap.h)
#include __incstr(platform/PLATFORM/camera.h)
#include __incstr(platform/PLATFORM/font.h)

namespace std {
template <>
struct hash<oak::string> {
    size_t operator()( const oak::string& k ) const {
        return k.hash();
    }
};
}


#endif

#endif 
