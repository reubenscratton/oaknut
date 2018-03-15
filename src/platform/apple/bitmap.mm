//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#include "bitmap.h"

static CVOpenGLESTextureCacheRef s_cvTextureCacheCamera;
static CVOpenGLESTextureCacheRef s_cvTextureCacheOther;

static CVOpenGLESTextureCacheRef createTextureCache() {
    CVOpenGLESTextureCacheRef cache;
    CVReturn err = CVOpenGLESTextureCacheCreate(NULL, NULL, GLGetCurrentContext(),
#if !TARGET_OS_IOS
                                                CGLGetPixelFormat(CGLGetCurrentContext()),
#endif
                                                NULL,
                                                &cache);
    assert(err==0);
    return cache;
}

static CVOpenGLESTextureCacheRef getTextureCache(bool camera) {
    if (camera) {
        if (!s_cvTextureCacheCamera) {
            s_cvTextureCacheCamera = createTextureCache();
        }
        return s_cvTextureCacheCamera;
    }
    if (!s_cvTextureCacheOther) {
        s_cvTextureCacheOther = createTextureCache();
    }
    return s_cvTextureCacheOther;
}


static CGBitmapInfo bitmapInfoForFormat(int format) {
    switch (format) {
        case BITMAPFORMAT_RGBA32: return kCGImageAlphaPremultipliedLast;
        case BITMAPFORMAT_BGRA32: return kCGImageAlphaPremultipliedLast;
        case BITMAPFORMAT_RGB565: return kCGImageAlphaNone;
        case BITMAPFORMAT_A8: return kCGImageAlphaOnly;
        default: assert(0);
    }
    return 0;
}

static int bytesPerPixelForFormat(int format) {
    switch (format) {
        case BITMAPFORMAT_RGBA32: return 4;
        case BITMAPFORMAT_BGRA32: return 4;
        case BITMAPFORMAT_RGB565: return 2;
        case BITMAPFORMAT_A8: return 1;
        default: assert(0);
    }
    return 0;
}


OSBitmap::OSBitmap(int width, int height, int format, void* pixels, int stride) : Bitmap(width, height, format) {
    if (stride == 0) {
        stride = width*bytesPerPixelForFormat(format);
    }
    CGColorSpaceRef colorspace = (format==BITMAPFORMAT_A8) ? CGColorSpaceCreateDeviceGray() : CGColorSpaceCreateDeviceRGB();
    _context = CGBitmapContextCreate(pixels, width, height, 8, stride, colorspace, bitmapInfoForFormat(format));
    // Flip Y. CoreGraphics bitmaps have origin at lower left but Oaknut coords are top left.
    CGContextScaleCTM(_context, 1, -1);
    CGContextTranslateCTM(_context, 0, -height);
    assert(_context);
    _needsUpload = true;
}

OSBitmap::OSBitmap(CVImageBufferRef cvImageBuffer, bool fromCamera) : Bitmap((int)CVPixelBufferGetWidth(cvImageBuffer), (int)CVPixelBufferGetHeight(cvImageBuffer), BITMAPFORMAT_RGBA32) {
    _cvImageBuffer = cvImageBuffer;
    _cvTextureCache = getTextureCache(fromCamera);
    
#if TARGET_OS_IOS
    CVReturn err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, _cvTextureCache, cvImageBuffer, NULL, GL_TEXTURE_2D, GL_RGBA , _width, _height, GL_BGRA, GL_UNSIGNED_BYTE, 0, &_cvTexture);
    _textureId = CVOpenGLESTextureGetName(_cvTexture);
#else
    CVReturn err = CVOpenGLTextureCacheCreateTextureFromImage(NULL, _cvTextureCache,
                                                              cvImageBuffer, NULL, &_cvTexture);
    _textureId = CVOpenGLTextureGetName(_cvTexture);
    _texTarget = CVOpenGLESTextureGetTarget(_cvTexture);
    if (_texTarget != GL_TEXTURE_2D) {
        
        GLint oldFBO, oldFBOread, oldTex;
        check_gl(glGetIntegerv, GL_FRAMEBUFFER_BINDING, &oldFBO);
        check_gl(glGetIntegerv, GL_READ_FRAMEBUFFER_BINDING, &oldFBOread);
        check_gl(glGetIntegerv, GL_TEXTURE_BINDING_2D, &oldTex);
        
        // Set up the camera texture for framebuffer read
        GLuint fbr = 0;
        check_gl(glGenFramebuffers, 1, &fbr);
        check_gl(glBindFramebuffer, GL_READ_FRAMEBUFFER, fbr);
        check_gl(glFramebufferTexture2D, GL_READ_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,_texTarget,_textureId,0);
        check_gl(glReadBuffer, GL_COLOR_ATTACHMENT0);
        
        // Create a new GL_TEXTURE_2D texture for framebuffer write
        GLuint fb = 0;
        check_gl(glGenFramebuffers, 1, &fb);
        check_gl(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, fb);
        GLuint texId2 = 0;
        check_gl(glGenTextures, 1, &texId2);
        check_gl(glBindTexture, GL_TEXTURE_2D, texId2);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        check_gl(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        check_gl(glFramebufferTexture2D, GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texId2,0);
        
        // Copy
        check_gl(glCopyTexSubImage2D, GL_TEXTURE_2D,0,0,0,0,0,_width,_height);
        _texTarget = GL_TEXTURE_2D;
        _textureId = texId2;
        
        check_gl(glBindTexture, GL_TEXTURE_2D, oldTex);
        check_gl(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, oldFBO);
        check_gl(glBindFramebuffer, GL_READ_FRAMEBUFFER, oldFBOread);
        check_gl(glDeleteFramebuffers, 1, &fbr);
        check_gl(glDeleteFramebuffers, 1, &fb);
        
        // Delete the CV texture now we've copied it to a normal one
        CVOpenGLTextureRelease(_cvTexture);
        _cvTexture = NULL;
        CVOpenGLTextureCacheFlush(_cvTextureCache, 0);
        _cvTextureCache = NULL;
        
    }
#endif
    assert(err==0);
}

OSBitmap::~OSBitmap() {
    if (_textureId) {
        if (!_cvTextureCache) {
            check_gl(glDeleteTextures, 1, &_textureId);
        }
        _textureId = 0;
    }
    if (_cvTexture) {
#if TARGET_OS_OSX
        CVOpenGLTextureRelease(_cvTexture);
#else
        CFRelease(_cvTexture);
#endif
        _cvTexture = NULL;
        CVOpenGLESTextureCacheFlush(_cvTextureCache, 0);
        _cvTextureCache = NULL;
    }
    if (_context) {
        CGContextRelease(_context);
        _context = NULL;
    }
}

void OSBitmap::lock(PIXELDATA* pixelData, bool forWriting) {

    // If wanting to write then try to create a Core Video image buffer that will give us direct
    // texture access. Not all pixel formats are supported so this is allowed to fail.
    // NB: This optimization is currently broken on OSX cos of GL_TEXTURE_RECTANGLE
#if TARGET_OS_IOS
    if (forWriting && !_cvImageBuffer) {
        OSType pixelFormat = 0;
        switch (_format) {
            case BITMAPFORMAT_RGBA32: pixelFormat = kCVPixelFormatType_32BGRA; break;
            case BITMAPFORMAT_BGRA32:
#if TARGET_OS_SIMULATOR
            pixelFormat = kCVPixelFormatType_32BGRA; break;
#else
            pixelFormat = kCVPixelFormatType_32BGRA; break;
#endif
            case BITMAPFORMAT_RGB565: pixelFormat = kCVPixelFormatType_16LE565; break;
            case BITMAPFORMAT_A8: pixelFormat = kCVPixelFormatType_OneComponent8; break;
            default: assert(0); break;
        }

        NSDictionary* pixelBufferAttributes = @{
            (id)kCVPixelBufferPixelFormatTypeKey : @(pixelFormat),
#if TARGET_OS_IOS
            (id)kCVPixelFormatOpenGLESCompatibility : @YES,
#else
            (id)kCVPixelFormatOpenGLCompatibility : @YES,
#endif
            (id)kCVPixelBufferIOSurfacePropertiesKey : @{}
        };
        CVReturn err = CVPixelBufferCreate(kCFAllocatorDefault, _width, _height, pixelFormat,
            (__bridge CFDictionaryRef)(pixelBufferAttributes), &_cvImageBuffer);
        assert((_cvImageBuffer && err == kCVReturnSuccess));// || err == kCVReturnInvalidPixelFormat);
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



void OSBitmap::unlock(PIXELDATA* pixelData, bool pixelDataChanged) {
    if (_cvImageBuffer) {
        CVReturn err = CVPixelBufferUnlockBaseAddress(_cvImageBuffer, 0);
        assert(err == kCVReturnSuccess);
    }
    //if (_cvTextureCache) {
    //    CVOpenGLESTextureCacheFlush(_cvTextureCache, 0);
   // }
    if (pixelDataChanged) {
        _needsUpload = true;
    }
}

void OSBitmap::bind() {

    // If we have a core video buffer then create a special core video texture than uses the buffer directly
    if (_cvImageBuffer && !_textureId) {
        CVReturn err;
        if (!_cvTextureCache) {
            _cvTextureCache = getTextureCache(false);
        }
#if TARGET_OS_IOS
        err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
           _cvTextureCache, _cvImageBuffer, NULL, GL_TEXTURE_2D,
                   (_format==4) ? GL_ALPHA : GL_RGBA,
           _width, _height, (_format==4) ? GL_ALPHA :
#if TARGET_OS_SIMULATOR
                                                           GL_RGBA
#else
                                                           GL_BGRA
#endif
                                                           , GL_UNSIGNED_BYTE, 0, &_cvTexture);
#else
        err = CVOpenGLTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                         _cvTextureCache,
                                                         _cvImageBuffer,
                                                         NULL,
                                                         &_cvTexture);
#endif
        assert(_cvTexture && err == kCVReturnSuccess);
        _texTarget = CVOpenGLESTextureGetTarget(_cvTexture);
        assert(_texTarget==GL_TEXTURE_2D); // oh dear, OSX uses GL_TEXTURE_RECTANGLE... needs special shaders
        _textureId = CVOpenGLESTextureGetName(_cvTexture);
    }
    
    Bitmap::bind();

    // If bitmap data changed we may need to update texture data
    if (!_needsUpload) {
        return;
    }
    _needsUpload = false;
    
    
    // Slow path
    if (!_cvImageBuffer) {
        void* data = CGBitmapContextGetData(_context);
        if (!_allocdTexData) {
            _allocdTexData = true;
            check_gl(glTexImage2D, _texTarget, 0, getGlInternalFormat(),
                    _width, _height, 0, getGlFormat(), getGlPixelType(), data);
        } else {
            check_gl(glTexSubImage2D, _texTarget, 0, 0, 0, _width, _height, getGlFormat(), getGlPixelType(),data);
        }
    }
}


Bitmap* oakBitmapCreate(int width, int height, int format) {
    return new OSBitmap(width, height, format, NULL, 0);
}


Bitmap* bitmapFromData(const void* data, int cb) {
    CGDataProviderRef dataProvider = CGDataProviderCreateWithData(NULL, data, cb, NULL);
    CGImageRef fullImage = CGImageCreateWithPNGDataProvider(dataProvider, NULL, NO, kCGRenderingIntentDefault);
    if (!fullImage) {
        fullImage = CGImageCreateWithJPEGDataProvider(dataProvider, NULL, NO, kCGRenderingIntentDefault);
    }
    if (!fullImage) {
        return NULL;
    }
    
    // Parse CGImage info
    int width = (int)CGImageGetWidth(fullImage);
    int height = (int)CGImageGetHeight(fullImage);
    CGBitmapInfo info = CGImageGetBitmapInfo(fullImage); // may be RGBA, BGRA, or ARGB
    CGColorSpaceRef colorspace = CGImageGetColorSpace(fullImage);
    CGColorSpaceModel colormodel = CGColorSpaceGetModel(colorspace);
    size_t bpp = CGImageGetBitsPerPixel(fullImage);
    if (bpp < 8 || bpp > 32 || (colormodel != kCGColorSpaceModelMonochrome && colormodel != kCGColorSpaceModelRGB)) {
        assert(0 && "unsupported CGImage type");
        return NULL;
    }
    
    // Get a pointer to the uncompressed image data.
    // This allows access to the original (possibly unpremultiplied) data, but any manipulation
    // (such as scaling) has to be done manually. Contrast this with drawing the image
    // into a CGBitmapContext, which allows scaling, but always forces premultiplication.
    CGDataProviderRef pro = CGImageGetDataProvider(fullImage);
    CFDataRef data2 = CGDataProviderCopyData(pro);
    assert(data2);
    GLubyte* pixels = (GLubyte *)CFDataGetBytePtr(data2);
    assert(pixels);
    size_t cbUncompressed = CFDataGetLength(data2);
    
    // Conversions from unsupported pixel formats
    int format = BITMAPFORMAT_UNKNOWN;
    if (bpp==16 && colormodel==kCGColorSpaceModelMonochrome) {  // A16 ==> A8
        format = BITMAPFORMAT_A8;
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
                    format = BITMAPFORMAT_BGRA32;
                    break;
                default:
                    format = BITMAPFORMAT_RGBA32;
            }
            break;
        case 24:
            assert(0); // Can OpenGL on iOS support these or do they need conversion? No idea...
            break;
        case 16:
            format = BITMAPFORMAT_RGB565;
            break;
        case 8:
            format = BITMAPFORMAT_A8;
            break;
        default:
            assert(0);
            return NULL;
    }
    
    // Convert ARGB to BGRA (NB: CGImage only produces ARGB on simulator)
    if (format == BITMAPFORMAT_BGRA32) {
        if ((info & kCGBitmapByteOrderMask) != kCGBitmapByteOrder32Host) {
            info = (info&~kCGBitmapByteOrderMask) | kCGBitmapByteOrder32Host;
            uint32_t* p = (uint32_t*)pixels;
            for (int i = 0; i < width * height; i++)
                p[i] = (p[i] << 24) | ((p[i] & 0xFF00) << 8) | ((p[i] >> 8) & 0xFF00) | (p[i] >> 24);
        }
    }

    // Create the native bitmap
    Bitmap* bitmap = new OSBitmap(width, height, format, pixels, (int)cbUncompressed/height);
    CGImageRelease(fullImage);
    return bitmap;
}

void oakBitmapCreateFromData(const void* data, int cb, std::function<void(Bitmap*)> callback) {
    Bitmap* bitmap = bitmapFromData(data, cb);
    dispatch_async(dispatch_get_main_queue(), ^() {
        callback(bitmap);
    });
}


/*
 
 static void printKeys(const void* key, const void* value, void* context) {
 CFShow(key);
 }
 
 CFArrayRef pixelFormatDescriptionsArray = NULL;
 CFIndex i;
 pixelFormatDescriptionsArray =
 CVPixelFormatDescriptionArrayCreateWithAllPixelFormatTypes(kCFAllocatorDefault);
 printf("Core Video Supported Pixel Format Types:\n\n");
 for (i = 0; i < CFArrayGetCount(pixelFormatDescriptionsArray); i++) {
 CFStringRef pixelFormat = NULL;
 CFNumberRef pixelFormatFourCC = (CFNumberRef)CFArrayGetValueAtIndex(pixelFormatDescriptionsArray, i);
 if (pixelFormatFourCC != NULL) {
 UInt32 value;
 CFNumberGetValue(pixelFormatFourCC, kCFNumberSInt32Type, &value);
 if (value <= 0x28) {
 pixelFormat = CFStringCreateWithFormat(kCFAllocatorDefault, NULL,
 CFSTR("Core Video Pixel Format Type: %d\n"), value);
 } else {
 pixelFormat = CFStringCreateWithFormat(kCFAllocatorDefault, NULL,
 CFSTR("Core Video Pixel Format Type (FourCC):%c%c%c%c\n"), (char)(value >> 24), (char)(value >> 16), (char)(value >> 8), (char)value);
 }
 
 CFDictionaryRef d = CVPixelFormatDescriptionCreateWithPixelFormatType(kCFAllocatorDefault, (OSType)value);
 CFDictionaryApplyFunction(d, printKeys, this);
 CFNumberRef glFormat = (CFNumberRef)CFDictionaryGetValue(d, kCVPixelFormatOpenGLFormat);
 CFNumberRef glInternalFormat = (CFNumberRef)CFDictionaryGetValue(d, kCVPixelFormatOpenGLInternalFormat);
 CFNumberRef glType = (CFNumberRef)CFDictionaryGetValue(d, kCVPixelFormatOpenGLType);
 UInt32 glFormatVal, glInternalFormatVal, glTypeVal;
 if (glFormat) {
 CFNumberGetValue(glFormat, kCFNumberSInt32Type, &glFormatVal);
 CFNumberGetValue(glInternalFormat, kCFNumberSInt32Type, &glInternalFormatVal);
 CFNumberGetValue(glType, kCFNumberSInt32Type, &glTypeVal);
 app.log("gl %d %d %d", glFormat, glInternalFormat, glType);
 }
 
 CFShow(pixelFormat);
 CFRelease(pixelFormat);
 }
 }
 

 */

#endif
