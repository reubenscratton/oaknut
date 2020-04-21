//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#include <oaknut.h>

Task* File::load(const string& path, std::function<size_t(variant&)> callback, size_t cb/*=0*/) {
    return Task::enqueue({
        {Task::IO, [=](variant&) -> variant {
            return load_sync(path);
        }},
        {Task::MainThread, [=](variant& r) -> variant {
            callback(r);
            return variant();
        }}
    });
}

#if PLATFORM_ANDROID

static AAssetManager* getAssetManager() {
    static AAssetManager* assetManager = nullptr;
    if (!assetManager) {
        JNIEnv* env = getJNIEnv();
        jclass jclassApp = env->FindClass(PACKAGE "/App");
          jmethodID jmidAppGetAssetManager = env->GetStaticMethodID(jclassApp, "getAssetManager", "()Landroid/content/res/AssetManager;");
          jobject jassetManager = env->CallStaticObjectMethod(jclassApp, jmidAppGetAssetManager);
          assetManager = AAssetManager_fromJava(env, jassetManager);
    }
    return assetManager;
}

static int android_fread(void* cookie, char* buf, int size) {
    return AAsset_read((AAsset*)cookie, buf, size);
}

static int android_fwrite(void* cookie, const char* buf, int size) {
    return EACCES; // can't provide write access to the apk
}

static fpos_t android_fseek(void* cookie, fpos_t offset, int whence) {
    return AAsset_seek((AAsset*)cookie, offset, whence);
}

static int android_fclose(void* cookie) {
    AAsset_close((AAsset*)cookie);
    return 0;
}

#endif

Task* File::exists(const string& path, std::function<void (bool)> callback) {
    return Task::enqueue({
        {Task::IO, [=](variant&) -> variant {
            return exists_sync(path);
        }},
        {Task::MainThread, [=](variant& r) -> variant {
            callback(r.boolVal());
            return variant();
        }}
    });
}

bool File::exists_sync(const string& apath) {
#if PLATFORM_ANDROID
    if (apath.hasPrefix("//assets")) {
        AAsset* asset = AAssetManager_open(getAssetManager(), apath.c_str()+9, 0);
        if (!asset) {
            return false;
        }
        AAsset_close(asset);
        return true;
    }
#endif
    string path = apath;
    resolve(path);
    struct stat buf;
    return 0==stat(path.c_str(), &buf); // TODO: this can fail for many reasons. Consider renaming API.
}

FILE* File::fopen_sync(string& path, const char* mode) {
#if PLATFORM_ANDROID
    if (path.hasPrefix("//assets")) {
        AAsset* asset = AAssetManager_open(getAssetManager(), path.c_str()+9, 0);
        if(!asset) return NULL;
        return funopen(asset, android_fread, android_fwrite, android_fseek, android_fclose);
    }
#endif
    resolve(path);
    return ::fopen(path.c_str(), mode);
}

/*
variant File::load(int fd) {
    assert(!Task::isMainThread());
    struct stat st;
    if (-1 == fstat(fd, &st)) {
        ::close(fd);
        return variant(error::fromErrno());
    }
    bytearray bytes((int32_t)st.st_size);
    ssize_t cbRead = ::read(fd, bytes.data(), st.st_size);
    if (cbRead == -1) {
        ::close(fd);
        return variant(error::fromErrno());
    }
    ::close(fd);
    assert(st.st_size == cbRead);
    return variant(bytes);
}
*/
variant File::load_sync(int fd) {
    struct stat st;
    if (-1 == fstat(fd, &st)) {
        ::close(fd);
        return variant(error::fromErrno());
    }
    bytearray bytes((int32_t)st.st_size);
    ssize_t cbRead = ::read(fd, bytes.data(), st.st_size);
    ::close(fd);
    if (cbRead == -1) {
        return variant(error::fromErrno());
    }
    return variant(bytes);
}

variant File::load_sync(const string& apath) {
#if PLATFORM_ANDROID
    if (apath.hasPrefix("//assets")) {
        AAsset* asset = AAssetManager_open(getAssetManager(), apath.c_str()+9, 0);
        if (asset) {
            off_t cb = AAsset_getLength(asset);
            if (cb > 0) {
                bytearray buf(cb);
                AAsset_read(asset, buf.data(), cb);
                AAsset_close(asset);
                return buf;
            }
            AAsset_close(asset);
        }
        return error(EACCES);
    }
#endif

    string path = apath;
    resolve(path);
    int fd = ::open(path.c_str(), O_RDONLY, 0);
    if (fd == -1) {
        return variant(error::fromErrno());
    }
    return load_sync(fd);
}


vector<string> File::dir_sync(string& path) {
    vector<string> files;
#if PLATFORM_ANDROID
    if (path.hasPrefix("//assets")) {
        AAssetDir* dir = AAssetManager_openDir(getAssetManager(), path.c_str()+9);
        while(const char * fileName = AAssetDir_getNextFileName(dir)) {
            files.push_back(string(fileName,-1));
        }
        AAssetDir_close(dir);
        return files;
    }
#endif

    resolve(path);
    struct dirent *pDirent;
    DIR *pDir = opendir(path.c_str());
    if (!pDir) {
        printf("Cannot open directory '%s'\n", path.c_str());
    } else {
        while ((pDirent = readdir(pDir)) != NULL) {
            if (pDirent->d_name[0] == '.') {
                if (pDirent->d_name[1] == '\0') {
                    continue;
                }
                if (pDirent->d_name[1] == '.' && pDirent->d_name[2] == '\0') {
                    continue;
                }
            }
            files.push_back(string(pDirent->d_name, strlen(pDirent->d_name)/*pDirent->d_namlen*/));
        }
        closedir(pDir);
    }
    return files;
}

/*variant File::load(const string& path) {
    assert(!Task::isMainThread());
    int fd = ::open(path.c_str(), O_RDONLY, 0);
    if (-1 == fd) {
        return variant(error::fromErrno());
    }
    return load(fd);
}*/

static void mkdir_rec(char* path) {
    char* p = path;
    while ((p = strchr(p+1, '/'))) {
        *p = 0;
        mkdir(path, S_IRWXU);
        *p = '/';
    }
}
error File::save_sync(const string& path, const bytearray& data) {
    assert(!Task::isMainThread());
    int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR);
    if (-1 == fd && 2==errno) {
        mkdir_rec((char*)path.c_str());
        fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR);
    }
    if (-1 == fd) {
        return error::fromErrno();
    }
    ssize_t cbWritten = ::write(fd, data.data(), data.size());
    ::close(fd);
    if (cbWritten != data.size()) {
        return error::fromErrno();
    }
    return error::none();
}

