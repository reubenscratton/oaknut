//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if PLATFORM_APPLE

#include <oaknut.h>
#include "audioinput.h"


bytearray AudioInputSamplesApple::getData() {
    CMBlockBufferRef buf = CMSampleBufferGetDataBuffer(_sampleBuffer);
    auto cb = CMBlockBufferGetDataLength(buf);
    bytearray bytes(cb);
    CMBlockBufferCopyDataBytes(buf, 0, cb, bytes.data());
    return bytes;
}

@interface AudioInputHelper : NSObject <AVCaptureAudioDataOutputSampleBufferDelegate>

@property (nonatomic) AudioInput* audioInput;

@end




/*
class AudioConverter {
 AudioConverterRef _audioConverter;
 AudioStreamBasicDescription _audioConverterInputFormat;
 NSMutableData* _audioConverterInputBuffer;
 NSMutableData* _appBuffer;
 NSMutableArray* _sentBuffers;
 _audioConverterInputBuffer = [NSMutableData new];
 _appBuffer = [NSMutableData new];
 _sentBuffers = [NSMutableArray new];

 void handleNewSamples() {
     // Instantiate a CoreAudio converter that will output at the desired rate
     if (!_audioConverter) {
         memcpy(&_audioConverterInputFormat, inFormat, sizeof(_audioConverterInputFormat));
         AudioStreamBasicDescription outFormat;
         memset(&outFormat, 0, sizeof(outFormat));
         FillOutASBDForLPCM(outFormat, _sampleRateOut, 1, 16, 16, false, false, false);
         OSStatus err = AudioConverterNew(inFormat, &outFormat, &_audioConverter);
         assert(!err);
        }
     [_sentBuffers removeAllObjects];
     }
 // Append the new sample buffer to the converter's input buffer
 CMBlockBufferRef blockBuffer;
 //AudioBufferList audioBufferList;
 size_t  bufferListSizeNeededOut = 0;
 OSStatus err = CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(sampleBuffer, &bufferListSizeNeededOut, NULL, 0, NULL,NULL,0,NULL);
 assert(err == 0);
 AudioBufferList* audioBufferList = (AudioBufferList*)malloc(bufferListSizeNeededOut);
 err=CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(sampleBuffer, NULL, audioBufferList, bufferListSizeNeededOut, NULL,NULL,0,&blockBuffer);
 for (NSUInteger i = 0; i < audioBufferList->mNumberBuffers; i++) {
    AudioBuffer audioBuffer = audioBufferList->mBuffers[i];
    [_audioConverterInputBuffer appendBytes:audioBuffer.mData length:audioBuffer.mDataByteSize];
}
free(audioBufferList);
CFRelease(blockBuffer);

// Ensure the converter's output buffer is big enough
uint32_t numPackets = (uint32_t) _audioConverterInputBuffer.length / _audioConverterInputFormat.mBytesPerPacket;


// Convert the audio data
uint32_t outputBufferSize = numPackets * sizeof(int16_t);
AudioBufferList outputBufferList;
outputBufferList.mNumberBuffers = 1;
outputBufferList.mBuffers[0].mNumberChannels = 1;
outputBufferList.mBuffers[0].mDataByteSize = outputBufferSize;
outputBufferList.mBuffers[0].mData = malloc(outputBufferSize);
memset(outputBufferList.mBuffers[0].mData, 0, outputBufferSize);
UInt32 numConvertedPackets = (numPackets-16)/ 3; // whatever number I specify here, callback will want 3x + 16 *packets*. How can that be right??
err = AudioConverterFillComplexBuffer(_audioConverter, EncoderDataProc, this, &numConvertedPackets, &outputBufferList, NULL);
assert(outputBufferList.mNumberBuffers == 1);
assert(err == 0 || err==1);


// Append converted data to the app buffer
[_appBuffer appendBytes:outputBufferList.mBuffers[0].mData length:outputBufferList.mBuffers[0].mDataByteSize];
free(outputBufferList.mBuffers[0].mData);

// Pass the converted data to the app
int numBytes = (int)_appBuffer.length;
onNewAudioSamples();
self.delegate->onNewAudioSamples(appBuffer.mutableBytes, numBytes);
//assert(numProcessed <= numAppPackets);

// Remove whatever the app processed from the front of its buffer
[_appBuffer replaceBytesInRange:NSMakeRange(0,numBytes) withBytes:nil length:0];

 }
 
 static OSStatus EncoderDataProc(AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets, AudioBufferList *ioData, AudioStreamPacketDescription **outDataPacketDescription, void *inUserData) {
 assert(!outDataPacketDescription);
 AudioInputApple* audioInput = (AudioInputApple*)inUserData;
 return audioInput->encoderCallback(ioNumberDataPackets, ioData);
 }
 OSStatus encoderCallback(UInt32* ioNumberDataPackets, AudioBufferList* ioData) {
 UInt32 cbWanted = *ioNumberDataPackets * _audioConverterInputFormat.mBytesPerPacket;
 //app.log("Callback wants %d packets (%d bytes) we have %d bytes available", *ioNumberDataPackets, cbWanted, self.audioConverterInputBuffer.length);
 //assert(cbWanted <= self.audioConverterInputBuffer.length);
 if (_audioConverterInputBuffer.length == 0) {
 ioData->mBuffers[0].mData = NULL;
 ioData->mBuffers[0].mDataByteSize = 0;
 *ioNumberDataPackets = 0;
 } else {
 if (cbWanted > _audioConverterInputBuffer.length) {
 cbWanted = (UInt32)_audioConverterInputBuffer.length;
 }
 
 *ioNumberDataPackets = cbWanted / _audioConverterInputFormat.mBytesPerFrame;
 
 // Extract the bytes we're providing this callback
 NSRange range = NSMakeRange(0, cbWanted);
 NSData* dataThisCallback = [_audioConverterInputBuffer subdataWithRange:range];
 [_audioConverterInputBuffer replaceBytesInRange:range withBytes:nil length:0];
 [_sentBuffers addObject:dataThisCallback];
 
 ioData->mBuffers[0].mData = (void*)dataThisCallback.bytes;
 ioData->mBuffers[0].mDataByteSize = (UInt32)dataThisCallback.length;
 ioData->mBuffers[0].mNumberChannels = _audioConverterInputFormat.mChannelsPerFrame;
 }
 return noErr;
 
 }
 

 }
 */


class AudioInputApple : public AudioInput {
public:
    
    AudioInputHelper* _helper;
    int _sampleRateIn, _sampleRateOut;
    AVCaptureSession* _session;
    AVCaptureDeviceInput* _audioCaptureDeviceInput;
    AVCaptureAudioDataOutput* _audioDataOutput;
    dispatch_queue_t _queue;
    
    AudioInputApple() {
        _helper = [AudioInputHelper new];
        _helper.audioInput = this;
    }

    void open(int sampleRate) override {
        _sampleRateIn = 22050; // supported on all iOS+macOS
        _sampleRateOut = sampleRate;
    }
    void start() override {
        _session = [[AVCaptureSession alloc] init];
        assert(_session);
        [_session setSessionPreset:AVCaptureSessionPreset640x480]; // ??? why is video here? is this copy paste error or does this do something useful?
        [_session beginConfiguration];
        AVCaptureDevice* audioCaptureDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeAudio];
        NSError* error = nil;
        _audioCaptureDeviceInput = [AVCaptureDeviceInput deviceInputWithDevice:audioCaptureDevice error:&error];
        [_session addInput:_audioCaptureDeviceInput];
        _audioDataOutput = [AVCaptureAudioDataOutput new];
        
#if TARGET_OS_OSX
        // CAVEAT! OSX 10.12 provides stereo *non-interleaved* buffers by default!
        // In 20+ years of software dev I've never seen non-interleaved stereo data, didn't
        // know it was a thing, and working out what the hell was going on cost me AN
        // ENTIRE DAY OF DEV. So to avoid the bloody pointless horror of it all we simply
        // force mono recording here.
        // TODO: Come back to this some day and figure out how to handle stereo formats.
        NSMutableDictionary * d = _audioDataOutput.audioSettings.mutableCopy;
        [d setObject:@(1) forKey:AVNumberOfChannelsKey];
        // Similarly OSX provides float data by default and the AudioConverter just
        // turns it into garbage! WTAF? Force it to record int16 here too...
        [d setObject:@(0) forKey:AVLinearPCMIsFloatKey];
        [d setObject:@(16) forKey:AVLinearPCMBitDepthKey];
        _audioDataOutput.audioSettings = d;
#endif
        _queue = dispatch_queue_create("AudioInput", DISPATCH_QUEUE_SERIAL);
        [_audioDataOutput setSampleBufferDelegate:_helper queue:_queue];
        [_session addOutput:_audioDataOutput];
        [_session commitConfiguration];
        dispatch_async(_queue, ^() {
            [_session startRunning];
        });
    }
    
    void handleNewSampleBuffer(CMSampleBufferRef sampleBuffer) {
        CMFormatDescriptionRef formatDescription = CMSampleBufferGetFormatDescription(sampleBuffer);
        const AudioStreamBasicDescription *inFormat = CMAudioFormatDescriptionGetStreamBasicDescription(formatDescription);
        assert(inFormat->mFramesPerPacket == 1);
        assert(inFormat->mChannelsPerFrame == 1);
        //bool isNonInterleaved = inFormat->mFormatFlags & kAudioFormatFlagIsNonInterleaved;

        sp<AudioInputSamplesApple> samples = new AudioInputSamplesApple();
        samples->_sampleBuffer = sampleBuffer;
        onNewAudioSamples(samples);
        
    }
    
    void stop() override {
        [_session stopRunning];
        [_session beginConfiguration];
        [_session removeOutput:_audioDataOutput];
        _audioDataOutput = nil;
        [_session removeInput:_audioCaptureDeviceInput];
        _audioCaptureDeviceInput = nil;
        [_session commitConfiguration];
    }
    void close() override {
    }
    
    
};

AudioInput* AudioInput::create() {
    return new AudioInputApple();
}


@implementation AudioInputHelper

- (void)captureOutput:(AVCaptureOutput *)captureOutput
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
       fromConnection:(AVCaptureConnection *)connection
{
    AudioInputApple* audioInput = (AudioInputApple*)_audioInput;
    audioInput->handleNewSampleBuffer(sampleBuffer);
}

@end


#endif

