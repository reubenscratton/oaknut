//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE && OAKNUT_WANT_CAMERA

#include <oaknut.h>
#include "camera.h"

Bitmap* CameraFrameApple::asBitmap() {
    CVPixelBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    return new Bitmap(imageBuffer, true);
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

    CameraApple(int cameraId) {
        _helper = [CameraHelper new];
        _helper->_camera = this;
    }
    
    void open() override {
    }
    
    void start() override {
        captureSession = [[AVCaptureSession alloc] init];
        assert(captureSession);
        [captureSession beginConfiguration];
        
        [captureSession setSessionPreset:AVCaptureSessionPreset640x480];
        
        // Get the a video device with preference to the front facing camera
        AVCaptureDevice* videoDevice = nil;
        NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
        for (AVCaptureDevice* device in devices) {
            if ([device position] == AVCaptureDevicePositionFront) {
                videoDevice = device;
            }
        }
        if(videoDevice == nil) {
            videoDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
        }
        assert(videoDevice);
        
        // Device input
        NSError* error = nil;
        AVCaptureDeviceInput *deviceInput = [AVCaptureDeviceInput deviceInputWithDevice:videoDevice error:&error];
        assert(!error);
        [captureSession addInput:deviceInput];
        
#if TARGET_OS_IOS
        [videoDevice lockForConfiguration:nil];
        videoDevice.videoZoomFactor = 1.0f;
        [videoDevice unlockForConfiguration];
#endif
      
        // Data output
        videoOutput = [[AVCaptureVideoDataOutput alloc] init];
        [videoOutput setAlwaysDiscardsLateVideoFrames:YES];
        [videoOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA] forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
        [videoOutput setSampleBufferDelegate:_helper queue:dispatch_get_main_queue()];
        
        [captureSession addOutput:videoOutput];
        for (AVCaptureConnection *connection in [videoOutput connections]) {
            if([connection isVideoOrientationSupported]) {
                [connection setVideoOrientation:AVCaptureVideoOrientationPortrait];
            }
            [connection setVideoMirrored:YES];
        }
        
        [captureSession commitConfiguration];
        
        auto formatDescription = [videoDevice activeFormat].formatDescription;
        auto dimensions = CMVideoFormatDescriptionGetDimensions(formatDescription);
        _previewWidth = dimensions.width;
        _previewHeight = dimensions.height;
        if (true) { // if (portrait) : todo: make this work
            _previewWidth = dimensions.height;
            _previewHeight = dimensions.width;
        }
        [captureSession startRunning];
    }
    
    void handleNewSampleBuffer(CMSampleBufferRef sampleBuffer) {
        _frame.sampleBuffer = sampleBuffer;
        onNewCameraFrame(&_frame);
    }

    void stop() override {
        [captureSession stopRunning];
    }
    
    void close() override {
    }

    CameraHelper* _helper;
};

Camera* Camera::create(int cameraId) {
    return new CameraApple(cameraId);
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
