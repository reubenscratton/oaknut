#include "android.h"
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

android_app* app;

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
            oakLog("Failed to attach to jvm");
            assert(0);
        }
    }
    return env;
}

bool _calledMain = false;

static void engine_draw_frame() {
    if (app->display == NULL) {
        return;
    }

    if (!_calledMain) {
        oakMain();
        _calledMain = true;
    }

    mainWindow->draw();


    eglSwapBuffers(app->display, app->surface);
}


JAVA_FN(void, MainActivity, onCreateNative)(JNIEnv *env, jobject obj,
                                                                         jobject jassetManager, jfloat screenScale) {
    // Create our app object
    app = (struct android_app*)malloc(sizeof(struct android_app));
    memset(app, 0, sizeof(struct android_app));

    app->scale = screenScale;

    /*
    if (savedState != NULL) {
        app->savedState = malloc(savedStateSize);
        app->savedStateSize = savedStateSize;
        memcpy(app->savedState, savedState, savedStateSize);
    }*/

    //app->config = AConfiguration_new();
    //AConfiguration_fromAssetManager(app->config, app->activity->assetManager);
    //print_cur_config(app);


    mainWindow = new Window();

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
        app->animating = 1;
    } else {
        app->animating = 0;
    }
}


JAVA_FN(void, MainActivity, onSurfaceCreatedNative)(JNIEnv* env, jobject clazz, jobject jsurface) {
    app->window = ANativeWindow_fromSurface(env, jsurface);

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

    ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, app->window, NULL);

    const EGLint attrib_list[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    context = eglCreateContext(display, config, NULL, attrib_list);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGI("Unable to eglMakeCurrent");
        return;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    app->display = display;
    app->context = context;
    app->surface = surface;
    app->width = w;
    app->height = h;

    // Initialize GL state.
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    mainWindow->resizeSurface(w, h, app->scale);

    engine_draw_frame();

}

JAVA_FN(void, MainActivity, onSurfaceChangedNative)(JNIEnv* env, jobject obj, jobject jsurface,
                                jint format, jint width, jint height) {

    app->window = ANativeWindow_fromSurface(env, jsurface);
    // TODO: handle size changes. But how likely is that?
}


JAVA_FN(void, MainActivity, redrawNative)(JNIEnv* env, jobject obj) {
    engine_draw_frame();
}

JAVA_FN(void, MainActivity, onSurfaceRedrawNeededNative)(JNIEnv* env, jobject obj, jobject jsurface) {
    engine_draw_frame();
}

JAVA_FN(void, MainActivity, onSurfaceDestroyedNative)(JNIEnv* env, jobject obj, jobject jsurface) {

    if (app->window) {
        ANativeWindow_release(app->window);
        app->window = NULL;
    }

    if (app->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(app->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (app->context != EGL_NO_CONTEXT) {
            eglDestroyContext(app->display, app->context);
        }
        if (app->surface != EGL_NO_SURFACE) {
            eglDestroySurface(app->display, app->surface);
        }
        eglTerminate(app->display);
    }
    app->animating = 0;
    app->display = EGL_NO_DISPLAY;
    app->context = EGL_NO_CONTEXT;
    app->surface = EGL_NO_SURFACE;
}


JAVA_FN(void, MainActivity, onContentRectChangedNative)(JNIEnv* env, jobject obj,
                                    jint x, jint y, jint w, jint h) {
    //mainWindow->resizeClientArea(x,y,w,h);
    check_gl(glClear, GL_COLOR_BUFFER_BIT);
}


JAVA_FN(void, MainActivity, onTouchEventNative)(JNIEnv* env, jobject obj, jint finger, jint action, jlong time, jfloat x, jfloat y) {
    int32_t em_action=0;
    switch (action) {
        case AMOTION_EVENT_ACTION_DOWN: em_action=TOUCH_EVENT_DOWN; break;
        case AMOTION_EVENT_ACTION_MOVE: em_action=TOUCH_EVENT_MOVE; break;
        case AMOTION_EVENT_ACTION_UP: em_action=TOUCH_EVENT_UP; break;
        case AMOTION_EVENT_ACTION_CANCEL: em_action=TOUCH_EVENT_CANCEL; break;
    }
    //LOGI("ev %d %f,%f", em_action, x, y);
    mainWindow->dispatchTouchEvent(em_action, finger, time, x ,y);
}


void oakLog(char const* fmt, ...) {
    char ach[512];
    va_list args;
    va_start(args, fmt);
    vsprintf(ach, fmt, args);
    LOGI("%s", ach);
}


long oakCurrentMillis() {
    timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    long l = t.tv_sec*1000 + (t.tv_nsec / 1000000);
    //emmaLog("time=%ld\n", l);
    return l;
}

string oakGetAppHomeDir() {
    JNIEnv* env = getJNIEnv();
    jstring result = (jstring)env->CallStaticObjectMethod(jclassApp, jmethodIDAppGetDocsPath);
    const char* cstr = env->GetStringUTFChars(result, NULL);
    string str(cstr);
    env->ReleaseStringUTFChars(result, cstr);
    return str;
}

Data* oakLoadAsset(const char* assetPath) {
    JNIEnv* env = getJNIEnv();
    jobject jstr = env->NewStringUTF(assetPath);
    jbyteArray result = (jbyteArray)env->CallStaticObjectMethod(jclassApp, jmethodIDAppLoadAsset, jstr);
    Data* data = NULL;
    if (result != NULL) {
        data = new Data();
        data->cb = env->GetArrayLength(result);
        data->data = (uint8_t*)malloc(data->cb);
        env->GetByteArrayRegion(result, 0, data->cb, reinterpret_cast<jbyte*>(data->data));
    }
    return data;
}



void oakRequestRedraw() {

}

void oakKeyboardShow(bool show) {

}

void oakKeyboardNotifyTextChanged() {

}


