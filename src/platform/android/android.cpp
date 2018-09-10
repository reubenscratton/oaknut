//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if PLATFORM_ANDROID

#include <oaknut.h>

struct android_app {
    AConfiguration* config;
    ANativeWindow* window;
    ARect contentRect;
    int activityState;
    int animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    float scale;
    float statusBarHeight;
    float navigationBarHeight;
};

JavaVM* g_jvm;
static android_app* g_app;
static AAssetManager* g_assetManager;
static bool g_calledMain = false;


extern "C" jint JNI_OnLoad(JavaVM *jvm, void *reserved) {
    g_jvm = jvm;
    return JNI_VERSION_1_6;
}

JNIEnv* getJNIEnv() {
    JNIEnv* env = NULL;
    int getEnvStat = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
    if (getEnvStat == JNI_EDETACHED) {
        if (g_jvm->AttachCurrentThread(&env, NULL) != 0) {
            app.log("Failed to attach to jvm");
            assert(0);
        }
    }
    return env;
}


static void engine_draw_rect() {
    if (g_app->display == NULL) {
        return;
    }

    if (!g_calledMain) {
        app._window->_safeAreaInsets.top = g_app->statusBarHeight;
        app._window->_safeAreaInsets.bottom = g_app->navigationBarHeight;
        app.main();
        g_calledMain = true;
    }
    app._window->draw();


    eglSwapBuffers(g_app->display, g_app->surface);
}


JAVA_FN(void, MainActivity, onCreateNative)(JNIEnv *env, jobject obj,
                                            jobject jassetManager, jfloat screenScale, jfloat statusBarHeight, jfloat navigationBarHeight) {
    // Create our app object
    g_app = (struct android_app*)malloc(sizeof(struct android_app));
    memset(g_app, 0, sizeof(struct android_app));
    g_app->scale = screenScale;
    g_app->statusBarHeight = statusBarHeight;
    g_app->navigationBarHeight = navigationBarHeight;
    g_app->context = EGL_NO_CONTEXT;

    app._window = new Window();
    g_assetManager = AAssetManager_fromJava(env, jassetManager);
}

JAVA_FN(void, MainActivity, onStartNative)(JNIEnv *env, jobject obj) {

}

JAVA_FN(void, MainActivity, onResumeNative)(JNIEnv *env, jobject obj) {

}

JAVA_FN(jbyteArray, MainActivity, onSaveInstanceStateNative)(JNIEnv* env, jobject clazz) {

    // TODO: implement
    jbyte* state = NULL;
    uint32_t len = 0;

    // Convert to java byte array
    jbyteArray array = NULL;
    if (len > 0) {
        array = env->NewByteArray(len);
        if (array != NULL) {
            env->SetByteArrayRegion(array, 0, len, state);
        }
    }
    if (state != NULL) {
        free(state);
    }
    return array;
}

JAVA_FN(void, MainActivity, onPauseNative)(JNIEnv *env, jobject obj) {

}

JAVA_FN(void, MainActivity, onStopNative)(JNIEnv *env, jobject obj) {

}


JAVA_FN(void, MainActivity, onConfigurationChangedNative)(JNIEnv *env, jobject obj) {
    //AConfiguration_fromAssetManager(android_app->config,
    //                                android_app->assetManager);
    //print_cur_config(android_app);
}

JAVA_FN(void, MainActivity, onWindowFocusChangedNative)(JNIEnv *env, jobject obj, jboolean focused) {
    if (focused) {
        g_app->animating = 1;
    } else {
        g_app->animating = 0;
    }
}


JAVA_FN(void, MainActivity, onSurfaceCreatedNative)(JNIEnv* env, jobject clazz, jobject jsurface) {
    g_app->window = ANativeWindow_fromSurface(env, jsurface);

    // initialize OpenGL ES and EGL
    const EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLint w, h, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(g_app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, g_app->window, NULL);

    if (g_app->context == EGL_NO_CONTEXT) {
        const EGLint attrib_list[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        g_app->context = eglCreateContext(display, config, NULL, attrib_list);
    }

    if (eglMakeCurrent(display, surface, surface, g_app->context) == EGL_FALSE) {
        LOGI("Unable to eglMakeCurrent");
        return;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    g_app->display = display;
    g_app->surface = surface;
    g_app->width = w;
    g_app->height = h;

    // Initialize GL state.
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    app._window->resizeSurface(w, h, g_app->scale);

    engine_draw_rect();

}

JAVA_FN(void, MainActivity, onSurfaceChangedNative)(JNIEnv* env, jobject obj, jobject jsurface,
                                jint format, jint width, jint height) {

    g_app->window = ANativeWindow_fromSurface(env, jsurface);
    // TODO: handle size changes. But how likely is that?
}


JAVA_FN(void, MainActivity, redrawNative)(JNIEnv* env, jobject obj) {
    engine_draw_rect();
}

JAVA_FN(void, MainActivity, onSurfaceRedrawNeededNative)(JNIEnv* env, jobject obj, jobject jsurface) {
    engine_draw_rect();
}

JAVA_FN(void, MainActivity, onSurfaceDestroyedNative)(JNIEnv* env, jobject obj, jobject jsurface) {

    app._window->destroySurface();

    if (g_app->window) {
        ANativeWindow_release(g_app->window);
        g_app->window = NULL;
    }

    if (g_app->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(g_app->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (g_app->context != EGL_NO_CONTEXT) {
            eglDestroyContext(g_app->display, g_app->context);
            g_app->context = EGL_NO_CONTEXT;
        }
        if (g_app->surface != EGL_NO_SURFACE) {
            eglDestroySurface(g_app->display, g_app->surface);
            g_app->surface = EGL_NO_SURFACE;
        }
        eglTerminate(g_app->display);
        g_app->display = EGL_NO_DISPLAY;
    }
    g_app->animating = 0;
}


JAVA_FN(void, MainActivity, onContentRectChangedNative)(JNIEnv* env, jobject obj,
                                    jint x, jint y, jint w, jint h) {
    //app._window->resizeClientArea(x,y,w,h);
    check_gl(glClear, GL_COLOR_BUFFER_BIT);
}


JAVA_FN(void, MainActivity, onTouchEventNative)(JNIEnv* env, jobject obj, jint finger, jint action, jlong time, jfloat x, jfloat y) {
    INPUTEVENT event;
    event.deviceType = INPUTEVENT::Mouse;
    event.deviceIndex = finger;
    switch (action) {
        case AMOTION_EVENT_ACTION_DOWN: event.type=INPUT_EVENT_DOWN; break;
        case AMOTION_EVENT_ACTION_MOVE: event.type=INPUT_EVENT_MOVE; break;
        case AMOTION_EVENT_ACTION_UP: event.type=INPUT_EVENT_UP; break;
        case AMOTION_EVENT_ACTION_CANCEL: event.type=INPUT_EVENT_CANCEL; break;
    }
    //LOGI("ev %d %f,%f scale=%f", em_action, x, y, app._window->_scale);
    event.pt.x = x; //* app._window->_scale;
    event.pt.y = y;// * app._window->_scale;
    event.time = time;
    app._window->dispatchInputEvent(event);
}

JAVA_FN(jboolean, MainActivity, onKeyEventNative)(JNIEnv* env, jobject obj, jboolean isDown, jint keyCode, jint charCode) {
    if (keyCode == 4) { // Don't handle 'back' button, let that go via onBackPressed so Android can handle it properly
        return false;
    }
    KeyboardInputSpecialKeyCode sk = SpecialKeyNone;
    if (keyCode == 21) sk = SpecialKeyCursorLeft;
    else if (keyCode == 22) sk = SpecialKeyCursorRight;
    else if (keyCode == 20) sk = SpecialKeyCursorDown;
    else if (keyCode == 19) sk = SpecialKeyCursorUp;
    if (keyCode == 67) sk = SpecialKeyDelete;
    if (app._window->_keyboardHandler) {
        app._window->_keyboardHandler->keyInputEvent(isDown ? KeyDown : KeyUp, sk, keyCode, charCode);
        return true;
    }
    return false;
}

JAVA_FN(jboolean, MainActivity, onBackPressedNative)(JNIEnv* env, jobject obj) {
    Window* window = app._window;
    ViewController* vc = window->_rootViewController;
    return vc->navigateBack();
}

JAVA_FN(void, MainActivity, onDestroyNative)(JNIEnv* env, jobject obj) {
    app._window->setRootViewController(NULL);
    g_calledMain = false;
}

#endif
