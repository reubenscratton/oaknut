//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#ifdef PLATFORM_ANDROID

#include <oaknut.h>

jbyteArray jbyteArrayFromString(JNIEnv* env, const string& str);
jstring jstringFromString(JNIEnv* env, const string& str);


class VideoPlayerAndroid : public VideoPlayer {
public:

    jobject _jplayer;

    void open(const string& assetPath) override {
        assert(_status == Unready);
        JNIEnv *env = getJNIEnv();
        jclass playerClass = env->FindClass(PACKAGE "/VideoPlayer");
        playerClass = (jclass) env->NewGlobalRef(playerClass);
        jmethodID jmidConstructor = env->GetMethodID(playerClass, "<init>", "(J[B)V");
        _jplayer = env->NewObject(playerClass, jmidConstructor, (jlong)this, jbyteArrayFromString(env, assetPath));
        _jplayer = env->NewGlobalRef(_jplayer);

        // open()
        jmethodID jmidOpen = env->GetMethodID(playerClass, "open", "()V");
        env->CallVoidMethod(_jplayer, jmidOpen);
    }

    void play() override {
        if (_status == Unready || _status == Playing) {
            return;
        }
        setStatus(Playing);
        JNIEnv *env = getJNIEnv();
        env->CallVoidMethod(_jplayer, env->GetMethodID(env->GetObjectClass(_jplayer), "play", "()V"));
    }


    void onFinished() {
        pause();
        setStatus(Finished);
    }

    void setCurrent(int current) override {
        JNIEnv *env = getJNIEnv();
        jmethodID  midSeek = env->GetMethodID(env->GetObjectClass(_jplayer), "seek", "(I)V");
        env->CallVoidMethod(_jplayer, midSeek, current);
    }


    void pause() override {
        if (_status != Playing) {
            return;
        }
        JNIEnv *env = getJNIEnv();
        env->CallVoidMethod(_jplayer, env->GetMethodID(env->GetObjectClass(_jplayer), "pause", "()V"));
        setStatus(Paused);
    }

    void close() override {
        JNIEnv *env = getJNIEnv();
        env->CallVoidMethod(_jplayer, env->GetMethodID(env->GetObjectClass(_jplayer), "close", "()V"));
        env->DeleteGlobalRef(_jplayer);
        _jplayer = NULL;
        setStatus(Unready);
    }

    void setReady() {
        setStatus(Ready);
    }

    TextureConverter _converter;

    void dispatchNewFrame(int textureId, int width, int height, float* transform) {

        // <sigh>. The texture passed to this function is an external texture that we could
        // trivially render, if only it was possible to hang on to the raw data beyond
        // the duration of the onFrameAvailable callback. Unfortunately it isn't possible,
        // this is a limitation of the SurfaceTexture API. As soon as onFrameAvailable returns
        // the buffer gets recycled, so we *have* to use it now. Here we convert the
        // external texture into a proper OpenGL texture that's the right way up.

        GLuint convertedTex = _converter.convert(textureId, width, height, transform);
        ObjPtr<Bitmap> frame = new Bitmap(convertedTex);
        frame->_width = width;
        frame->_height = height;
        //frame->_texTarget = GL_TEXTURE_EXTERNAL_OES;
        onNewFrameReady(frame);
    }

    Bitmap* _frameBitmap;
};

VideoPlayer* VideoPlayer::create() {
    return new VideoPlayerAndroid();
}


JAVA_FN(void, VideoPlayer, nativeSetReady)(JNIEnv *env, jobject obj, long nativeObj) {
    VideoPlayerAndroid *player = (VideoPlayerAndroid *) nativeObj;
    player->setReady();
}

JAVA_FN(void, VideoPlayer, nativeGotFrame)(JNIEnv *env, jobject obj, long nativeObj, jint textureID, jint width, jint height, jfloatArray jtransform) {
    VideoPlayerAndroid *player = (VideoPlayerAndroid *) nativeObj;
    float transform[16];
    env->GetFloatArrayRegion(jtransform, 0, 16, transform);
    player->dispatchNewFrame(textureID, width, height, transform);
}

JAVA_FN(void, VideoPlayer, nativeOnFinished)(JNIEnv *env, jobject obj, long nativeObj) {
    VideoPlayerAndroid *player = (VideoPlayerAndroid *) nativeObj;
    player->onFinished();
}



#endif
