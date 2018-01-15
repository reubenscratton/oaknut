//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "oaknut.h"
#include <CoreGraphics/CoreGraphics.h>
#include <CoreVideo/CoreVideo.h>

#if TARGET_OS_IOS
#define GLGetCurrentContext() [EAGLContext currentContext]
#else
#define GLGetCurrentContext() CGLGetCurrentContext()
#define CVOpenGLESTextureRef CVOpenGLTextureRef
#define CVOpenGLESTextureCacheRef CVOpenGLTextureCacheRef
#define CVOpenGLESTextureGetTarget CVOpenGLTextureGetTarget
#define CVOpenGLESTextureGetName CVOpenGLTextureGetName
#define CVOpenGLESTextureCacheFlush CVOpenGLTextureCacheFlush
#define CVOpenGLESTextureCacheCreate CVOpenGLTextureCacheCreate
#endif

class OSBitmap : public Bitmap {
public:
    CGContextRef _context; // nil if bitmap is texture-only
    CVImageBufferRef _cvImageBuffer; 
    CVOpenGLESTextureRef _cvTexture;
    CVOpenGLESTextureCacheRef _cvTextureCache;
    bool _needsUpload;

    OSBitmap(int width, int height, int format, void* pixels, int stride);
    OSBitmap(CVPixelBufferRef cvImageBuffer, bool fromCamera);
    ~OSBitmap();
    
    // Overrides
    virtual void lock(PIXELDATA* pixelData, bool forWriting);
    virtual void unlock(PIXELDATA* pixelData, bool pixelDataChanged);
    virtual void bind();
};

