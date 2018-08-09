//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


#define SAMPLE_TYPE int16_t
#define SAMPLE_RATE 31250
//#define SAMPLE_RATE 24000


AudioOutput::AudioOutput(int bufferSize)
#ifndef USE_NATIVE_IOS_AUDIO
#ifdef OPENAL
        : _format(AL_FORMAT_MONO16)
#endif
#endif
{
    _buffer = new CircularBuffer(bufferSize);
}




#ifdef USE_NATIVE_IOS_AUDIO
OSStatus AudioInputProc(void *inRefCon,
                        AudioUnitRenderActionFlags *ioActionFlags,
                        const AudioTimeStamp *inTimeStamp,
                        UInt32 inBusNumber,
                        UInt32 inNumberFrames,
                        AudioBufferList *ioData) {
    
    AudioOutput* audioOutput = (AudioOutput*)inRefCon;
    int8_t* buffer = (int8_t*)ioData->mBuffers[0].mData;
    uint32_t cb = inNumberFrames * sizeof(int16_t);
    uint32_t cbThis = audioOutput->_buffer->read((uint8_t*)buffer, cb);
    cb -= cbThis;
    if (cb > 0) {
        memset(buffer+cbThis, 0, cb);
    }
    return noErr;
}
#endif



#ifndef OPENAL
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    AudioOutput* audioOutput = (AudioOutput*)context;
    uint8_t* audioBuffer = *audioOutput->_audioBuffers.begin();
    free(audioBuffer);
    audioOutput->_audioBuffers.erase(audioOutput->_audioBuffers.begin());
    audioOutput->flush();
}
#endif

void AudioOutput::open() {
#if TARGET_OS_IOS

    // Get the default playback output unit
    AudioComponentDescription defaultOutputDescription;
    defaultOutputDescription.componentType = kAudioUnitType_Output;
    defaultOutputDescription.componentSubType = kAudioUnitSubType_RemoteIO;
    defaultOutputDescription.componentManufacturer = kAudioUnitManufacturer_Apple;
    defaultOutputDescription.componentFlags = 0;
    defaultOutputDescription.componentFlagsMask = 0;
    AudioComponent defaultOutput = AudioComponentFindNext(NULL, &defaultOutputDescription);
    assert(defaultOutput); // "Can't find default output";
    OSErr err = AudioComponentInstanceNew(defaultOutput, &toneUnit);
    assert(toneUnit);  // "Error creating unit: %hd", err
    
    // Set our tone rendering function on the unit
    AURenderCallbackStruct input;
    input.inputProc = AudioInputProc;
    input.inputProcRefCon = this;
    err = AudioUnitSetProperty(toneUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &input,sizeof(input));
    assert(err == noErr); // "Error setting callback: %hd", err
    
    // Setup a mono audio stream for sound
    AudioStreamBasicDescription format;
    format.mSampleRate       = SAMPLE_RATE;
    format.mFormatID         = kAudioFormatLinearPCM;
    format.mFormatFlags      = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    format.mBitsPerChannel   = 8 * sizeof(SAMPLE_TYPE);
    format.mChannelsPerFrame = 1;
    format.mBytesPerFrame    = sizeof(SAMPLE_TYPE);
    format.mFramesPerPacket  = 1;
    format.mBytesPerPacket   = format.mBytesPerFrame * format.mFramesPerPacket;
    format.mReserved         = 0;
    err = AudioUnitSetProperty (toneUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &format,sizeof(format));
    assert(err == noErr); //  Error setting stream format: %hd", err
    err = AudioUnitInitialize(toneUnit);
    assert(err == noErr); // Error initializing unit: %hd", err

#else
#ifdef OPENAL
    _device = alcOpenDevice(NULL);
    _context = alcCreateContext(_device, NULL);
    alcMakeContextCurrent(_context);
    alGenSources(1, &_source);
#else
    const SLInterfaceID engineMixIIDs[] = {SL_IID_ENGINE};
    const SLboolean engineMixReqs[] = {SL_BOOLEAN_TRUE};
    SLresult result = slCreateEngine(&_engineObject, 0, NULL, 1, engineMixIIDs, engineMixReqs);
    result = (*_engineObject)->Realize(_engineObject, SL_BOOLEAN_FALSE);
    result = (*_engineObject)->GetInterface(_engineObject, SL_IID_ENGINE, &_engineEngine);
    result = (*_engineEngine)->CreateOutputMix(_engineEngine, &_outputMixObject, 0, NULL, NULL);
    result = (*_outputMixObject)->Realize(_outputMixObject, SL_BOOLEAN_FALSE);

    SLDataLocator_AndroidSimpleBufferQueue dataLocatorInput;
    dataLocatorInput.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
    dataLocatorInput.numBuffers = 4;

    SLDataFormat_PCM format;
    format.formatType = SL_DATAFORMAT_PCM;
    format.numChannels = 1;
    format.samplesPerSec = (SLuint32) SAMPLE_RATE * 1000; //SL_SAMPLINGRATE_32;
    format.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    format.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
    format.channelMask = SL_SPEAKER_FRONT_CENTER;
    format.endianness = SL_BYTEORDER_LITTLEENDIAN;

    SLDataSource dataSource;
    dataSource.pLocator = &dataLocatorInput;
    dataSource.pFormat = &format;

    SLDataLocator_OutputMix dataLocatorOut;
    dataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    dataLocatorOut.outputMix = _outputMixObject;

    SLDataSink dataSink;
    dataSink.pLocator = &dataLocatorOut;
    dataSink.pFormat = NULL;
    app.log("Got to here 3");

    // create audio player
    const SLInterfaceID ids[2] = {SL_IID_PLAY, SL_IID_BUFFERQUEUE};
    const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*_engineEngine)->CreateAudioPlayer(_engineEngine, &_bqPlayerObject, &dataSource, &dataSink, 2, ids, req);
    if (result == 0) {
    result = (*_bqPlayerObject)->Realize(_bqPlayerObject, SL_BOOLEAN_FALSE);
    result = (*_bqPlayerObject)->GetInterface(_bqPlayerObject, SL_IID_PLAY, &_bqPlayerPlay);
    result = (*_bqPlayerObject)->GetInterface(_bqPlayerObject, SL_IID_BUFFERQUEUE, &_bqPlayerBufferQueue);
    result = (*_bqPlayerBufferQueue)->RegisterCallback(_bqPlayerBufferQueue, bqPlayerCallback, this);
    }

#endif
#endif
}

void AudioOutput::write(uint8_t* bytes, uint32_t cb) {
    _buffer->write(bytes, cb);
}

void AudioOutput::flush() {

#if TARGET_OS_IOS
    // no-op
#else
#ifdef OPENAL
    ALuint bufferId = 0;
    
    // Free any completed buffers
    ALint buffersProcessed = 0;
    alGetSourcei(_source, AL_BUFFERS_PROCESSED, &buffersProcessed);
    while (buffersProcessed > 0) {
        alSourceUnqueueBuffers(_source, 1, &bufferId);
        buffersProcessed--;
    }
    
    // cb=6250 i.e. 3125 16-bit samples, i.e. 1/10th of a second of audio
    
    // Exit if there's nothing to do
    uint32_t cb = _buffer->_fill;
    if (!cb) {
        return;
    }

    // Get the audio data from the circular buffer into a flat buffer
    uint8_t* audioBuffer = (uint8_t*)malloc(cb);
    _buffer->read(audioBuffer, cb);
    
    // Get a buffer ID
    if (!bufferId) {
        alGenBuffers(1, &bufferId);
    }
    
    // Buffer the audio data
    alBufferData(bufferId, _format, audioBuffer, cb, SAMPLE_RATE);
    free(audioBuffer);
    
    // If there's nothing queued then save the buffer for *next* time (so we can q 2 buffers)
    if (!_pendingBufferId) {
        ALint buffersQueued = 0;
        alGetSourcei(_source, AL_BUFFERS_QUEUED, &buffersQueued);
        if (!buffersQueued) {
            _pendingBufferId = bufferId;
            return;
        }
    }
    
    // Submit buffer(s) for playback
    if (_pendingBufferId) {
        ALuint ids[2];
        ids[0] = _pendingBufferId;
        ids[1] = bufferId;
        alSourceQueueBuffers(_source, 2, ids);
        _pendingBufferId = 0;
    } else {
        alSourceQueueBuffers(_source, 1, &bufferId);
    }
    
    // Autoplay
    ALint state = 0;
    alGetSourcei(_source, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING) {
        alSourcePlay(_source);
    }
    
#else

    if (_bqPlayerBufferQueue) {
    uint32_t cb = _buffer->_fill;
    if (cb > 0) {
        uint8_t* audioBuffer = (uint8_t*)malloc(cb);
        _audioBuffers.push_back(audioBuffer);
        _buffer->read(audioBuffer, cb);
        SLuint32 result = (*_bqPlayerBufferQueue)->Enqueue(_bqPlayerBufferQueue, audioBuffer, cb);
    }
    }

#endif
#endif
    
}


void AudioOutput::resume() {
#if TARGET_OS_IOS
    OSErr err = AudioOutputUnitStart(toneUnit);
    assert(err == noErr);
#endif
#ifndef OPENAL
    if (_bqPlayerPlay) {
        SLresult result = (*_bqPlayerPlay)->SetPlayState(_bqPlayerPlay, SL_PLAYSTATE_PLAYING);
        assert(SL_RESULT_SUCCESS == result);
    }
#endif
}

void AudioOutput::pause() {
#if TARGET_OS_IOS
    OSErr err = AudioOutputUnitStop(toneUnit);
    assert(err == noErr);
#endif
#ifndef OPENAL
    if (_bqPlayerPlay) {
        SLresult result = (*_bqPlayerPlay)->SetPlayState(_bqPlayerPlay, SL_PLAYSTATE_STOPPED);
        assert(SL_RESULT_SUCCESS == result);
    }
#endif
}
