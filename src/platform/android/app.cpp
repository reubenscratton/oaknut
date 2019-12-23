//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if PLATFORM_ANDROID

#include <oaknut.h>
#include <dirent.h>

static AAssetManager* assetManager;

vector<string> App::fileList(oak::string& path) const {
    vector<string> results;
    if (path.hasPrefix("//assets/")) {
        AAssetDir* dir = AAssetManager_openDir(assetManager, path.c_str()+9);
        while(const char * fileName = AAssetDir_getNextFileName(dir)) {
            results.push_back(string(fileName,-1));
        }
        AAssetDir_close(dir);
    } else {
        struct dirent *pDirent;
        DIR *pDir = opendir(path.c_str());
        if (!pDir) {
            printf("Cannot open directory '%s'\n", path.c_str());
        } else {
            while ((pDirent = readdir(pDir)) != NULL) {
                results.push_back(pDirent->d_name);
            }
            closedir(pDir);
        }
    }
    return results;
}

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
    jmethodID jmidAppGetAssetManager = env->GetStaticMethodID(jclassApp, "getAssetManager", "()Landroid/content/res/AssetManager;");
    jobject jassetManager = env->CallStaticObjectMethod(jclassApp, jmidAppGetAssetManager);
    assetManager = AAssetManager_fromJava(env, jassetManager);
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

static int oaknut_android_fread(void* cookie, char* buf, int size) {
    return AAsset_read((AAsset*)cookie, buf, size);
}

static int oaknut_android_fwrite(void* cookie, const char* buf, int size) {
    return EACCES; // can't provide write access to the apk
}

static fpos_t oaknut_android_fseek(void* cookie, fpos_t offset, int whence) {
    return AAsset_seek((AAsset*)cookie, offset, whence);
}

static int oaknut_android_fclose(void* cookie) {
    AAsset_close((AAsset*)cookie);
    return 0;
}
#undef fopen

FILE* oaknut_android_fopen(const char* fname, const char* mode) {
    if (0!=strncmp(fname, "//assets", 8)) {
        return fopen(fname, mode);
    }
    AAsset* asset = AAssetManager_open(assetManager, fname+9, 0);
    if(!asset) return NULL;
    return funopen(asset, oaknut_android_fread, oaknut_android_fwrite, oaknut_android_fseek, oaknut_android_fclose);
}


bool App::fileResolve(string& path) const {
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

/*
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
}*/

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
