//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if PLATFORM_ANDROID

#include <oaknut.h>

JavaVM* g_jvm;
jclass s_jclassURLRequest;

static ALooper* mainThreadLooper;
static int messagePipe[2];
static os_sem mainThreadCallbacksSem;
static std::list<std::function<void()>> mainThreadCallbacks;

static int mainThreadLooperCallback(int fd, int events, void* data) {
    uint8_t msg;
    read(fd, &msg, 1); // read message from pipe
    assert(msg == 1);
    os_sem_wait(mainThreadCallbacksSem, -1);
    assert(mainThreadCallbacks.size() > 0);
    auto callback = *mainThreadCallbacks.begin();
    mainThreadCallbacks.pop_front();
    os_sem_signal(mainThreadCallbacksSem);
    callback();
    return 1; // continue listening for events
}


extern "C" jint JNI_OnLoad(JavaVM *jvm, void *reserved) {
    g_jvm = jvm;

    auto env = getJNIEnv();
    s_jclassURLRequest = env->FindClass(PACKAGE "/URLRequest");
    s_jclassURLRequest = (jclass) env->NewGlobalRef(s_jclassURLRequest);

    os_sem_init(&mainThreadCallbacksSem, 1);
    mainThreadLooper = ALooper_forThread();
    ALooper_acquire(mainThreadLooper);
    pipe(messagePipe); //create send-receive pipe
    // listen for pipe read end, if there is something to read
    // - notify via provided callback on main thread
    ALooper_addFd(mainThreadLooper, messagePipe[0],
                  0, ALOOPER_EVENT_INPUT, mainThreadLooperCallback, nullptr);

    return JNI_VERSION_1_6;
}


void Task::postToMainThread(std::function<void()> callback, int delay) {
    assert(delay==0); // TODO: implement delayed callbacks
    os_sem_wait(mainThreadCallbacksSem, -1);
    mainThreadCallbacks.push_back(callback);
    os_sem_signal(mainThreadCallbacksSem);
    uint8_t msg = 1;
    write(messagePipe[1], &msg, 1);
}


JNIEnv* getJNIEnv() {
    JNIEnv* env = NULL;
    int getEnvStat = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
    if (getEnvStat == JNI_EDETACHED) {
        if (g_jvm->AttachCurrentThread(&env, NULL) != 0) {
            log("Failed to attach to jvm");
            assert(0);
        }
    }
    return env;
}

jbyteArray jbyteArrayFromString(JNIEnv* env, const string& str) {
    jbyteArray array = NULL;
    int cb = str.lengthInBytes();
    if (cb > 0) {
        array = env->NewByteArray(cb);
        if (array != NULL) {
            env->SetByteArrayRegion(array, 0, cb, (jbyte*)str.c_str());
        }
    }
    return array;
}
string stringFromJbyteArray(JNIEnv* env, jbyteArray jbytes) {
    int cb = env->GetArrayLength(jbytes);
    bytearray data(cb);
    env->GetByteArrayRegion(jbytes, 0, cb, reinterpret_cast<jbyte*>(data.data()));
    return data.toString();
}


jstring jstringFromString(JNIEnv* env, const string& str) {
    int cb = str.lengthInBytes();
    if (cb > 0) {
        return env->NewStringUTF(str.c_str());
    }
    return NULL;
}

class GLSurfaceAndroid : public GLSurface {
public:
    GLSurfaceAndroid(Renderer* renderer, bool isPrivate) : GLSurface(renderer, isPrivate) {
    }

    void bindToNativeWindow(long nativeWindowHandle) override {
        //assert(0); // todo: move the gl setup here
    }

};

class WindowAndroid : public Window {
public:
    jobject activity;
    AConfiguration* config;
    ANativeWindow* window;
    ARect contentRect;
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

    /**
     * Permissions
     */
    string androidPermissionName(Permission permission) {
        switch (permission) {
            case PermissionCamera:
                return "android.permission.CAMERA";
            case PermissionMic:
                return "android.permission.RECORD_AUDIO";
        }
        assert(0); // unknown permission
        return "";
    }
    struct AsyncPermissionRequest {
        std::function<void(vector<bool>)> callback;
    };
    bool hasPermission(Permission permission) override {
        JNIEnv* env = getJNIEnv();
        string permissionName = androidPermissionName(permission);
        jmethodID jmid = env->GetMethodID(env->GetObjectClass(activity), "hasPermission", "([B)Z");
        return env->CallBooleanMethod(activity, jmid, jbyteArrayFromString(env, permissionName));
    }

    void runWithPermissions(vector<Permission> permissions, std::function<void(vector<bool>)> callback) override {
        JNIEnv* env = getJNIEnv();
        jobjectArray names = env->NewObjectArray(permissions.size(), env->FindClass("java/lang/String"), NULL);
        int i=0;
        for (Permission permission : permissions) {
            string permissionName = androidPermissionName(permission);
            jstring permissionNameStr = jstringFromString(env, permissionName);
            env->SetObjectArrayElement(names, i++, permissionNameStr);
        }

        // Async
        AsyncPermissionRequest* req = new AsyncPermissionRequest();
        req->callback = callback;
        jmethodID  jmid = env->GetMethodID(env->GetObjectClass(activity), "requestPermission", "(J[Ljava/lang/String;)V");
        env->CallVoidMethod(activity, jmid, (jlong)req, names);
    }
    void onGotPermissionsResults(jlong nativeRequestPtr, jbooleanArray results) {
        JNIEnv* env = getJNIEnv();
        AsyncPermissionRequest* req = (AsyncPermissionRequest*)nativeRequestPtr;
        int c = env->GetArrayLength(results);
        jboolean* jbools = new jboolean[c];
        env->GetBooleanArrayRegion(results, 0, c, jbools);
        vector<bool> vec(c);
        for (int i=0 ; i<c ; i++) {
            vec[i] = jbools[i];
        }
        delete [] jbools;
        req->callback(vec);
        delete req;
    }

};

Window* Window::create() {
    return new WindowAndroid();
}


class AndroidRenderer : public GLRenderer {
public:
    AndroidRenderer() : GLRenderer() {
    }
    Surface* createSurface(bool isPrivate) override {
        return new GLSurfaceAndroid(this, isPrivate);
    }

    /*void bindToNativeWindow(long nativeWindowHandle) override {
        // todo: move EAGL setup code here
    }
    void commit() override {
        // todo: move Linux swapBuffer stuff here
    }*/

};

Renderer* Renderer::create() {
    return new AndroidRenderer();
}

JAVA_FN(jlong, MainActivity, onCreateNative)(JNIEnv *env, jobject obj,
                                            jobject jassetManager,
                                             jint screenWidth,
                                             jint screenHeight,
                                             jfloat xdpi,
                                             jfloat ydpi,
                                             jfloat statusBarHeight,
                                             jfloat navigationBarHeight,
                                            jlong rootVCcreator) {

    app->_defaultDisplay = new Display(screenWidth,
                                       screenHeight,
                                       xdpi, ydpi,
                                       (xdpi/163.0));

    WindowAndroid* window = (WindowAndroid*)Window::create();
    window->retain();
    window->activity = env->NewGlobalRef(obj);
    window->setSafeInsets(EDGEINSETS(0, statusBarHeight, 0, navigationBarHeight));

    app->loadStyleAssetSync("styles.res");

    // If no VC created then this is app startup
    app->_window = window; // this sucks. I'd like to lose app->_window completely...
    if (!rootVCcreator) {
    } else {
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

    if (window == app->_window) {
        app->main();
    }

    // Initialize GL state.
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    window->resizeSurface(w, h);
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
        case AMOTION_EVENT_ACTION_CANCEL: event.type=INPUT_EVENT_TAP_CANCEL; break;
    }
    // LOGI("ev %d %f,%f", event.type, x, y);
    event.pt.x = x;
    event.pt.y = y;
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

static const int IME_ACTION_UNSPECIFIED = 0x00000000;
static const int IME_ACTION_NONE = 0x00000001;
static const int IME_ACTION_GO = 0x00000002;
static const int IME_ACTION_SEARCH = 0x00000003;
static const int IME_ACTION_SEND = 0x00000004;
static const int IME_ACTION_NEXT = 0x00000005;
static const int IME_ACTION_DONE = 0x00000006;

JAVA_FN(jint, MainActivity, textInputGetInputType)(JNIEnv* env, jobject obj, jlong nativePtr) {
    WindowAndroid *window = (WindowAndroid *) nativePtr;
    if (!window->_textInputReceiver) {
        return 0;
    }
    int keyboardType = window->_textInputReceiver->getSoftKeyboardType();
    if (keyboardType == SoftKeyboardType::KeyboardPhone) return TYPE_CLASS_PHONE;
    if (keyboardType == SoftKeyboardType::KeyboardNumber) return TYPE_CLASS_NUMBER;
    if (keyboardType == SoftKeyboardType::KeyboardEmail) return TYPE_CLASS_TEXT | TYPE_TEXT_VARIATION_EMAIL_ADDRESS;
    return TYPE_CLASS_TEXT;
}

JAVA_FN(jint, MainActivity, textInputGetPreferredActionType)(JNIEnv* env, jobject obj, jlong nativePtr) {
    WindowAndroid *window = (WindowAndroid *) nativePtr;
    if (window->_textInputReceiver) {
        switch (window->_textInputReceiver->getActionType()) {
            case ActionNone:
                return IME_ACTION_NONE;
            case ActionNext:
                return IME_ACTION_NEXT;
            case ActionSearch:
                return IME_ACTION_SEARCH;
            case ActionDone:
                return IME_ACTION_DONE;
            case ActionGo:
                return IME_ACTION_GO;
        }
    }
    return IME_ACTION_UNSPECIFIED;
}


JAVA_FN(jbyteArray, MainActivity, textInputGetText)(JNIEnv* env, jobject obj, jlong nativePtr) {
    WindowAndroid *window = (WindowAndroid *) nativePtr;
    if (!window->_textInputReceiver) {
        return NULL;
    }
    int len = window->_textInputReceiver->getTextLength();
    string text = window->_textInputReceiver->textInRange(0, len);
    jbyteArray array = jbyteArrayFromString(env, text);
    return array;
}

JAVA_FN(void, MainActivity, textInputSetText)(JNIEnv* env, jobject obj, jlong nativePtr, jbyteArray textBytes, jint newCursorPosition) {
    WindowAndroid *window = (WindowAndroid *) nativePtr;
    if (!window->_textInputReceiver) {
        return;
    }

    // Get the inserted text to a string
    int cb = env->GetArrayLength(textBytes);
    bytearray data(cb);
    env->GetByteArrayRegion(textBytes, 0, cb, reinterpret_cast<jbyte*>(data.data()));
    string text = data.toString();

    // Perform the insert/replace
    int32_t s = window->_textInputReceiver->getSelectionStart();
    int32_t e = window->_textInputReceiver->getInsertionPoint();
    window->_textInputReceiver->insertText(text, s, e);

    // newCursorPosition: One of Android's dumber API parameters, it's key to note this position is
    // relative to the text PARAMETER given to this function, not the text in the EditText. Almost
    // a whole day lost to this misunderstanding. >:-[
    if (newCursorPosition > 0) { // relative to end of inserted text - 1
        newCursorPosition = MIN(s,e) + text.length() + newCursorPosition - 1;
    } else { // relative to the start of the inserted text.
        newCursorPosition =  MIN(s,e) - newCursorPosition;
    }

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
JAVA_FN(void, MainActivity, textInputActionPressed)(JNIEnv* env, jobject obj, jlong nativePtr) {
    WindowAndroid *window = (WindowAndroid *) nativePtr;
    if (!window->_textInputReceiver) {
        return;
    }
    return window->_textInputReceiver->handleActionPressed();
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

JAVA_FN(void, MainActivity, onGotPermissionsResults)(JNIEnv* env, jobject obj, jlong nativePtr, jlong nativeReqPtr, jbooleanArray results) {
    WindowAndroid *window = (WindowAndroid *) nativePtr;
    window->onGotPermissionsResults(nativeReqPtr, results);
}

JAVA_FN(jlong, MainActivity, getMainEGLContext)(JNIEnv* env, jobject obj, jlong nativePtr) {
    WindowAndroid *window = (WindowAndroid *) nativePtr;
    return (jlong)window->context;
}

#endif
