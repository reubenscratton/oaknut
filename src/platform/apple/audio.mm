//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
#if PLATFORM_APPLE

#include <oaknut.h>
#import <AVFoundation/AVFoundation.h>
#import <AVFoundation/AVCaptureDevice.h>
#import <AVFoundation/AVCaptureOutput.h>

#define BUFFER_SIZE 8192




@interface AudioInputHelper : NSObject <AVCaptureAudioDataOutputSampleBufferDelegate>

@property (nonatomic) int sampleRate;
@property (nonatomic) AVCaptureSession* session;
@property (nonatomic) AVCaptureDeviceInput* audioCaptureDeviceInput;
@property (nonatomic) AVCaptureAudioDataOutput* audioDataOutput;
@property (nonatomic) dispatch_queue_t queue;
@property (nonatomic) AudioConverterRef audioConverter;
@property (nonatomic) AudioStreamBasicDescription audioConverterInputFormat;
@property (nonatomic) NSMutableData* audioConverterInputBuffer;
@property (nonatomic) NSMutableData* appBuffer;
@property (nonatomic) AudioInput* delegate;
@property (nonatomic) NSMutableArray* sentBuffers;

- (OSStatus)encoderCallback:(UInt32*)ioNumberDataPackets ioData:(AudioBufferList*)ioData;

@end

/**
 @param      inAudioConverter       The AudioConverter requesting input.
 @param      ioNumberDataPackets
                                     On entry, the minimum number of packets of input audio data the converter
                                     would like in order to fulfill its current FillBuffer request. On exit, the
                                     number of packets of audio data actually being provided for input, or 0 if
                                     there is no more input.
 @param      ioData
                                     On exit, the members of ioData should be set to point to the audio data
                                     being provided for input.
 @param      outDataPacketDescription
                                     If non-null, on exit, the callback is expected to fill this in with
                                     an AudioStreamPacketDescription for each packet of input data being provided.
 @param      inUserData
                                 The inInputDataProcUserData parameter passed to AudioConverterFillComplexBuffer().

 */
static OSStatus EncoderDataProc(AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets, AudioBufferList *ioData, AudioStreamPacketDescription **outDataPacketDescription, void *inUserData) {
    assert(!outDataPacketDescription);
    AudioInputHelper* audioInput = (__bridge AudioInputHelper*)inUserData;
    return [audioInput encoderCallback:ioNumberDataPackets ioData:ioData];
}

@implementation AudioInputHelper

- (void)start {
    self.session = [[AVCaptureSession alloc] init];
    assert(self.session);
    [self.session setSessionPreset:AVCaptureSessionPreset640x480];
    [self.session beginConfiguration];
    AVCaptureDevice* audioCaptureDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeAudio];
    NSError* error = nil;
    self.audioCaptureDeviceInput = [AVCaptureDeviceInput deviceInputWithDevice:audioCaptureDevice error:&error];
    [self.session addInput:self.audioCaptureDeviceInput];
    self.audioDataOutput = [AVCaptureAudioDataOutput new];
    
#if TARGET_OS_OSX
    // CAVEAT! OSX 10.12 provides stereo *non-interleaved* buffers by default!
    // In 20+ years of software dev I've never seen non-interleaved stereo data, didn't
    // know it was a thing, and working out what the hell was going on cost me AN
    // ENTIRE DAY OF DEV. So to avoid the bloody pointless horror of it all we simply
    // force mono recording here.
    // TODO: Come back to this some day and figure out how to handle stereo formats.
    NSMutableDictionary * d = self.audioDataOutput.audioSettings.mutableCopy;
    [d setObject:@(1) forKey:AVNumberOfChannelsKey];
    // Similarly OSX provides float data by default and the AudioConverter just
    // turns it into garbage! WTAF? Force it to record int16 here too...
    [d setObject:@(0) forKey:AVLinearPCMIsFloatKey];
    [d setObject:@(16) forKey:AVLinearPCMBitDepthKey];
    self.audioDataOutput.audioSettings = d;
#endif
    _queue = dispatch_queue_create("AudioInput", DISPATCH_QUEUE_SERIAL);
    [self.audioDataOutput setSampleBufferDelegate:self queue:_queue];
    [self.session addOutput:self.audioDataOutput];
    [self.session commitConfiguration];
    self.audioConverterInputBuffer = [NSMutableData new];
    self.appBuffer = [NSMutableData new];
    self.sentBuffers = [NSMutableArray new];
    dispatch_async(_queue, ^() {
        [self.session startRunning];
    });
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
       fromConnection:(AVCaptureConnection *)connection
{
    CMFormatDescriptionRef formatDescription = CMSampleBufferGetFormatDescription(sampleBuffer);
    const AudioStreamBasicDescription *inFormat = CMAudioFormatDescriptionGetStreamBasicDescription(formatDescription);
    assert(inFormat->mFramesPerPacket == 1);
    assert(inFormat->mChannelsPerFrame == 1);
    //bool isNonInterleaved = inFormat->mFormatFlags & kAudioFormatFlagIsNonInterleaved;
    
    // Instantiate a CoreAudio converter that will output at the desired rate
    if (!self.audioConverter) {
        memcpy(&_audioConverterInputFormat, inFormat, sizeof(_audioConverterInputFormat));
        AudioStreamBasicDescription outFormat;
        memset(&outFormat, 0, sizeof(outFormat));
        FillOutASBDForLPCM(outFormat, self.sampleRate, 1, 16, 16, false, false, false);
        OSStatus err = AudioConverterNew(inFormat, &outFormat, &_audioConverter);
        assert(!err);
    }
    //AudioConverterReset(self.audioConverter);
    [self.sentBuffers removeAllObjects];
    
    //CMBlockBufferRef blockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);


    // Append the new sample buffer to the converter's input buffer
    CMBlockBufferRef blockBuffer;
    //AudioBufferList audioBufferList;
    size_t  bufferListSizeNeededOut = 0;
    OSStatus err = CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(sampleBuffer, &bufferListSizeNeededOut, NULL, 0, NULL,NULL,0,NULL);
    assert(err == 0);
    AudioBufferList* audioBufferList = (AudioBufferList*)malloc(bufferListSizeNeededOut);
    err=CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(sampleBuffer, NULL, audioBufferList, bufferListSizeNeededOut, NULL,NULL,0,&blockBuffer);
    /*&audioBufferList, sizeof(AudioBufferList), NULL, NULL, kCMSampleBufferFlag_AudioBufferList_Assure16ByteAlignment, &blockBuffer);*/
    for (NSUInteger i = 0; i < audioBufferList->mNumberBuffers; i++) {
        AudioBuffer audioBuffer = audioBufferList->mBuffers[i];
        [self.audioConverterInputBuffer appendBytes:audioBuffer.mData length:audioBuffer.mDataByteSize];
    }
    free(audioBufferList);
    CFRelease(blockBuffer);
    
    // Ensure the converter's output buffer is big enough
    uint32_t numPackets = (uint32_t) self.audioConverterInputBuffer.length / self.audioConverterInputFormat.mBytesPerPacket;
    
    
    // Convert the audio data
    uint32_t outputBufferSize = numPackets * sizeof(int16_t);
    AudioBufferList outputBufferList;
    outputBufferList.mNumberBuffers = 1;
    outputBufferList.mBuffers[0].mNumberChannels = 1;
    outputBufferList.mBuffers[0].mDataByteSize = outputBufferSize;
    outputBufferList.mBuffers[0].mData = malloc(outputBufferSize);
    memset(outputBufferList.mBuffers[0].mData, 0, outputBufferSize);
    UInt32 numConvertedPackets = (numPackets-16)/ 3; // whatever number I specify here, callback will want 3x + 16 *packets*. How can that be right??
    err = AudioConverterFillComplexBuffer(_audioConverter, EncoderDataProc, (__bridge void*)self, &numConvertedPackets, &outputBufferList, NULL);
    assert(outputBufferList.mNumberBuffers == 1);
    assert(err == 0 || err==1);

    
    // Append converted data to the app buffer
    [self.appBuffer appendBytes:outputBufferList.mBuffers[0].mData length:outputBufferList.mBuffers[0].mDataByteSize];
    free(outputBufferList.mBuffers[0].mData);

    // Pass the converted data to the app
    int numAppPackets = (int)self.appBuffer.length/sizeof(int16_t);
    int numProcessed = self.delegate->onNewAudioSamples(numAppPackets, (int16_t*)self.appBuffer.bytes);
    assert(numProcessed <= numAppPackets);
    
    // Remove whatever the app processed from the front of its buffer
    [self.appBuffer replaceBytesInRange:NSMakeRange(0,numProcessed*sizeof(int16_t)) withBytes:nil length:0];

}

- (OSStatus)encoderCallback:(UInt32*)ioNumberDataPackets ioData:(AudioBufferList*)ioData {
    UInt32 cbWanted = *ioNumberDataPackets * self.audioConverterInputFormat.mBytesPerPacket;
    //app.log("Callback wants %d packets (%d bytes) we have %d bytes available", *ioNumberDataPackets, cbWanted, self.audioConverterInputBuffer.length);
    //assert(cbWanted <= self.audioConverterInputBuffer.length);
    if (self.audioConverterInputBuffer.length == 0) {
        ioData->mBuffers[0].mData = NULL;
        ioData->mBuffers[0].mDataByteSize = 0;
        *ioNumberDataPackets = 0;
    } else {
        if (cbWanted > self.audioConverterInputBuffer.length) {
            cbWanted = (UInt32)self.audioConverterInputBuffer.length;
        }
    
        *ioNumberDataPackets = cbWanted / self.audioConverterInputFormat.mBytesPerFrame;
        
        // Extract the bytes we're providing this callback
        NSRange range = NSMakeRange(0, cbWanted);
        NSData* dataThisCallback = [self.audioConverterInputBuffer subdataWithRange:range];
        [self.audioConverterInputBuffer replaceBytesInRange:range withBytes:nil length:0];
        [self.sentBuffers addObject:dataThisCallback];

        ioData->mBuffers[0].mData = (void*)dataThisCallback.bytes;
        ioData->mBuffers[0].mDataByteSize = (UInt32)dataThisCallback.length;
        ioData->mBuffers[0].mNumberChannels = self.audioConverterInputFormat.mChannelsPerFrame;
    }
    return noErr;

}
- (void)stop {
    [self.session stopRunning];
    [self.session beginConfiguration];
    [self.session removeOutput:self.audioDataOutput];
    self.audioDataOutput = nil;
    [self.session removeInput:self.audioCaptureDeviceInput];
    self.audioCaptureDeviceInput = nil;
    [self.session commitConfiguration];
    
}

@end


class AudioInputApple : public AudioInput {
public:
    
    // API
    AudioInputApple() {
        sampleRate = 22050; // assume this is supported on all iOS+macOS
    }
    
    void open() override {
        _helper = [AudioInputHelper new];
        _helper.sampleRate = sampleRate;
        CFBridgingRetain(_helper);
    }
    void start() override {
        _helper.delegate = this;
        [_helper start];
    }
    void stop() override {
        [_helper stop];
    }
    void close() override {
        //CFBridgingRelease(_helper);
        _helper = nil;
    }
    
    
    AudioInputHelper* _helper;
};

AudioInput* AudioInput::create() {
    return new AudioInputApple();
}

#endif

