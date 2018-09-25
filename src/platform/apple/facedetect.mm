//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#include "oaknut.h"
#import <CoreImage/CoreImage.h>

class FaceDetectorApple : public FaceDetector {
public:
    CIDetector* _detector;
    
    FaceDetectorApple() {
    
        CIContext* context =
#if TARGET_OS_IPHONE
            [CIContext contextWithEAGLContext:[EAGLContext currentContext]];
#else
            [CIContext contextWithCGLContext:CGLGetCurrentContext()
                                                  pixelFormat:nil
                                                   colorSpace:nil
                                                      options:nil];    
#endif

        _detector = [CIDetector detectorOfType:CIDetectorTypeFace
                           context:context
                           options:@{ CIDetectorAccuracy : CIDetectorAccuracyHigh }];
    }
    

    int detectFaces(Bitmap* bitmap) override {
        PIXELDATA pixeldata;
        bitmap->lock(&pixeldata, false);
        NSData* data = [NSData dataWithBytesNoCopy:pixeldata.data length:pixeldata.cb freeWhenDone:NO];
        CIImage* image = [CIImage imageWithBitmapData:data bytesPerRow:pixeldata.stride size:CGSizeMake(bitmap->_width,bitmap->_height) format:kCIFormatRGBA8 colorSpace:nil];
        //[CIImage imageWithTexture:tex->_textureId size: flipped:NO colorSpace:nil];
        NSArray *features = [_detector featuresInImage:image options:@{}];
        bitmap->unlock(&pixeldata, false);
        //app.log("faces: %d", features.count);
        return (int)features.count;
    }
};


FaceDetector* FaceDetector::create() {
    return new FaceDetectorApple();
}

#endif
