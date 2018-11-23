//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if PLATFORM_APPLE && 0 // code isnt ready yet

#include <oaknut.h>


class JpegEncoderWorker : public WorkerImpl {
public:
    
    JpegEncoderWorker() : WorkerImpl() {
    }
    
    void start_(const variant& config) override {
    }
    
    variant process_(const variant& data_in) override {
        
        float quality = data_in.floatVal("quality");
        int width = data_in.intVal("width");
        int height = data_in.intVal("height");
        int format = data_in.intVal("format");
        const bytearray& data = data_in.bytearrayVal("data");
        const unsigned char *imageData = (const unsigned char *)data.data();

#if TARGET_OS_IOS
        UIImage* image = nil;
        if (_context) {
            CGImageRef imgRef = CGBitmapContextCreateImage(_context);
            image = [UIImage imageWithCGImage:imgRef];
            CGImageRelease(imgRef);
        }
        else if (_cvImageBuffer) {
            CIImage* imgRef = [CIImage imageWithCVImageBuffer:_cvImageBuffer];
            CGImageRef cgImage = [[CIContext contextWithOptions:nil] createCGImage:imgRef fromRect:CGRectMake(0,0,_width,_height)];
            image = [UIImage imageWithCGImage:cgImage];
        }
        else {
            assert(0); // oops! todo: support other kinds of image (gl texture, probably)
        }
        NSData* data = UIImageJPEGRepresentation(image, quality);
#else
        NSBitmapImageRep* bitmapRep = nil;
        if (_context) {
            CGImageRef imgRef = CGBitmapContextCreateImage(_context);
            bitmapRep = [[NSBitmapImageRep alloc] initWithCGImage:imgRef];
        }
        else if (_cvImageBuffer) {
            CIImage* imgRef = [CIImage imageWithCVImageBuffer:_cvImageBuffer];
            bitmapRep = [[NSBitmapImageRep alloc] initWithCIImage:imgRef];
        }
        else {
            assert(0); // oops! todo: support other kinds of image (gl texture, probably)
        }
        NSData* data = [bitmapRep representationUsingType:NSJPEGFileType properties:@{NSImageCompressionFactor:@(quality)}];
#endif
            return bytearray((uint8_t*)data.bytes, (int)data.length);
        }

        return out.getWrittenBytes();
    }
};

DECLARE_WORKER_IMPL(JpegEncoderWorker);

#endif

