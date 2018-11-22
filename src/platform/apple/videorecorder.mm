//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#import <oaknut.h>
#include "audioinput.h"


class VideoRecorderApple : public VideoRecorder {
public:
    NSURL* outputURL;
    dispatch_queue_t recordingQueue;
    AVAssetWriter* assetWriter;
    AVAssetWriterInput* assetWriterInputVideo;
    AVAssetWriterInput* assetWriterInputAudio;
    bool assetWriterSessionStarted;
    CFTimeInterval recordingStartTime;

    VideoRecorderApple(const string& outputPath) : VideoRecorder(outputPath) {

    }
    void start(SIZE size, int frameRate, int keyframeRate, int audioSampleRate) override {
        recordingQueue = dispatch_queue_create("RecordingQueue", DISPATCH_QUEUE_SERIAL);
        recordingStartTime = CACurrentMediaTime();
        int videoWidth = (int)size.width;
        int videoHeight = (int)size.height;
        
        dispatch_async(recordingQueue, ^{
            
            // Create a temporary file to write the asset to
            NSString* outputPath =  [NSString stringWithUTF8String:_outputPath.data()];
            outputURL = [NSURL fileURLWithPath: outputPath];
            [[NSFileManager defaultManager] removeItemAtURL:outputURL error:nil];
            
            // Create the writer
            assetWriter = [AVAssetWriter assetWriterWithURL:outputURL fileType:AVFileTypeMPEG4 error:nil];
            
            // Video input
            NSDictionary* videoOutputSettings = @{AVVideoCodecKey: AVVideoCodecH264,
                                                  AVVideoWidthKey: @(videoWidth),
                                                  AVVideoHeightKey: @(videoHeight),
                                                  AVVideoCompressionPropertiesKey: @{
                                                          AVVideoAverageBitRateKey: @(480000),
                                                          AVVideoProfileLevelKey: AVVideoProfileLevelH264Main41
                                                          }
                                                  };
            assetWriterInputVideo = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeVideo outputSettings:videoOutputSettings sourceFormatHint:nil];
            assetWriterInputVideo.expectsMediaDataInRealTime = YES;
            //assetWriterInputVideo.transform = CGAffineTransformMakeRotation(CGFloat(M_PI_2));
            [assetWriter addInput:assetWriterInputVideo];
            
            // Audio input
            NSDictionary* audioOutputSettings = @{
                                                  AVFormatIDKey: @(kAudioFormatMPEG4AAC),
                                                  AVNumberOfChannelsKey: @(1),
                                                  AVSampleRateKey: @(16000),
                                                  AVEncoderBitRateKey: @(32000)
                                                  };
            assetWriterInputAudio = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeAudio outputSettings:audioOutputSettings sourceFormatHint:nil];
            assetWriterInputAudio.expectsMediaDataInRealTime = YES;
            [assetWriter addInput:assetWriterInputAudio];
            
            assetWriterSessionStarted = NO;
            BOOL started = [assetWriter startWriting];
            assert(started);
        });
    }
    
    void ensureSessionStarted(CMSampleBufferRef sampleBuffer) {
        if (assetWriter && !assetWriterSessionStarted) {
            [assetWriter startSessionAtSourceTime:CMSampleBufferGetPresentationTimeStamp(sampleBuffer)];
            assetWriterSessionStarted = YES;
        }
    }

    void handleNewCameraFrame(CameraFrame* aframe) override {

        CameraFrameApple* frame = (CameraFrameApple*)aframe;
        CMSampleBufferRef sampleBuffer = frame->sampleBuffer;
        
        // Recording
        if (assetWriter) {
            CFRetain(sampleBuffer);
            dispatch_async(recordingQueue, ^{
                ensureSessionStarted(sampleBuffer);
                if ([assetWriterInputVideo isReadyForMoreMediaData]) {
                    BOOL ok = [assetWriterInputVideo appendSampleBuffer:sampleBuffer];
                    assert(ok);
                }
                CFRelease(sampleBuffer);
            });
        }
    }
    
    void handleNewAudioSamples(AudioSamples* audioSamples) override {
        //- (void)onAudioNewData:(CMSampleBufferRef)sampleBuffer {
        if (!assetWriter) {
            return;
        }
        CMSampleBufferRef sampleBuffer = ((AudioSamplesApple*)audioSamples)->_sampleBuffer;
        CFRetain(sampleBuffer);
        dispatch_async(recordingQueue, ^{
            ensureSessionStarted(sampleBuffer);
            if ([assetWriterInputAudio isReadyForMoreMediaData]) {
                BOOL ok = [assetWriterInputAudio appendSampleBuffer:sampleBuffer];
                assert(ok);
            }
            CFRelease(sampleBuffer);
        });
    }
    
    void stop(std::function<void()> onFinished) override {
        dispatch_async(recordingQueue, ^{
            [assetWriterInputAudio markAsFinished];
            [assetWriterInputVideo markAsFinished];
            AVAssetWriter* writer = assetWriter;
            assetWriter = nil;
            assetWriterInputAudio = nil;
            assetWriterInputVideo = nil;
            [writer finishWritingWithCompletionHandler:^{
                dispatch_async(dispatch_get_main_queue(), ^() {
                    onFinished();
                });
            }];
        });

    }

};

VideoRecorder* VideoRecorder::create(const string& outputPath) {
    return new VideoRecorderApple(outputPath);
}

#endif
