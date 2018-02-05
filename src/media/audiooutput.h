//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

// TODO: this is a mess, fix it

#if EMSCRIPTEN
 #import <Al/al.h>
 #import <Al/alc.h>
#else
 #ifdef ANDROID
  #include <SLES/OpenSLES.h>
  #include <SLES/OpenSLES_Android.h>
 #elif __APPLE__
  #if TARGET_OS_IOS
   #import <OpenAl/al.h>
   #import <OpenAl/alc.h>
   #define USE_NATIVE_IOS_AUDIO
   #import <AudioUnit/AudioUnit.h>
  #else
    #import <OpenAL/OpenAL.h>
  #endif
 #endif
#endif

#if PLATFORM_LINUX
#include <AL/al.h>
#include <AL/alc.h>
#endif


class AudioOutput : public Object {
public:
#ifdef USE_NATIVE_IOS_AUDIO
    AudioComponentInstance toneUnit;
#else
#ifdef OPENAL
    ALCdevice* _device;
    ALCcontext* _context;
    const ALenum _format;
    ALuint _source;
    int _pendingBufferId;
#else
    SLObjectItf _engineObject;
    SLEngineItf _engineEngine;
    SLObjectItf _outputMixObject;
    SLObjectItf _bqPlayerObject;
    SLPlayItf _bqPlayerPlay;
    SLAndroidSimpleBufferQueueItf _bqPlayerBufferQueue;
    std::vector<uint8_t*> _audioBuffers;
#endif
#endif
    CircularBuffer* _buffer;
    
    AudioOutput(int bufferSize);
    void open();
    void write(uint8_t* bytes, uint32_t cb);
    void flush();
    void resume();
    void pause();
};
