//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if PLATFORM_ANDROID

#include <oaknut.h>
#include <dirent.h>


static jclass jclassApp;
static jmethodID jmidAppResolvePath;
static jmethodID jmidAppGetPrefsInt;
static jmethodID jmidAppSetPrefsInt;
static jmethodID jmidAppGetPrefsString;
static jmethodID jmidAppSetPrefsString;
static jmethodID jmidAppCreateUUID;
static jmethodID jmidAppGetCurrentCountryCode;


static JNIEnv* getAppEnv() {
  JNIEnv* env = getJNIEnv();
  if (!jclassApp) {
    jclassApp = env->FindClass(PACKAGE "/App");
    jclassApp = (jclass)env->NewGlobalRef(jclassApp);
    jmidAppResolvePath = env->GetStaticMethodID(jclassApp, "resolvePath", "([B)[B");
    jmidAppGetPrefsInt = env->GetStaticMethodID(jclassApp, "getPrefsInt", "([BI)I");
    jmidAppSetPrefsInt = env->GetStaticMethodID(jclassApp, "setPrefsInt", "([BI)V");
    jmidAppGetPrefsString = env->GetStaticMethodID(jclassApp, "getPrefsString", "([B[B)[B");
    jmidAppSetPrefsString = env->GetStaticMethodID(jclassApp, "setPrefsString", "([B[B)V");
    jmidAppCreateUUID = env->GetStaticMethodID(jclassApp, "createUUID", "()Ljava/lang/String;");
    jmidAppGetCurrentCountryCode = env->GetStaticMethodID(jclassApp, "getCurrentCountryCode", "()[B");
  }
  return env;
}

class jstringHelper {
public:
    jstringHelper(const string& utf8) {
        int cb = utf8.lengthInBytes();
        if (!cb) {
            _jba = NULL;
        } else {
            JNIEnv* env = getAppEnv();
            _jba = env->NewByteArray(cb);
            env->SetByteArrayRegion(_jba, 0, cb, (jbyte*)utf8.c_str());
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
                return string((char *) ba.data(), cb);
            }
        }
        return "";
    }
private:
    jbyteArray _jba;
};

bool File::resolve(string& path) {
    if (!path.hasPrefix("//"_S)) {
        return true;
    }
    JNIEnv* env = getAppEnv();
    jstringHelper jstr(path);
    jbyteArray jbytes = (jbyteArray)env->CallStaticObjectMethod(jclassApp, jmidAppResolvePath, (jbyteArray)jstr);
    path = stringFromJbyteArray(env, jbytes);
    return true;
}



string App::currentCountryCode() const {
    JNIEnv* env = getAppEnv();
    jbyteArray jbytes = (jbyteArray)env->CallStaticObjectMethod(jclassApp, jmidAppGetCurrentCountryCode);
    return stringFromJbyteArray(env, jbytes);
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


string string::uuid() {
    JNIEnv* env = getAppEnv();
    jstring jstr = (jstring)env->CallStaticObjectMethod(jclassApp, jmidAppCreateUUID);
    jboolean isCopy = false;
    auto sz = env->GetStringUTFChars(jstr, &isCopy);
    return string(sz, -1);
}



bool App::getBoolSetting(const string& key, const bool defaultValue) {
    return getIntSetting(key, defaultValue?1:0);
}
void App::setBoolSetting(const string& key, const bool value) {
    setIntSetting(key, value?1:0);
}

int App::getIntSetting(const string& key, const int defaultValue) {
  return getAppEnv()->CallStaticIntMethod(jclassApp, jmidAppGetPrefsInt,
    (jbyteArray)jstringHelper(key), defaultValue);
}
void App::setIntSetting(const string& key, const int value) {
  getAppEnv()->CallStaticVoidMethod(jclassApp, jmidAppSetPrefsInt,
    (jbyteArray)jstringHelper(key), value);
}
string App::getStringSetting(const string& key, const string& defaultValue) {
  jstringHelper ba = (jbyteArray)getAppEnv()->CallStaticObjectMethod(jclassApp, jmidAppGetPrefsString,
    (jbyteArray)jstringHelper(key), (jbyteArray)jstringHelper(defaultValue));
  return ba.toString();
}
void App::setStringSetting(const string& key, const string& value) {
  getAppEnv()->CallStaticVoidMethod(jclassApp, jmidAppSetPrefsString,
    (jbyteArray)jstringHelper(key), (jbyteArray)jstringHelper(value));
}

#endif
