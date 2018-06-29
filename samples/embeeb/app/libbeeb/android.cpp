//
// Created by Reuben Scratton on 28/09/15.
//

#if 0
#include "libbeeb.h"
#include "Beeb.h"
#include "Beeb.hpp"
#include <jni.h>
#include <assert.h>
#include <vector>
#include <android/log.h>
#include "Platform.h"

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>


#define LOG_TAG    "libbeeb"

HBEEB g_hbeeb;
int g_textureId;
uint32_t g_bitmapStride;
uint8_t* g_bitmapPixels;

jobject  g_objBeeb;
jobject  g_objBeebView;
static JavaVM *gJavaVM = 0;
JNIEnv* env = 0;
jmethodID midVideoCallback;
jmethodID midTriggerCallback;

// Todo: move to opensl file
static SLObjectItf engineObject;
static SLEngineItf engineEngine;
static SLObjectItf outputMixObject;
static SLObjectItf bqPlayerObject;
static SLPlayItf bqPlayerPlay;
static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
static std::vector<uint8_t*> audioBuffers;

// this callback handler is called every time a buffer finishes playing

void Platform_audioFlush() {
    Beeb* beeb = (Beeb*)g_hbeeb;
    uint32_t cb = beeb->audiobuff->_fill;
    if (cb > 0) {
        uint8_t* audioBuffer = (uint8_t*)malloc(cb);
        audioBuffers.push_back(audioBuffer);
        beeb->audiobuff->read(audioBuffer, cb);
        SLuint32 result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, audioBuffer, cb);
    }
}

void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    //bbcLogI("OpenSL ES callback!");
    uint8_t* audioBuffer = *audioBuffers.begin();
    free(audioBuffer);
    audioBuffers.erase(audioBuffers.begin());
    Platform_audioFlush();
}

void Platform_init(HBEEB hbeeb) {
    g_hbeeb = hbeeb;

    // Create pixel store
#ifdef USE_32BPP
	g_bitmapStride = SURFACE_WIDTH * (32/8);
#else
	g_bitmapStride = SURFACE_WIDTH * (16/8);
#endif
	g_bitmapPixels = (unsigned char*) malloc(g_bitmapStride*SURFACE_HEIGHT);
    Beeb_setVideoBitmapParams(g_hbeeb, g_bitmapPixels, g_bitmapStride);
    
    
    // Create audio
    const SLInterfaceID engineMixIIDs[] = {SL_IID_ENGINE};
    const SLboolean engineMixReqs[] = {SL_BOOLEAN_TRUE};
    SLresult result = slCreateEngine(&engineObject, 0, NULL, 1, engineMixIIDs, engineMixReqs);
    assert(SL_RESULT_SUCCESS == result);
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    assert(SL_RESULT_SUCCESS == result);
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    SLDataLocator_AndroidSimpleBufferQueue dataLocatorInput;
    dataLocatorInput.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
    dataLocatorInput.numBuffers = 4;
    
    SLDataFormat_PCM format;
    format.formatType = SL_DATAFORMAT_PCM;
    format.numChannels = 1;
    format.samplesPerSec = (SLuint32) 32150000; //SL_SAMPLINGRATE_32;
    format.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    format.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
    format.channelMask = SL_SPEAKER_FRONT_CENTER;
    format.endianness = SL_BYTEORDER_LITTLEENDIAN;
    
    SLDataSource dataSource;
    dataSource.pLocator = &dataLocatorInput;
    dataSource.pFormat = &format;

    SLDataLocator_OutputMix dataLocatorOut;
    dataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    dataLocatorOut.outputMix = outputMixObject;
    
    SLDataSink dataSink;
    dataSink.pLocator = &dataLocatorOut;
    dataSink.pFormat = NULL;

    // create audio player
    const SLInterfaceID ids[2] = {SL_IID_PLAY, SL_IID_BUFFERQUEUE};
    const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &dataSource, &dataSink, 2, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE, &bqPlayerBufferQueue);
    assert(SL_RESULT_SUCCESS == result);
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, NULL);
    assert(SL_RESULT_SUCCESS == result);

}


void Platform_pause() {
    SLresult result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_STOPPED);
    assert(SL_RESULT_SUCCESS == result);
}
void Platform_resume() {
    SLresult result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);
}

void Platform_createGLContext() {

    // Get a texture ID
	GLuint textures[] = {1};
	glGenTextures(1, textures);
	g_textureId = textures[0];

	// Set up the texture
	glBindTexture(GL_TEXTURE_2D, g_textureId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	// Assign the bitmap pixels to the texture
	glTexImage2D(GL_TEXTURE_2D,
		0,
#ifdef USE_32BPP
        GL_RGBA,
#else
        GL_RGB,
#endif
		SURFACE_WIDTH,
		SURFACE_HEIGHT,
		0,
#ifdef USE_32BPP
        GL_RGBA,
        GL_UNSIGNED_BYTE,
#else
        GL_RGB,
        GL_UNSIGNED_SHORT_5_6_5,
#endif
		g_bitmapPixels);
}

void Platform_configGLContext() {
}

void Platform_bindTexture() {
	glBindTexture(GL_TEXTURE_2D, g_textureId);
}

void Platform_lockTexture() {
    Platform_bindTexture();
}
void Platform_unlockTexture() {
}

void Platform_Logv(char const *format, va_list args) {
    __android_log_vprint(ANDROID_LOG_INFO, LOG_TAG, format, args);
}


FILE* s_file = NULL;

void Platform_Logf(char const *format, va_list args) {
    char buff[256];
    vsprintf (buff,format, args);

    if (!s_file) {
#ifdef _ARM_
        s_file = fopen("/sdcard/6502_arm.log","w+");
#else
        s_file = fopen("/sdcard/6502_x86.log","w+");
#endif
    }

    if (s_file) {
        fputs(buff, s_file);
        fflush(s_file);
        //fclose(file);
    } else {
        bbcLogI("Oops! Cant log!");
    }
}




jint JNI_OnLoad(JavaVM *jvm, void *reserved) {
    bbcLogI("JNI_OnLoad");
    gJavaVM = jvm;
    int status = gJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4);

    jclass cls = env->FindClass("libbeeb/BeebView");
    midVideoCallback = env->GetMethodID(cls, "videoCallback", "()V");
    cls = env->FindClass("libbeeb/Beeb");
    midTriggerCallback = env->GetMethodID(cls, "triggerCallback", "(Ljava/lang/Object;)V");

    return JNI_VERSION_1_4;

}

extern "C" {



void Platform_videoCallback() {
     env->CallVoidMethod(g_objBeebView, midVideoCallback);
}

JNIEXPORT void JNICALL Java_libbeeb_Beeb_bbcInit(JNIEnv * env, jobject  obj) {
    g_objBeeb = env->NewGlobalRef(obj);
    g_hbeeb = Beeb_init(NULL);
    //bbcSetCallbacks(NULL, videoCallback, LEDsCallback);
}


JNIEXPORT void JNICALL Java_libbeeb_Beeb_bbcResume(JNIEnv * env, jobject  obj) {
    Beeb_resume(g_hbeeb);
}

JNIEXPORT void JNICALL Java_libbeeb_Beeb_bbcTick(JNIEnv * env, jobject  obj) {
    Beeb_onTick(g_hbeeb);
}

JNIEXPORT void JNICALL Java_libbeeb_Beeb_bbcPause(JNIEnv * env, jobject  obj) {
    Beeb_pause(g_hbeeb);
}

JNIEXPORT jint JNICALL Java_libbeeb_BeebView_bbcInitGl(JNIEnv * env, jobject  obj) {
    g_objBeebView = env->NewGlobalRef(obj);
    Beeb_createGLContext(g_hbeeb);
    Beeb_configGLContext(g_hbeeb);
    return 1;
}

JNIEXPORT jint JNICALL Java_libbeeb_BeebView_bbcDrawFrame(JNIEnv * env, jobject obj) {
    Beeb_drawFrame(g_hbeeb);
    return 1;
}



JNIEXPORT void JNICALL Java_libbeeb_Beeb_bbcKeyEvent(JNIEnv * env, jobject  obj, jint vkey, jint down) {
    Beeb_postKeyboardEvent(g_hbeeb, vkey, down);
}

JNIEXPORT void JNICALL Java_libbeeb_Beeb_bbcLoadDisc(JNIEnv * env, jobject  obj, jobject directBuffer, jint do_autoboot) {
    unsigned int size = env->GetDirectBufferCapacity(directBuffer);
    uint8_t* disc = (uint8_t*)env->GetDirectBufferAddress(directBuffer);
    Beeb_bbcLoadDisc(g_hbeeb, disc, size, do_autoboot);
}


/**
Triggers
*/
typedef struct {
    jobject jController;
} TRIGGER;

std::vector<TRIGGER*> g_triggers;

JNIEXPORT void JNICALL Java_libbeeb_Beeb_bbcResetTriggers(JNIEnv* env, jobject  obj) {
    //bbcLogI("bbcResetTriggers");
    for (std::vector<TRIGGER*>::iterator it=g_triggers.begin() ; it != g_triggers.end() ; it++) {
        TRIGGER* trigger = *it;
        env->DeleteGlobalRef(trigger->jController);
        free(trigger);
    }
    g_triggers.clear();
    Beeb_resetTriggers(g_hbeeb);
}


static void TriggerCallback(const void* param1, void* param2) {
    //bbcLogI("Boom! Trigger... ");
    TRIGGER* trigger = (TRIGGER*)param1;
    env->CallVoidMethod(g_objBeeb, midTriggerCallback, trigger->jController);

}

JNIEXPORT void JNICALL Java_libbeeb_Beeb_bbcSetTrigger(JNIEnv* env, jobject  obj, jint pc_addr, jobject jController) {
    //bbcLogI("bbcSetTrigger");
    TRIGGER* trigger = (TRIGGER*)malloc(sizeof(TRIGGER));
    trigger->jController = env->NewGlobalRef(jController);
    g_triggers.push_back(trigger);
    Beeb_addTrigger(g_hbeeb, (uint16_t)pc_addr, TriggerCallback, trigger, 0);
}
}

/*


JNIEXPORT int JNICALL Java_libbeeb_Beeb_bbcSerialize(JNIEnv * env, jobject  obj, jbyteArray abuff) {
    bbcLogI("bbcSerialize");
    uint8_t *buff = (uint8_t*)env->GetByteArrayElements(abuff, 0);
    return bbcSerialize(buff);
}

JNIEXPORT void JNICALL Java_libbeeb_Beeb_bbcDeserialize(JNIEnv * env, jobject  obj, jbyteArray abuff) {
    bbcLogI("bbcDeserialize");
    uint8_t* buff = (uint8_t*)env->GetByteArrayElements(abuff, 0);
    bbcDeserialize(buff);
}


JNIEXPORT jint JNICALL Java_libbeeb_Beeb_bbcGetThumbnail(JNIEnv * env, jobject  obj, jobject jbitmap)
{
    AndroidBitmapInfo info;
    void* pixels;
    int ret;

    if ((ret = AndroidBitmap_getInfo(jbitmap, &info)) < 0) {
        bbcLogE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return -1;
    }

    if ((ret = AndroidBitmap_lockPixels(jbitmap, &pixels)) < 0) {
        bbcLogE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return -1;
    }

    bbcGetThumbnail(pixels, info.stride);

    AndroidBitmap_unlockPixels(jbitmap);
    return 0;
}


JNIEXPORT void JNICALL Java_libbeeb_Beeb_bbcExit(JNIEnv * env, jobject  obj) {
    //moncleanup();
}

*/

#endif
