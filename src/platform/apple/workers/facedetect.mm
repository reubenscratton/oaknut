//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#include "oaknut.h"
#import <CoreImage/CoreImage.h>

class FaceDetectorWorker : public WorkerApple {
public:
    CIDetector* _detector;
    
    FaceDetectorWorker() {
    
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
    

    const variant process_(const variant& data_in) override {
        int width = data_in.intVal("width");
        int height = data_in.intVal("height");
        const bytearray& bytes = data_in.bytearrayVal("data");
        NSData* data = [NSData dataWithBytesNoCopy:bytes.data() length:bytes.length() freeWhenDone:NO];
        CIImage* image = [CIImage imageWithBitmapData:data bytesPerRow:width*4 size:CGSizeMake(width,height) format:kCIFormatRGBA8 colorSpace:nil];
        //[CIImage imageWithTexture:tex->_textureId size: flipped:NO colorSpace:nil];
        NSArray *features = [_detector featuresInImage:image options:@{}];
        //app.log("faces: %d", features.count);
        variant retval;
        retval.set("numFaces", (int)features.count);
        return retval;
    }
};

DECLARE_DYNCREATE(FaceDetectorWorker);


#endif
