//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>
#include "bitmap.h"



long App::currentMillis() {
    static val jsfn_getTimestamp = val::global("getTimestamp");
    return jsfn_getTimestamp().as<int>();
    //return (clock() * 1000) / CLOCKS_PER_SEC;
}

EMSCRIPTEN_KEEPALIVE
extern "C" void dispatchMainWindowDraw() {
    app._window->draw();
}

void App::requestRedraw() {
    EM_ASM({ requestAnimationFrame( function() { _dispatchMainWindowDraw(); }) });
}


class OSTimer : public Timer {
public:
    OSTimer(const TimerDelegate& del, int intervalMillis, bool repeats) : Timer(del, intervalMillis, repeats) {
        _timerId = EM_ASM_INT ({
            return setInterval(function() { Runtime.dynCall('vi', $2, [$0]); }, $1);
        }, this, intervalMillis, dispatch);
    }
    
    void stop() {
        if (_timerId) {
            EM_ASM_({
                clearInterval($0);
            }, _timerId);
            _timerId = 0;
        }
        Timer::stop();
    }
    
    static void dispatch(OSTimer* timer) {
        if (!timer->_repeats) {
            timer->stop();
        }
        timer->_del();
    }

protected:
    int _timerId;
};


Timer* Timer::start(const TimerDelegate& del, int intervalMillis, bool repeats) {
    return new OSTimer(del, intervalMillis, repeats);
}




static void Emma_setWindowSize(int width, int height, int scale) {
    app.log("window size %d x %d, scale=%d", width, height, scale);
    app._window->resizeSurface(width, height, scale);
    //glutReshapeWindow(width, height);
}

static void Emma_userEvent(int eventType, int eventSourceId, int x, int y) {
    //app.log("userEv type=%d src=%d x=%d,y=%d", eventType, eventSourceId, x, y);
    x *= app._window->_scale;
    y *= app._window->_scale;
    app._window->dispatchInputEvent(eventType, (INPUT_SOURCE_TYPE_MOUSE<<8) | eventSourceId, app.currentMillis(), x, y);
}

void oakMessageLoop() {
    //    app._window->draw();
}

static void appMain() {
    app.main();
}

EMSCRIPTEN_BINDINGS(libbeeb) {
    emscripten::function("Emma_setWindowSize", &Emma_setWindowSize);
    emscripten::function("Emma_userEvent", &Emma_userEvent);
    emscripten::function("Emma_main", &appMain);
}


int main(int argc, char *argv[]) {
    app.log("main()\n");
    
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = 0;
    attr.depth = 0;
    attr.stencil = 0;
    attr.antialias = 0;
    attr.preferLowPowerToHighPerformance = 0;
    attr.failIfMajorPerformanceCaveat = 0;
    attr.preserveDrawingBuffer = 0;
    attr.enableExtensionsByDefault = 1;
    attr.premultipliedAlpha = 0;
    attr.majorVersion = 1;
    attr.minorVersion = 0;
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context(0, &attr);
    EM_ASM(
           gl = Module.canvas.getContext('webgl');
           );
    emscripten_webgl_make_context_current(ctx);
    
    
    app._window = new Window();
    
    // Run the main loop (which does nothing)
    emscripten_set_main_loop(oakMessageLoop, 1, 1);
    
    return 0;
}

static void onDataXLoad(OnDataLoadCompleteDelegate* delegate, void* val) {
    app.log("onDataLoad! %X", val);
}


Data* App::loadAsset(const char* assetPath) {
    
    string str = "/assets/";
    str.append(assetPath);
    FILE* asset = fopen(str.data(), "rb");
    if (!asset) {
        app.log("Failed to open asset: %s", assetPath);
        return NULL;
    }
    
    Data* data = new Data();
    fseek (asset, 0, SEEK_END);
    data->cb = ftell(asset);
    data->data = (uint8_t*) malloc (sizeof(char)*data->cb);
    fseek ((FILE*)asset, 0, SEEK_SET);
    size_t read = fread(data->data, 1, data->cb, (FILE*)asset);
    assert(read == data->cb);
    fclose(asset);
    return data;
    
}

/*
void oakLoadAppFile(const char* path, OnDataLoadCompleteDelegate delegate) {
    EM_ASM_({
        var onDataXLoad=$0;
        var delegate=$1;
        var val = localStorage.getItem(Pointer_stringify($2));
        //alert('The item is ' + val);
        Runtime.dynCall('vii', onDataXLoad, [delegate, val]);
    }, onDataXLoad, &delegate, path);
}

void oakSaveAppFile(const char* path, Data* data) {
    EM_ASM_({
        var subarray = HEAP8.subarray($1, $1+$2);
        subarray = bytesToHex(subarray);
        localStorage.setItem(Pointer_stringify($0), subarray);
    }, path, data, cb);    
}*/


string App::getAppHomeDir() {
    return ".";
}

void App::keyboardShow(bool show) {
    
}
void App::keyboardNotifyTextChanged() {
    
}

#endif
