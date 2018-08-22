//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

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

class Bitmap : public BitmapBase {
public:
    CGContextRef _context; // nil if bitmap is texture-only
    CVImageBufferRef _cvImageBuffer;
    CVOpenGLESTextureRef _cvTexture;
    CVOpenGLESTextureCacheRef _cvTextureCache;
    CFDataRef _cfData;
    
    Bitmap();
    Bitmap(int width, int height, int format);
    Bitmap(int width, int height, int format, void* pixels, int stride);
    Bitmap(CVPixelBufferRef cvImageBuffer, bool fromCamera);
    ~Bitmap();
    
    // Overrides
    void lock(PIXELDATA* pixelData, bool forWriting) override;
    void unlock(PIXELDATA* pixelData, bool pixelDataChanged) override;
    void bind() override;
    
    void fromVariant(const Variant& v) override;
    void toVariant(Variant& v) override;
    
private:
    void createBitmapContext();

};


