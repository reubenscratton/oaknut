//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#include <oaknut.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreImage/CoreImage.h>


@interface CameraHelper : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate> {
@public
    Camera* _delegate;
}

@property (nonatomic) AVCaptureSession* captureSession;
@property (nonatomic) AVCaptureVideoDataOutput* videoOutput;

@end

@implementation CameraHelper

- (void)startPreview {
    self.captureSession = [[AVCaptureSession alloc] init];
    assert(self.captureSession);
    [self.captureSession beginConfiguration];
    
    [self.captureSession setSessionPreset:AVCaptureSessionPreset640x480];
    
    
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
    [self.captureSession addInput:deviceInput];
    
#if TARGET_OS_IOS
    [videoDevice lockForConfiguration:nil];
    videoDevice.videoZoomFactor = 1.0f;
    [videoDevice unlockForConfiguration];
#endif

    self.videoOutput = [[AVCaptureVideoDataOutput alloc] init];
    [self.videoOutput setAlwaysDiscardsLateVideoFrames:YES];
    [self.videoOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA] forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
    [self.videoOutput setSampleBufferDelegate:self queue:dispatch_get_main_queue()];
    
    [self.captureSession addOutput:self.videoOutput];
    for (AVCaptureConnection *connection in [self.videoOutput connections]) {
        if([connection isVideoOrientationSupported]) {
            [connection setVideoOrientation:AVCaptureVideoOrientationPortrait];
        }
        [connection setVideoMirrored:YES];
    }
    
    [self.captureSession commitConfiguration];
    [self.captureSession startRunning];
}

- (void)stopPreview {
    [self.captureSession stopRunning];
}

//
// AVCaptureVideoDataOutputSampleBufferDelegate
//
- (void)captureOutput:(AVCaptureOutput *)captureOutput
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
       fromConnection:(AVCaptureConnection *)connection {
    if (captureOutput == self.videoOutput) {

        
        // Get brightness
        CFDictionaryRef metadataDict = CMCopyDictionaryOfAttachments(NULL, sampleBuffer, kCMAttachmentMode_ShouldPropagate);
        NSDictionary *metadata = [[NSMutableDictionary alloc] initWithDictionary:(__bridge NSDictionary*)metadataDict];
        CFRelease(metadataDict);
        NSDictionary *exifMetadata = metadata[(NSString*)kCGImagePropertyExifDictionary];
        float brightnessValue = exifMetadata ? [exifMetadata[(NSString *)kCGImagePropertyExifBrightnessValue] floatValue] : 5.0f; // assume midrange brightness if exif data doesnt have it
        
        // Create the new bitmap
        CVPixelBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
        ObjPtr<Bitmap> bitmap = new Bitmap(imageBuffer, true);
        
        // Call delegate
        _delegate->onNewCameraFrame(bitmap, brightnessValue);
    }

}



@end


class CameraApple : public Camera {
public:
    
    CameraApple(int cameraId) {
    }
    
    void open() override {
        _helper = [CameraHelper new];
    }
    
    void start() override {
        _helper->_delegate = this;
        [_helper startPreview];
    }
    
    void stop() override {
        _helper->_delegate = NULL;
        [_helper stopPreview];
    }
    
    void close() override {
//        CFBridgingRelease(_helper);
        _helper = nil;
    }

    CameraHelper* _helper;
};

Camera* Camera::create(int cameraId) {
    return new CameraApple(cameraId);
}


#endif
