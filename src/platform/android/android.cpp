//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if PLATFORM_ANDROID

#include "bitmap.h"

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
};

android_app* aapp;

AAssetManager *assetManager;
JavaVM* g_jvm;
jclass jclassApp;
jmethodID jmethodIDAppLoadAsset;
jmethodID jmethodIDAppGetDocsPath;


extern "C" jint JNI_OnLoad(JavaVM *jvm, void *reserved) {
    LOGI("JNI_OnLoad");
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

bool _calledMain = false;

static void engine_draw_frame() {
    if (aapp->display == NULL) {
        return;
    }

    if (!_calledMain) {
        app.main();
        _calledMain = true;
    }
    app._window->draw();


    eglSwapBuffers(aapp->display, aapp->surface);
}


JAVA_FN(void, MainActivity, onCreateNative)(JNIEnv *env, jobject obj,
                                            jobject jassetManager, jfloat screenScale) {
    // Create our app object
    aapp = (struct android_app*)malloc(sizeof(struct android_app));
    memset(aapp, 0, sizeof(struct android_app));

    aapp->scale = screenScale;

    /*
    if (savedState != NULL) {
        app->savedState = malloc(savedStateSize);
        app->savedStateSize = savedStateSize;
        memcpy(app->savedState, savedState, savedStateSize);
    }*/

    //aapp->config = AConfiguration_new();
    //AConfiguration_fromAssetManager(aapp->config, aapp->activity->assetManager);
    //print_cur_config(app);


    app._window = new Window();

    assetManager = AAssetManager_fromJava(env, jassetManager);
    jclassApp = env->FindClass(PACKAGE "/App");
    jclassApp = (jclass)env->NewGlobalRef(jclassApp);
    jmethodIDAppLoadAsset = env->GetStaticMethodID(jclassApp, "loadAsset", "(Ljava/lang/String;)[B");
    jmethodIDAppGetDocsPath = env->GetStaticMethodID(jclassApp, "getDocsPath", "()Ljava/lang/String;");
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
        aapp->animating = 1;
    } else {
        aapp->animating = 0;
    }
}


JAVA_FN(void, MainActivity, onSurfaceCreatedNative)(JNIEnv* env, jobject clazz, jobject jsurface) {
    aapp->window = ANativeWindow_fromSurface(env, jsurface);

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
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(aapp->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, aapp->window, NULL);

    const EGLint attrib_list[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    context = eglCreateContext(display, config, NULL, attrib_list);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGI("Unable to eglMakeCurrent");
        return;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    aapp->display = display;
    aapp->context = context;
    aapp->surface = surface;
    aapp->width = w;
    aapp->height = h;

    // Initialize GL state.
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    app._window->resizeSurface(w, h, aapp->scale);

    engine_draw_frame();

}

JAVA_FN(void, MainActivity, onSurfaceChangedNative)(JNIEnv* env, jobject obj, jobject jsurface,
                                jint format, jint width, jint height) {

    aapp->window = ANativeWindow_fromSurface(env, jsurface);
    // TODO: handle size changes. But how likely is that?
}


JAVA_FN(void, MainActivity, redrawNative)(JNIEnv* env, jobject obj) {
    engine_draw_frame();
}

JAVA_FN(void, MainActivity, onSurfaceRedrawNeededNative)(JNIEnv* env, jobject obj, jobject jsurface) {
    engine_draw_frame();
}

JAVA_FN(void, MainActivity, onSurfaceDestroyedNative)(JNIEnv* env, jobject obj, jobject jsurface) {

    if (aapp->window) {
        ANativeWindow_release(aapp->window);
        aapp->window = NULL;
    }

    if (aapp->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(aapp->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (aapp->context != EGL_NO_CONTEXT) {
            eglDestroyContext(aapp->display, aapp->context);
        }
        if (aapp->surface != EGL_NO_SURFACE) {
            eglDestroySurface(aapp->display, aapp->surface);
        }
        eglTerminate(aapp->display);
    }
    aapp->animating = 0;
    aapp->display = EGL_NO_DISPLAY;
    aapp->context = EGL_NO_CONTEXT;
    aapp->surface = EGL_NO_SURFACE;
}


JAVA_FN(void, MainActivity, onContentRectChangedNative)(JNIEnv* env, jobject obj,
                                    jint x, jint y, jint w, jint h) {
    //app._window->resizeClientArea(x,y,w,h);
    check_gl(glClear, GL_COLOR_BUFFER_BIT);
}


JAVA_FN(void, MainActivity, onTouchEventNative)(JNIEnv* env, jobject obj, jint finger, jint action, jlong time, jfloat x, jfloat y) {
    int32_t em_action=0;
    switch (action) {
        case AMOTION_EVENT_ACTION_DOWN: em_action=INPUT_EVENT_DOWN; break;
        case AMOTION_EVENT_ACTION_MOVE: em_action=INPUT_EVENT_MOVE; break;
        case AMOTION_EVENT_ACTION_UP: em_action=INPUT_EVENT_UP; break;
        case AMOTION_EVENT_ACTION_CANCEL: em_action=INPUT_EVENT_CANCEL; break;
    }
    //LOGI("ev %d %f,%f scale=%f", em_action, x, y, app._window->_scale);
    x *= app._window->_scale;
    y *= app._window->_scale;
    app._window->dispatchInputEvent(em_action, MAKE_SOURCE(INPUT_SOURCE_TYPE_FINGER,finger), time, x ,y);
}


void App::log(char const* fmt, ...) {
    char ach[512];
    va_list args;
    va_start(args, fmt);
    vsprintf(ach, fmt, args);
    LOGI("%s", ach);
}


long App::currentMillis() {
    timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    long l = t.tv_sec*1000 + (t.tv_nsec / 1000000);
    //emmaLog("time=%ld\n", l);
    return l;
}


ByteBuffer* App::loadAsset(const char* assetPath) {
    JNIEnv* env = getJNIEnv();
    jobject jstr = env->NewStringUTF(assetPath);
    jbyteArray result = (jbyteArray)env->CallStaticObjectMethod(jclassApp, jmethodIDAppLoadAsset, jstr);
    ByteBuffer* data = NULL;
    if (result != NULL) {
        data = new ByteBuffer();
        data->cb = env->GetArrayLength(result);
        data->data = (uint8_t*)malloc(data->cb);
        env->GetByteArrayRegion(result, 0, data->cb, reinterpret_cast<jbyte*>(data->data));
    }
    return data;
}



void App::requestRedraw() {

}

void App::keyboardShow(bool show) {

}

void App::keyboardNotifyTextChanged() {

}

#endif
