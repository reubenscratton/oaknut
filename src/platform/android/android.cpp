//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if PLATFORM_ANDROID

#include <oaknut.h>

JavaVM* g_jvm;



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


class WindowAndroid : public Window {
public:
    jobject activity;
    AConfiguration* config;
    ANativeWindow* window;
    AAssetManager* assetManager;
    ARect contentRect;
    int activityState;
    int animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;

    WindowAndroid() {
        context = EGL_NO_CONTEXT;
    }
    void draw() override {
        if (display == NULL) {
            return;
        }

        Window::draw();
        eglSwapBuffers(display, surface);
    }

    void destroySurface() override {
        if (window) {
            ANativeWindow_release(window);
            window = NULL;
        }

        if (display != EGL_NO_DISPLAY) {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (context != EGL_NO_CONTEXT) {
                eglDestroyContext(display, context);
                context = EGL_NO_CONTEXT;
            }
            if (surface != EGL_NO_SURFACE) {
                eglDestroySurface(display, surface);
                surface = EGL_NO_SURFACE;
            }
            eglTerminate(display);
            display = EGL_NO_DISPLAY;
        }
        animating = 0;

        Window::destroySurface();
    }

    void keyboardShow(bool show) override {
        JNIEnv* env = getJNIEnv();
        jmethodID jmidShowKeyboard = env->GetMethodID(env->GetObjectClass(activity), "showKeyboard", "(Z)V");
        env->CallVoidMethod(activity, jmidShowKeyboard, show);
    }
    void keyboardNotifyTextChanged() override {
        JNIEnv* env = getJNIEnv();
        jmethodID jmid = env->GetMethodID(env->GetObjectClass(activity), "keyboardNotifyTextChanged", "()V");
        env->CallVoidMethod(activity, jmid);
    }
    void keyboardNotifyTextSelectionChanged() override {
        JNIEnv* env = getJNIEnv();
        jmethodID jmid = env->GetMethodID(env->GetObjectClass(activity), "keyboardNotifyTextSelectionChanged", "()V");
        env->CallVoidMethod(activity, jmid);
    }

};

Window* Window::create() {
    return new WindowAndroid();
}


JAVA_FN(jlong, MainActivity, onCreateNative)(JNIEnv *env, jobject obj,
                                            jobject jassetManager, jfloat screenScale,
                                             jfloat statusBarHeight, jfloat navigationBarHeight,
                                            jlong rootVCcreator) {
    WindowAndroid* window = (WindowAndroid*)Window::create();
    window->retain();
    window->activity = env->NewGlobalRef(obj);
    window->_safeAreaInsets.top = statusBarHeight;
    window->_safeAreaInsets.bottom = navigationBarHeight;
    window->_scale = screenScale;
    window->assetManager = AAssetManager_fromJava(env, jassetManager);

    app.loadStyleAsset("styles.res");

    // If no VC created then this is app startup
    if (!rootVCcreator) {
        app._window = window;
        app.main();
    } else {
        app._window = window; // this sucks. I'd like to lose app._window completely...
        ViewController* rootVC = ((ViewController* (*)(void*))rootVCcreator)(NULL);
        window->setRootViewController((ViewController*)rootVC);
    }
    window->show();
    return (jlong)window;
}

JAVA_FN(void, MainActivity, onStartNative)(JNIEnv *env, jobject obj, jlong nativePtr) {

}

JAVA_FN(void, MainActivity, onResumeNative)(JNIEnv *env, jobject obj, jlong nativePtr) {

}

JAVA_FN(jbyteArray, MainActivity, onSaveInstanceStateNative)(JNIEnv* env, jobject obj, jlong nativePtr) {

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

JAVA_FN(void, MainActivity, onPauseNative)(JNIEnv *env, jobject obj, jlong nativePtr) {

}

JAVA_FN(void, MainActivity, onStopNative)(JNIEnv *env, jobject obj, jlong nativePtr) {

}


JAVA_FN(void, MainActivity, onConfigurationChangedNative)(JNIEnv *env, jobject obj, jlong nativePtr) {
    //AConfiguration_fromAssetManager(android_app->config,
    //                                android_app->assetManager);
    //print_cur_config(android_app);
}

JAVA_FN(void, MainActivity, onWindowFocusChangedNative)(JNIEnv *env, jobject obj, jlong nativePtr, jboolean focused) {
    WindowAndroid* window = (WindowAndroid*)nativePtr;
    if (focused) {
        window->animating = 1;
    } else {
        window->animating = 0;
    }
}


JAVA_FN(void, MainActivity, onSurfaceCreatedNative)(JNIEnv* env, jobject obj, jlong nativePtr, jobject jsurface) {
    WindowAndroid* window = (WindowAndroid*)nativePtr;
    window->window = ANativeWindow_fromSurface(env, jsurface);

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

    ANativeWindow_setBuffersGeometry(window->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, window->window, NULL);

    if (window->context == EGL_NO_CONTEXT) {
        const EGLint attrib_list[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        window->context = eglCreateContext(display, config, NULL, attrib_list);
    }

    if (eglMakeCurrent(display, surface, surface, window->context) == EGL_FALSE) {
        LOGI("Unable to eglMakeCurrent");
        return;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    window->display = display;
    window->surface = surface;
    window->width = w;
    window->height = h;

    // Initialize GL state.
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    window->resizeSurface(w, h, window->_scale);
    window->draw();
}

JAVA_FN(void, MainActivity, onSurfaceChangedNative)(JNIEnv* env, jobject obj, jlong nativePtr, jobject jsurface,
                                jint format, jint width, jint height) {
    WindowAndroid* window = (WindowAndroid*)nativePtr;
    window->window = ANativeWindow_fromSurface(env, jsurface);
    // TODO: handle size changes. But how likely is that?
}


JAVA_FN(void, MainActivity, redrawNative)(JNIEnv* env, jobject obj, jlong nativePtr) {
    WindowAndroid* window = (WindowAndroid*)nativePtr;
    window->draw();
}

JAVA_FN(void, MainActivity, onSurfaceRedrawNeededNative)(JNIEnv* env, jobject obj, jlong nativePtr, jobject jsurface) {
    WindowAndroid* window = (WindowAndroid*)nativePtr;
    window->draw();
}

JAVA_FN(void, MainActivity, onSurfaceDestroyedNative)(JNIEnv* env, jobject obj, jlong nativePtr) {
    WindowAndroid* window = (WindowAndroid*)nativePtr;
    window->destroySurface();

}


JAVA_FN(void, MainActivity, onContentRectChangedNative)(JNIEnv* env, jobject obj, jlong nativePtr,
                                    jint x, jint y, jint w, jint h) {
    WindowAndroid* window = (WindowAndroid*)nativePtr;
    //window->resizeClientArea(x,y,w,h);
    check_gl(glClear, GL_COLOR_BUFFER_BIT);
}


JAVA_FN(void, MainActivity, onTouchEventNative)(JNIEnv* env, jobject obj, jlong nativePtr, jint finger, jint action, jlong time, jfloat x, jfloat y) {
    WindowAndroid* window = (WindowAndroid*)nativePtr;
    INPUTEVENT event;
    event.deviceType = INPUTEVENT::Mouse;
    event.deviceIndex = finger;
    switch (action) {
        case AMOTION_EVENT_ACTION_DOWN: event.type=INPUT_EVENT_DOWN; break;
        case AMOTION_EVENT_ACTION_MOVE: event.type=INPUT_EVENT_MOVE; break;
        case AMOTION_EVENT_ACTION_UP: event.type=INPUT_EVENT_UP; break;
        case AMOTION_EVENT_ACTION_CANCEL: event.type=INPUT_EVENT_CANCEL; break;
    }
    //LOGI("ev %d %f,%f scale=%f", em_action, x, y, window->_scale);
    event.pt.x = x; //* window->_scale;
    event.pt.y = y;// * window->_scale;
    event.time = time;
    window->dispatchInputEvent(event);
}

JAVA_FN(jboolean, MainActivity, textInputIsFocused)(JNIEnv* env, jobject obj, jlong nativePtr) {
    WindowAndroid *window = (WindowAndroid *) nativePtr;
    return window->_textInputReceiver != NULL;
}

// From Android's InputType.java:
static const int TYPE_CLASS_TEXT = 0x00000001;
static const int TYPE_CLASS_NUMBER = 0x00000002;
static const int TYPE_CLASS_PHONE = 0x00000003;
static const int TYPE_TEXT_VARIATION_EMAIL_ADDRESS = 0x00000020;

JAVA_FN(jint, MainActivity, textInputGetInputType)(JNIEnv* env, jobject obj, jlong nativePtr) {
    WindowAndroid *window = (WindowAndroid *) nativePtr;
    if (!window->_textInputReceiver) {
        return 0;
    }
    int keyboardType = window->_textInputReceiver->getSoftKeyboardType();
    if (keyboardType == SoftKeyboardType::Phone) return TYPE_CLASS_PHONE;
    if (keyboardType == SoftKeyboardType::Number) return TYPE_CLASS_NUMBER;
    if (keyboardType == SoftKeyboardType::Email) return TYPE_CLASS_TEXT | TYPE_TEXT_VARIATION_EMAIL_ADDRESS;
    return TYPE_CLASS_TEXT;
}

JAVA_FN(jbyteArray, MainActivity, textInputGetText)(JNIEnv* env, jobject obj, jlong nativePtr) {
    WindowAndroid *window = (WindowAndroid *) nativePtr;
    if (!window->_textInputReceiver) {
        return NULL;
    }
    int len = window->_textInputReceiver->getTextLength();
    string text = window->_textInputReceiver->textInRange(0, len);
    jbyteArray array = NULL;
    if (len > 0) {
        array = env->NewByteArray(text.lengthInBytes());
        if (array != NULL) {
            env->SetByteArrayRegion(array, 0, text.lengthInBytes(), (jbyte*)text.data());
        }
    }
    return array;
}

JAVA_FN(void, MainActivity, textInputSetText)(JNIEnv* env, jobject obj, jlong nativePtr, jbyteArray textBytes, jint newCursorPosition) {
    WindowAndroid *window = (WindowAndroid *) nativePtr;
    if (!window->_textInputReceiver) {
        return;
    }

    int cb = env->GetArrayLength(textBytes);
    bytearray data(cb);
    env->GetByteArrayRegion(textBytes, 0, cb, reinterpret_cast<jbyte*>(data.data()));
    string text((char*)data.data());

    window->_textInputReceiver->insertText(text, window->_textInputReceiver->getSelectionStart(),  window->_textInputReceiver->getInsertionPoint());
    window->_textInputReceiver->setSelectedRange(newCursorPosition, newCursorPosition);
}

JAVA_FN(jint, MainActivity, textInputGetSelStart)(JNIEnv* env, jobject obj, jlong nativePtr) {
    WindowAndroid *window = (WindowAndroid *) nativePtr;
    if (!window->_textInputReceiver) {
        return 0;
    }
    return window->_textInputReceiver->getSelectionStart();
}
JAVA_FN(jint, MainActivity, textInputGetSelEnd)(JNIEnv* env, jobject obj, jlong nativePtr) {
    WindowAndroid *window = (WindowAndroid *) nativePtr;
    if (!window->_textInputReceiver) {
        return 0;
    }
    return window->_textInputReceiver->getInsertionPoint();
}


JAVA_FN(jboolean, MainActivity, onKeyEventNative)(JNIEnv* env, jobject obj, jlong nativePtr, jboolean isDown, jint keyCode, jint charCode) {
    WindowAndroid* window = (WindowAndroid*)nativePtr;
    if (keyCode == 4) { // Don't handle 'back' button, let that go via onBackPressed so Android can handle it properly
        return false;
    }
    KeyboardInputSpecialKeyCode sk = SpecialKeyNone;
    if (keyCode == 21) sk = SpecialKeyCursorLeft;
    else if (keyCode == 22) sk = SpecialKeyCursorRight;
    else if (keyCode == 20) sk = SpecialKeyCursorDown;
    else if (keyCode == 19) sk = SpecialKeyCursorUp;
    else if (keyCode == 59) sk = SpecialKeyShift;
    else if (keyCode == 67) sk = SpecialKeyDelete;
    if (window->_keyboardHandler) {
        window->_keyboardHandler->keyInputEvent(isDown ? KeyDown : KeyUp, sk, keyCode, charCode);
        return true;
    }
    return false;
}

JAVA_FN(jboolean, MainActivity, onBackPressedNative)(JNIEnv* env, jobject obj, jlong nativePtr) {
    WindowAndroid* window = (WindowAndroid*)nativePtr;
    ViewController* vc = window->_rootViewController;
    return vc->navigateBack();
}

JAVA_FN(void, MainActivity, onDestroyNative)(JNIEnv* env, jobject obj, jlong nativePtr) {
    WindowAndroid* window = (WindowAndroid*)nativePtr;
    window->setRootViewController(NULL);
    env->DeleteGlobalRef(window->activity);
    window->activity = NULL;
    window->release();
}


#endif
