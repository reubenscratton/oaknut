//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

// TODO: this is a mess, fix it

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


