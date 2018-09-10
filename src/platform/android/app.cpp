//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if PLATFORM_ANDROID

#include <oaknut.h>


static jclass jclassApp;
static jmethodID jmidAppLoadAsset;
static jmethodID jmidAppGetPath;
static jmethodID jmidAppGetPrefsInt;
static jmethodID jmidAppSetPrefsInt;
static jmethodID jmidAppGetPrefsString;
static jmethodID jmidAppSetPrefsString;
static jmethodID jmidAppCreateUUID;
static jmethodID jmidAppShowKeyboard;

static JNIEnv* getAppEnv() {
  JNIEnv* env = getJNIEnv();
  if (!jclassApp) {
    jclassApp = env->FindClass(PACKAGE "/App");
    jclassApp = (jclass)env->NewGlobalRef(jclassApp);
    jmidAppLoadAsset = env->GetStaticMethodID(jclassApp, "loadAsset", "(Ljava/lang/String;)[B");
    jmidAppGetPath = env->GetStaticMethodID(jclassApp, "getPath", "(I)[B");
    jmidAppGetPrefsInt = env->GetStaticMethodID(jclassApp, "getPrefsInt", "([BI)I");
    jmidAppSetPrefsInt = env->GetStaticMethodID(jclassApp, "setPrefsInt", "([BI)V");
    jmidAppGetPrefsString = env->GetStaticMethodID(jclassApp, "getPrefsString", "([B[B)[B");
    jmidAppSetPrefsString = env->GetStaticMethodID(jclassApp, "setPrefsString", "([B[B)V");
    jmidAppCreateUUID = env->GetStaticMethodID(jclassApp, "createUUID", "()Ljava/lang/String;");
      jmidAppShowKeyboard = env->GetStaticMethodID(jclassApp, "showKeyboard", "(Z)V");
  }
  return env;
}

static string getPath(int i) {
  JNIEnv* env = getAppEnv();
  jbyteArray jbytes = (jbyteArray)env->CallStaticObjectMethod(jclassApp, jmidAppGetPath, i);
  int cb = env->GetArrayLength(jbytes);
  bytearray data(cb);
  env->GetByteArrayRegion(jbytes, 0, cb, reinterpret_cast<jbyte*>(data.data()));
  string path((char*)data.data());
  return path;
}

string App::getPathForGeneralFiles() {
  return getPath(1);
}
string App::getPathForUserDocuments() {
  return getPath(1);
}
string App::getPathForCacheFiles() {
  return getPath(1);
}

void App::log(char const* fmt, ...) {
    char ach[512];
    va_list args;
    va_start(args, fmt);
    vsprintf(ach, fmt, args);
    LOGI("%s", ach);
}
void App::warn(char const* fmt, ...) {
    char ach[512];
    va_list args;
    va_start(args, fmt);
    vsprintf(ach, fmt, args);
    LOGW("%s", ach);
}


TIMESTAMP App::currentMillis() {
    timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    TIMESTAMP l = t.tv_sec*1000 + (t.tv_nsec / 1000000);
    return l;
}


ByteBuffer* App::loadAsset(const char* assetPath) {
    JNIEnv* env = getAppEnv();
    jobject jstr = env->NewStringUTF(assetPath);
    jbyteArray result = (jbyteArray)env->CallStaticObjectMethod(jclassApp, jmidAppLoadAsset, jstr);
    ByteBuffer* data = NULL;
    if (result != NULL) {
        data = new ByteBuffer();
        data->cb = env->GetArrayLength(result);
        data->data = (uint8_t*)malloc(data->cb);
        env->GetByteArrayRegion(result, 0, data->cb, reinterpret_cast<jbyte*>(data->data));
    }
    return data;
}

string string::uuid() {
    JNIEnv* env = getAppEnv();
    jstring jstr = (jstring)env->CallStaticObjectMethod(jclassApp, jmidAppCreateUUID);
    jboolean isCopy = false;
    auto sz = env->GetStringUTFChars(jstr, &isCopy);
    return string(sz);

}


void App::requestRedraw() {

}

void App::keyboardShow(bool show) {
    JNIEnv* env = getAppEnv();
    env->CallStaticVoidMethod(jclassApp, jmidAppShowKeyboard, show);
}

void App::keyboardNotifyTextChanged() {

}

/*EDGEINSETS App::getWindowSafeAreaInsets() {
    return {0,40,0,100}; // TODO: determine properly
}*/

class jstringHelper {
public:
  jstringHelper(const char* utf8) {
    if (!utf8) {
      _jba = NULL;
    } else {
      JNIEnv* env = getAppEnv();
      int cb = strlen(utf8);
      _jba = env->NewByteArray(cb);
      env->SetByteArrayRegion(_jba, 0, cb, (jbyte*)utf8);
    }
  }
  jstringHelper(jbyteArray jba) {
    _jba = jba ? (jbyteArray)getAppEnv()->NewLocalRef(jba) : NULL;
  }

  ~jstringHelper() {
    if (_jba) {
      getAppEnv()->DeleteLocalRef(_jba);
      _jba = NULL;
    }
  }
  operator jbyteArray() const {
    return _jba;
  }
  string toString() const {
    if (_jba) {
      JNIEnv* env = getAppEnv();
      int cb = env->GetArrayLength(_jba);
      if (cb > 0) {
          bytearray ba(cb);
          env->GetByteArrayRegion(_jba, 0, cb, (jbyte *) ba.data());
          return string((char *) ba.data());
      }
    }
    return "";
  }
private:
  jbyteArray _jba;
};

int App::getIntSetting(const char *key, const int defaultValue) {
  return getAppEnv()->CallStaticIntMethod(jclassApp, jmidAppGetPrefsInt,
    (jbyteArray)jstringHelper(key), defaultValue);
}
void App::setIntSetting(const char* key, const int value) {
  getAppEnv()->CallStaticVoidMethod(jclassApp, jmidAppSetPrefsInt,
    (jbyteArray)jstringHelper(key), value);
}
string App::getStringSetting(const char *key, const char* defaultValue) {
  jstringHelper ba = (jbyteArray)getAppEnv()->CallStaticObjectMethod(jclassApp, jmidAppGetPrefsString,
    (jbyteArray)jstringHelper(key), (jbyteArray)jstringHelper(defaultValue));
  return ba.toString();
}
void App::setStringSetting(const char* key, const char* value) {
  getAppEnv()->CallStaticVoidMethod(jclassApp, jmidAppSetPrefsString,
    (jbyteArray)jstringHelper(key), (jbyteArray)jstringHelper(value));
}

#endif
