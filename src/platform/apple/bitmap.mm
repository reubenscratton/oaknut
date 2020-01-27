//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE
#include <oaknut.h>


static CGBitmapInfo bitmapInfoForFormat(int format) {
    switch (format) {
        case PIXELFORMAT_RGBA32: return kCGImageAlphaPremultipliedLast;
        case PIXELFORMAT_BGRA32: return kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst;
        case PIXELFORMAT_RGB24: return kCGBitmapByteOrder32Little| kCGImageAlphaNone;
        case PIXELFORMAT_RGB565: return kCGImageAlphaNone;
        case PIXELFORMAT_A8: return kCGImageAlphaOnly;
        default: assert(0);
    }
    return 0;
}

static int bytesPerPixelForFormat(int format) {
    switch (format) {
        case PIXELFORMAT_RGBA32: return 4;
        case PIXELFORMAT_BGRA32: return 4;
        case PIXELFORMAT_RGB24: return 3;
        case PIXELFORMAT_RGB565: return 2;
        case PIXELFORMAT_A8: return 1;
        default: assert(0);
    }
    return 0;
}

BitmapApple::BitmapApple() {
}
BitmapApple::BitmapApple(int width, int height, int format) : BitmapApple(width, height, format, NULL, 0) {
}

BitmapApple::BitmapApple(int width, int height, int format, void* pixels, int stride) : Bitmap(width, height, format) {
    if (stride == 0) {
        stride = width*bytesPerPixelForFormat(format);
    }
    CGColorSpaceRef colorspace = (format==PIXELFORMAT_A8) ? CGColorSpaceCreateDeviceGray() : CGColorSpaceCreateDeviceRGB();
    
    _context = CGBitmapContextCreate(pixels, width, height, 8, stride, colorspace, bitmapInfoForFormat(format));
    // Flip Y. CoreGraphics bitmaps have origin at lower left but Oaknut coords are top left.
    CGContextScaleCTM(_context, 1, -1);
    CGContextTranslateCTM(_context, 0, -height);
    assert(_context);
}

static int getBitmapFormat(CVImageBufferRef cvImageBuffer) {
    OSType type = CVPixelBufferGetPixelFormatType(cvImageBuffer);
    if (type == 'BGRA') return PIXELFORMAT_BGRA32;
    if (type == 'RGBA') return PIXELFORMAT_RGBA32;
    assert(0);
}
BitmapApple::BitmapApple(CVImageBufferRef cvImageBuffer, bool fromCamera) : Bitmap((int)CVPixelBufferGetWidth(cvImageBuffer), (int)CVPixelBufferGetHeight(cvImageBuffer), getBitmapFormat(cvImageBuffer)) {
    _cvImageBuffer = cvImageBuffer;
    CFRetain(cvImageBuffer);
}

BitmapApple::~BitmapApple() {
    if (_cvImageBuffer) {
        CFRelease(_cvImageBuffer);
        _cvImageBuffer = NULL;
    }

    if (_context) {
        CGContextRelease(_context);
        _context = NULL;
    }
    if (_cfData) {
        CFRelease(_cfData);
        _cfData = NULL;
    }
}

void BitmapApple::lock(PIXELDATA* pixelData, bool forWriting) {

    // If wanting to write then try to create a Core Video image buffer that will give us direct
    // texture access. Not all pixel formats are supported so this is allowed to fail.
    // NB: This optimization is currently broken on OSX cos of GL_TEXTURE_RECTANGLE
#if TARGET_OS_IOS && !TARGET_OS_SIMULATOR
    if (forWriting && !_cvImageBuffer) {
        OSType pixelFormat = 0;
        switch (_format) {
            case PIXELFORMAT_RGBA32: pixelFormat = kCVPixelFormatType_32BGRA; break;
            case PIXELFORMAT_BGRA32: pixelFormat = kCVPixelFormatType_32BGRA; break;
            case PIXELFORMAT_RGB24: pixelFormat = kCVPixelFormatType_24RGB; break;
            case PIXELFORMAT_RGB565: pixelFormat = kCVPixelFormatType_16LE565; break;
            case PIXELFORMAT_A8: pixelFormat = kCVPixelFormatType_OneComponent8; break;
            default: assert(0); break;
        }

        NSDictionary* pixelBufferAttributes = @{
            (id)kCVPixelBufferPixelFormatTypeKey : @(pixelFormat),
            (id)kCVPixelFormatOpenGLESCompatibility : @YES,
            (id)kCVPixelBufferIOSurfacePropertiesKey : @{}
        };
        CVReturn err = CVPixelBufferCreate(kCFAllocatorDefault, _width, _height, pixelFormat,
            (__bridge CFDictionaryRef)(pixelBufferAttributes), &_cvImageBuffer);
        assert((_cvImageBuffer && err == kCVReturnSuccess) || err == kCVReturnInvalidPixelFormat);
        assert(!_texture); // if this hits then we need to recreate the texture so its a CoreVideoTexture!
    }
#endif

    if (_cvImageBuffer) {
        CVReturn err = CVPixelBufferLockBaseAddress(_cvImageBuffer, 0);
        assert(err == kCVReturnSuccess);
        pixelData->data = (uint8_t*)CVPixelBufferGetBaseAddress(_cvImageBuffer);
        pixelData->cb = (int)CVPixelBufferGetDataSize(_cvImageBuffer);
        pixelData->stride = (int)CVPixelBufferGetBytesPerRow(_cvImageBuffer);
    } else {
        pixelData->data = CGBitmapContextGetData(_context);
        pixelData->stride = (int)CGBitmapContextGetBytesPerRow(_context);
        pixelData->cb = _height * pixelData->stride;
    }
    assert(pixelData->data && pixelData->cb && pixelData->stride);
}



void BitmapApple::unlock(PIXELDATA* pixelData, bool pixelDataChanged) {
    if (_cvImageBuffer) {
        CVReturn err = CVPixelBufferUnlockBaseAddress(_cvImageBuffer, 0);
        assert(err == kCVReturnSuccess);
    }
    if (pixelDataChanged) {
        texInvalidate();
    }
}




void BitmapApple::fromVariant(const variant& v) {
    Bitmap::fromVariant(v);
    int32_t stride = v.intVal("s");
    auto& bb = v.bytearrayRef("bb");
    CGColorSpaceRef colorspace = (_format==PIXELFORMAT_A8) ? CGColorSpaceCreateDeviceGray() : CGColorSpaceCreateDeviceRGB();
    _context = CGBitmapContextCreateWithData(NULL, _width, _height, 8, stride, colorspace, bitmapInfoForFormat(_format), nil, nil);
    void* pixels = CGBitmapContextGetData(_context);
    memcpy(pixels, bb.data(), bb.size());

    // Flip Y. CoreGraphics bitmaps have origin at lower left but Oaknut coords are top left.
    CGContextScaleCTM(_context, 1, -1);
    CGContextTranslateCTM(_context, 0, -_height);
    assert(_context);
}
void BitmapApple::toVariant(variant& v) {
    Bitmap::toVariant(v);
    
    PIXELDATA pixelData;
    lock(&pixelData, false);
    v.set("s", pixelData.stride);
    v["bb"] = bytearray((uint8_t*)pixelData.data, pixelData.cb);
    unlock(&pixelData, false);
}



Bitmap* Bitmap::createFromData(bytearray& data) {
    
    // Detach the compressed image data so nothing owns it
    uint8_t* pd = data.data();
    uint32_t cb = data.size();
    assert(pd && cb);
    data.detach();
    

    CGDataProviderRef dataProvider = CGDataProviderCreateWithData(NULL, pd, cb, NULL);
    CGImageRef cgImage;
    auto hdr = pd;
    bool isPNG = (hdr[0]==0x89 && hdr[1]=='P' && hdr[2]=='N' && hdr[3]=='G');
    if (isPNG) {
        cgImage = CGImageCreateWithPNGDataProvider(dataProvider, NULL, NO, kCGRenderingIntentDefault);
    } else if (hdr[0]==0xFF && hdr[1]==0xD8) {
        cgImage = CGImageCreateWithJPEGDataProvider(dataProvider, NULL, NO, kCGRenderingIntentDefault);
    } else {
        //NSImage* img = [[NSImage alloc] initWithData:[NSData dataWithBytesNoCopy:data.data() length:data.size()]];
        //cgImage = [img CGImageForProposedRect:nil context:nil hints: nil];
        //if (!cgImage) {
            return NULL;
        //}
        //CGImageRetain(cgImage);
    }
    CGColorSpaceRef colorspace = CGImageGetColorSpace(cgImage);
    CGColorSpaceModel colormodel = CGColorSpaceGetModel(colorspace);
    size_t bpp = CGImageGetBitsPerPixel(cgImage);
    if (bpp < 8 || bpp > 32 /*|| (colormodel != kCGColorSpaceModelMonochrome && colormodel != kCGColorSpaceModelRGB)*/) {
        assert(0 && "unsupported CGImage type");
        return NULL;
    }

    // Parse CGImage info
    int width = (int)CGImageGetWidth(cgImage);
    int height = (int)CGImageGetHeight(cgImage);
    CGBitmapInfo info = CGImageGetBitmapInfo(cgImage); // may be RGBA, BGRA, or ARGB
    
    // Get a pointer to the uncompressed image data.
    // This allows access to the original (possibly unpremultiplied) data, but any manipulation
    // (such as scaling) has to be done manually. Contrast this with drawing the image
    // into a CGBitmapContext, which allows scaling, but always forces premultiplication.
    CGDataProviderRef pro = CGImageGetDataProvider(cgImage);
    CFDataRef dataRef = CGDataProviderCopyData(pro);
    assert(dataRef);
    GLubyte* pixels = (GLubyte *)CFDataGetBytePtr(dataRef);
    assert(pixels);
    size_t cbUncompressed = CFDataGetLength(dataRef);
    CGImageRelease(cgImage);

    
    // Conversions from unsupported pixel formats
    int format = PIXELFORMAT_UNKNOWN;
    if (bpp==16 && colormodel==kCGColorSpaceModelMonochrome) {  // A16 ==> A8
        format = PIXELFORMAT_A8;
        uint8_t* p1 = (uint8_t*)pixels;
        uint16_t* p2 = (uint16_t*)pixels;
        for (int i=0 ; i<width*height ; i++) {
            uint16_t vp2 = *p2++;
            *p1++ = vp2>>8;
        }
        cbUncompressed >>= 1;
        bpp = 8;
    }

    // Choose OpenGL format
    switch(bpp) {
        case 32:
            switch(info & kCGBitmapAlphaInfoMask) {
                case kCGImageAlphaPremultipliedFirst:
                case kCGImageAlphaFirst:
                case kCGImageAlphaNoneSkipFirst:
                    format = PIXELFORMAT_BGRA32;
                    break;
                default:
                    format = PIXELFORMAT_RGBA32;
            }
            break;
        case 24:
            format = PIXELFORMAT_RGB24;
            assert(0); // Can Metal or GL on iOS support these or do they need conversion? No idea...
            break;
        case 16:
            format = PIXELFORMAT_RGB565;
            break;
        case 8:
            format = PIXELFORMAT_A8;
            break;
        default:
            assert(0);
            return NULL;
    }
    
    // Convert ARGB to BGRA (NB: CGImage only produces ARGB on simulator)
    if (format == PIXELFORMAT_BGRA32) {
        if ((info & kCGBitmapByteOrderMask) != kCGBitmapByteOrder32Host) {
            info = (info&~kCGBitmapByteOrderMask) | kCGBitmapByteOrder32Host;
            uint32_t* p = (uint32_t*)pixels;
            for (int i = 0; i < width * height; i++)
                p[i] = (p[i] << 24) | ((p[i] & 0xFF00) << 8) | ((p[i] >> 8) & 0xFF00) | (p[i] >> 24);
        }
    }

    // If on iOS hardware convert RGBA to BGRA as the GPU greatly prefers it and we get
    // to use CoreVideo direct access
#if !TARGET_SIMULATOR
    if (format == PIXELFORMAT_RGBA32) {
        format = PIXELFORMAT_BGRA32;
        uint32_t* p = (uint32_t*)pixels;
        for (int i = 0; i < width * height; i++)
            p[i] = (p[i] & 0xFF00FF00) | ((p[i] & 0xFF) << 16) | ((p[i]&0xFF0000)>>16);
    }
#endif
    

    // Create the native bitmap
    BitmapApple* bitmap = new BitmapApple(width, height, format, pixels, (int)cbUncompressed/height);
    bitmap->_cfData = dataRef;
    return bitmap;

}

Bitmap* Bitmap::create(int width, int height, int format) {
    return new BitmapApple(width, height, format);
}





#endif
