//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>
#include "bitmap.h"


class WindowWeb : public Window {
public:

    void requestRedrawNative() override {
        EM_ASM({ requestAnimationFrame( function() { _dispatchMainWindowDraw(); }) });
    }

    void keyboardShow(bool show) override {
        
    }
    void keyboardNotifyTextChanged() override {
        
    }

};

Window* Window::create() {
    return new WindowWeb();
}

static val getTimestamp = val::global("getTimestamp");

TIMESTAMP App::currentMillis() {
    return getTimestamp().as<TIMESTAMP>();
}

EMSCRIPTEN_KEEPALIVE
extern "C" void dispatchMainWindowDraw() {
    app._window->draw();
}



class OSTimer : public Timer {
public:
    OSTimer(const std::function<void()>& del, int intervalMillis, bool repeats) : Timer(del, intervalMillis, repeats) {
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


Timer* Timer::start(const std::function<void()>& del, int intervalMillis, bool repeats) {
    return new OSTimer(del, intervalMillis, repeats);
}




static void oak_setWindowSize(int width, int height, int scale) {
    app.log("window size %d x %d, scale=%d", width, height, scale);
    app._window->resizeSurface(width, height, scale);
}

static void oak_userEvent(int eventType, int eventSourceId, int x, int y) {
    //app.log("userEv type=%d src=%d x=%d,y=%d", eventType, eventSourceId, x, y);
    
    INPUTEVENT inputEvent;
    inputEvent.deviceType = INPUTEVENT::Mouse;
    inputEvent.deviceIndex = eventSourceId;
    inputEvent.type = eventType;
    inputEvent.pt.x = x * app._window->_scale;
    inputEvent.pt.y = y * app._window->_scale;
    inputEvent.time = app.currentMillis();
    app._window->dispatchInputEvent(inputEvent);
}

static map<string,KeyboardInputSpecialKeyCode> s_specialKeys = {
    {"Alt", SpecialKeyAlt},
    {"ArrowLeft", SpecialKeyCursorLeft},
    {"ArrowRight", SpecialKeyCursorRight},
    {"ArrowUp", SpecialKeyCursorUp},
    {"ArrowDown", SpecialKeyCursorDown},
    {"Backspace", SpecialKeyDelete},
    {"Delete", SpecialKeyDelete},
    {"AltGraph", SpecialKeyAlt},
    {"CapsLock", SpecialKeyCapsLock},
    {"Control", SpecialKeyControl},
    {"Escape", SpecialKeyEscape},
    {"Fn", SpecialKeyFunction},
    {"Meta", SpecialKeyCommand},
    {"Shift", SpecialKeyShift}
};

static void oak_keyEvent(int keyDown, int keyCode, int keynameBuffPtr) {
    if (app._window->_keyboardHandler) {
        string keyname((char*)keynameBuffPtr);
        //app.log("key: %s", keyname.data());
        char32_t charCode = 0;
        KeyboardInputSpecialKeyCode specialKey = SpecialKeyNone;
        if (keyname.length()==1) {
            charCode = keyname.charAt(0);
        } else {
            const auto& it = s_specialKeys.find(keyname);
            if (it != s_specialKeys.end()) {
                specialKey = it->second;
            } else {
                if (keyname=="Tab") charCode='\t';
                if (keyname=="Enter") charCode='\n';
            }
        }
        app._window->_keyboardHandler->keyInputEvent(keyDown? KeyDown:KeyUp, specialKey, keyCode, charCode);
    }
}

void oakMessageLoop() {
    //    app._window->draw();
}

static void oak_main() {
    app.loadStyleAsset("styles.res");
    app.main();
    app._window->show();
}

EMSCRIPTEN_BINDINGS(oaknut) {
    emscripten::function("oak_setWindowSize", &oak_setWindowSize);
    emscripten::function("oak_userEvent", &oak_userEvent);
    emscripten::function<void,int,int,int>("oak_keyEvent", &oak_keyEvent);
    emscripten::function("oak_main", &oak_main);
}


int main(int argc, char *argv[]) {
    
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
    
    
    app._window = Window::create();

    // Run the main loop (which does nothing)
    emscripten_set_main_loop(oakMessageLoop, 1, 1);
    
    return 0;
}




#endif
