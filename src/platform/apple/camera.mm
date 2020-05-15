//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#include <oaknut.h>
#import <AVFoundation/AVFoundation.h>

Bitmap* CameraFrameApple::asBitmap() {
    CVPixelBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    return new BitmapApple(imageBuffer, true);
}

@interface CameraHelper : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate> {
@public
    Camera* _camera;
}
@end



class CameraApple : public Camera {
public:

    AVCaptureSession* captureSession;
    AVCaptureVideoDataOutput* videoOutput;
    CameraFrameApple _frame;
    bool _portrait;

    CameraApple(const Options& options) : Camera(options) {
        _helper = [CameraHelper new];
        _helper->_camera = this;
    }
    
    void open() override {
    }
    
    void start() override {
        
        
        // Get the video device that matches the front/back option
        AVCaptureDevice* videoDevice = nil;
        NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
        for (AVCaptureDevice* device in devices) {
            if (_options.frontFacing && [device position] == AVCaptureDevicePositionFront) {
                videoDevice = device;
            }
            if (!_options.frontFacing && [device position] != AVCaptureDevicePositionFront) {
                videoDevice = device;
            }
        }
        if(videoDevice == nil) {
            videoDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
        }
        assert(videoDevice);
        
        // Find the rate that
        AVCaptureDeviceFormat *bestFormat = nil;
        AVFrameRateRange *bestFrameRateRange = nil;
        uint32_t bestSizeDiff = INT32_MAX;
        for (AVCaptureDeviceFormat *format in [videoDevice formats]) {
            CMVideoDimensions dims = CMVideoFormatDescriptionGetDimensions(format.formatDescription);
            int shortSize = MIN(dims.width, dims.height);
            int longSize = MAX(dims.width, dims.height);
            log_dbg("short:%d long:%d", shortSize, longSize);
            // Calculate the size diff
            auto shortDiff = shortSize - _options.frameSizeShort;
            auto longDiff = longSize - _options.frameSizeLong;
            auto sizeDiff = shortDiff*shortDiff + longDiff*longDiff;
            
            // Get the size nearest the requested one
            if (!bestFormat || (sizeDiff < bestSizeDiff)) {
                bestFormat = format;
                bestSizeDiff = sizeDiff;
            }


            /*for ( AVFrameRateRange *range in format.videoSupportedFrameRateRanges ) {
                if ( range.maxFrameRate > bestFrameRateRange.maxFrameRate ) {
                    bestFormat = format;
                    bestFrameRateRange = range;
                }
            }*/
        }
        
        
        // Create input
        NSError* error = nil;
        AVCaptureDeviceInput *deviceInput = [AVCaptureDeviceInput deviceInputWithDevice:videoDevice error:&error];
        assert(!error);
        
        // Create output
        videoOutput = [[AVCaptureVideoDataOutput alloc] init];
        [videoOutput setAlwaysDiscardsLateVideoFrames:YES];
        [videoOutput setVideoSettings:@{
#if RENDERER_METAL
            (NSString *)kCVPixelBufferMetalCompatibilityKey: @YES,
#endif
            (NSString*)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA),
        }];
        [videoOutput setSampleBufferDelegate:_helper queue:dispatch_get_main_queue()];
        
        // Create session and add input and output. There is conflicting information on t'web
        // as to the right order to do these things.
        captureSession = [[AVCaptureSession alloc] init];
        assert(captureSession);
        [captureSession beginConfiguration];
        [captureSession addInput:deviceInput];
        [captureSession addOutput:videoOutput];
        [captureSession commitConfiguration];

        for (AVCaptureConnection *connection in [videoOutput connections]) {
            if([connection isVideoOrientationSupported]) {
                [connection setVideoOrientation:AVCaptureVideoOrientationPortrait];
            }
            if (_options.frontFacing) {
                [connection setVideoMirrored:YES];
            }
        }

        _portrait = true; // todo!
        
        auto formatDescription = [videoDevice activeFormat].formatDescription;
        auto dimensions = CMVideoFormatDescriptionGetDimensions(formatDescription);
        _previewWidth = _portrait ? dimensions.height : dimensions.width;
        _previewHeight = _portrait ? dimensions.width : dimensions.height;
        [captureSession startRunning];
        
        // Configure input
        NSError* err = nil;
        BOOL ok = [videoDevice lockForConfiguration:&err];
        assert(ok && !err);
        videoDevice.activeFormat = bestFormat;
#if TARGET_OS_IOS
        videoDevice.videoZoomFactor = 1.0f;
#endif
        [videoDevice unlockForConfiguration];
    }
    
    void handleNewSampleBuffer(CMSampleBufferRef sampleBuffer) {
        _frame.sampleBuffer = sampleBuffer;
        _frame._width = _previewWidth;
        _frame._height = _previewHeight;
        onNewCameraFrame(&_frame);
    }

    void stop() override {
        [captureSession stopRunning];
    }
    
    void close() override {
    }

    CameraHelper* _helper;
};

Camera* Camera::create(const Options& options) {
    return new CameraApple(options);
}



@implementation CameraHelper

//
// AVCaptureVideoDataOutputSampleBufferDelegate
//
- (void)captureOutput:(AVCaptureOutput *)captureOutput
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
       fromConnection:(AVCaptureConnection *)connection {
    
    CameraApple* camera = (CameraApple*)_camera;
    if (captureOutput == camera->videoOutput) {
        camera->handleNewSampleBuffer(sampleBuffer);
    }

}



@end

#endif
