//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#include "oaknut.h"
#import <CoreImage/CoreImage.h>


void* oakFaceDetectorCreate() {
    
    CIContext* context =
#if TARGET_OS_IPHONE
        [CIContext contextWithEAGLContext:[EAGLContext currentContext]];
#else
         [CIContext contextWithCGLContext:CGLGetCurrentContext()
                                                  pixelFormat:nil
                                                   colorSpace:nil
                                                      options:nil];    
#endif

    return (__bridge_retained void*)
        [CIDetector detectorOfType:CIDetectorTypeFace
                           context:context
                           options:@{ CIDetectorAccuracy : CIDetectorAccuracyHigh }];
}

int oakFaceDetectorDetectFaces(void* osobj, Bitmap* bitmap) {
    CIDetector* detector = (__bridge CIDetector*)osobj;
    PIXELDATA pixeldata;
    bitmap->lock(&pixeldata, false);
    NSData* data = [NSData dataWithBytesNoCopy:pixeldata.data length:pixeldata.cb freeWhenDone:NO];
    CIImage* image = [CIImage imageWithBitmapData:data bytesPerRow:pixeldata.stride size:CGSizeMake(bitmap->_width,bitmap->_height) format:kCIFormatRGBA8 colorSpace:nil];
    //[CIImage imageWithTexture:tex->_textureId size: flipped:NO colorSpace:nil];
    NSArray *features = [detector featuresInImage:image options:@{}];
    bitmap->unlock(&pixeldata, false);
    //app.log("faces: %d", features.count);
    return (int)features.count;
}
void oakFaceDetectorClose(void* osobj) {
    CIDetector* __unused detector = (__bridge_transfer CIDetector*)osobj;
}

#endif
