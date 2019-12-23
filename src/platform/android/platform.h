#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
typedef float GLfloat;

#define PACKAGE "org/oaknut/main"
#define JAVA_FN(RETURNS, CLAZZ,METHOD) extern "C" JNIEXPORT \
    RETURNS JNICALL Java_org_oaknut_main_ ## CLAZZ ## _ ## METHOD

#include <stdlib.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <android/configuration.h>
#include <android/looper.h>
#include <android/native_activity.h>
#include <android/native_window_jni.h>

#include <jni.h>
#include <sys/time.h>
#include <time.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android/asset_manager_jni.h>
#include <android/bitmap.h>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "oaknut-app", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "oaknut-app", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "oaknut-app", __VA_ARGS__))
#ifndef NDEBUG
#  define LOGV(...)  ((void)__android_log_print(ANDROID_LOG_VERBOSE, "oaknut-app", __VA_ARGS__))
#else
#  define LOGV(...)  ((void)0)
#endif

extern JavaVM* g_jvm;
JNIEnv* getJNIEnv();

// Reroute fopen() to a horrible hack that means it can work with compressed assets
#define fopen(name, mode) oaknut_android_fopen(name, mode)
FILE* oaknut_android_fopen(const char* fname, const char* mode);

// Utilities for handling Java strings without falling foul of JVM's weird ideas about UTF
jbyteArray jbyteArrayFromString(JNIEnv* env, const string& str);
string stringFromJbyteArray(JNIEnv* env, jbyteArray jbytes);
jstring jstringFromString(JNIEnv* env, const string& str);

