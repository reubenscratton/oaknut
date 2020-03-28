//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE
#include <oaknut.h>
#if RENDERER_GL

#if TARGET_OS_IOS
#define GLGetCurrentContext() [EAGLContext currentContext]
#else
#define GLGetCurrentContext() CGLGetCurrentContext()
#define CVOpenGLESTextureRef CVOpenGLTextureRef
#define CVOpenGLESTextureCacheRef CVOpenGLTextureCacheRef
#define CVOpenGLESTextureGetName CVOpenGLTextureGetName
#define CVOpenGLESTextureCacheFlush CVOpenGLTextureCacheFlush
#define CVOpenGLESTextureCacheCreate CVOpenGLTextureCacheCreate
#endif


class GLTextureCoreVideo : public GLTexture {
public:

    CVOpenGLESTextureRef _cvTexture;
    CVOpenGLESTextureCacheRef _cvTextureCache;
    
    GLTextureCoreVideo(BitmapApple* bitmap, GLRenderer* renderer, CVOpenGLESTextureCacheRef cvTextureCache) : GLTexture(renderer, bitmap->_format) {
        _cvTextureCache = cvTextureCache;
        _size.width = bitmap->_width;
        _size.height = bitmap->_height;
        _usesSharedMem = true;
        _isNativeTextureValid = true;
        
#if TARGET_OS_IOS
        CVReturn err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, _cvTextureCache, bitmap->_cvImageBuffer, NULL, GL_TEXTURE_2D, GL_RGBA , bitmap->_width, bitmap->_height, GL_BGRA, GL_UNSIGNED_BYTE, 0, &_cvTexture);
        assert(err==0);
        _textureId = CVOpenGLESTextureGetName(_cvTexture);
        _texTarget = CVOpenGLESTextureGetTarget(_cvTexture);
#else
        CVReturn err = CVOpenGLTextureCacheCreateTextureFromImage(NULL, _cvTextureCache,
                                                                  bitmap->_cvImageBuffer, NULL, &_cvTexture);
        assert(err==0);
        _textureId = CVOpenGLTextureGetName(_cvTexture);
        _texTarget = CVOpenGLTextureGetTarget(_cvTexture); // GL_TEXTURE_RECTANGLE
        _type = Rect;
        _denormalizedCoords = true;
#endif

    }
    ~GLTextureCoreVideo() {
        if (_cvTexture) {
            _textureId = 0;
            auto t = _cvTexture;
            auto tc = _cvTextureCache;
            _cvTexture = NULL;
            _cvTextureCache = NULL;
            CFRelease(t);
            CVOpenGLESTextureCacheFlush(tc, 0);
        }
    }
    
};


class GLSurfaceApple : public GLSurface {
public:
    
    GLSurfaceApple(Renderer* r, bool isPrivate) : GLSurface(r, isPrivate) {
    }
    //NativeView* nativeView = (__bridge NativeView*)(void*)nativeWindowHandle;
    // todo: move Apple-specific EAGL setup code here

    void bindToNativeWindow(long nativeWindowHandle) override {
        //assert(0); // todo: move the gl setup here
        /*
        NativeView* nativeView = (__bridge NativeView*)(void*)nativeWindowHandle;
        _metalLayer = (CAMetalLayer*)nativeView.layer;
        _metalLayer.opaque = YES;
        _metalLayer.device = s_device;
        _metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        _metalLayer.framebufferOnly = false;
        _metalLayer.contentsScale = app->_defaultDisplay->_scale;// [NSScreen mainScreen].backingScaleFactor;
        _metalLayer.drawsAsynchronously = YES;
#if PLATFORM_MACOS
        _metalLayer.displaySyncEnabled = 1;
        nativeView.wantsLayer = YES;
#endif
         */
    }
};


class GLRendererApple : public GLRenderer {
public:
    CVOpenGLESTextureCacheRef _cvTextureCache;

    Surface* createSurface(bool isPrivate) override {
        return new GLSurfaceApple(this, isPrivate);
    }

//    void commit() override {
        // todo: move Apple-specific swapBuffer stuff here
//    }
    
    void createTextureForBitmap(Bitmap* abitmap) override {
        BitmapApple* bitmap = (BitmapApple*)abitmap;
        if (bitmap->_cvImageBuffer) {
            if (!_cvTextureCache) {
                CVReturn err = CVOpenGLESTextureCacheCreate(NULL, NULL, GLGetCurrentContext(),
#if !TARGET_OS_IOS
                                                        CGLGetPixelFormat(CGLGetCurrentContext()),
#endif
                                                        NULL,
                                                        &_cvTextureCache);
                assert(err==0);
                CFRetain(_cvTextureCache);
            }
            bitmap->_texture = new GLTextureCoreVideo(bitmap, this, _cvTextureCache);
        } else {
            Renderer::createTextureForBitmap(abitmap);
        }
    }


};

Renderer* Renderer::create() {
    return new GLRendererApple();
}






#endif
#endif
