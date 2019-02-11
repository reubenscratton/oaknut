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
#define CVOpenGLESTextureGetTarget CVOpenGLTextureGetTarget
#define CVOpenGLESTextureGetName CVOpenGLTextureGetName
#define CVOpenGLESTextureCacheFlush CVOpenGLTextureCacheFlush
#define CVOpenGLESTextureCacheCreate CVOpenGLTextureCacheCreate
#endif


class CoreVideoTexture : public GLTexture {
public:

    CVOpenGLESTextureRef _cvTexture;
    CVOpenGLESTextureCacheRef _cvTextureCache;
    
    CoreVideoTexture(BitmapApple* bitmap, GLRenderer* renderer, CVOpenGLESTextureCacheRef cvTextureCache) : GLTexture(renderer, bitmap->_format) {
        _cvTextureCache = cvTextureCache;

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
        _texTarget = CVOpenGLESTextureGetTarget(_cvTexture); // GL_TEXTURE_RECTANGLE
        _type = Rect;
        _denormalizedCoords = true;
#endif

    }
    ~CoreVideoTexture() {
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
    /*
    void bind() override {
        
        if (!_textureId) {
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
            _textureId = CVOpenGLESTextureGetName(_cvTexture);
        }
        
        GLTexture::bind();
    }*/
    
    void upload() override {
        if (!_cvTexture) { // CoreVideo textures are direct access, no upload needed
            GLTexture::upload();
        }
    }

    /*void unload() override {
        GLTexture::unload();
        if (_cvTexture) {
#if TARGET_OS_OSX
            CVOpenGLTextureRelease(_cvTexture);
#else
            CFRelease(_cvTexture);
#endif
            CVOpenGLESTextureCacheFlush(_cvTextureCache, 0);
            _cvTexture = NULL;
            _cvTextureCache = NULL;
        }
    }*/
    


};



class RendererApple : public GLRenderer {
public:
    CVOpenGLESTextureCacheRef _cvTextureCache;


    RendererApple(Window* window) : GLRenderer(window) {
    }

    void bindToNativeWindow(long nativeWindowHandle) override {
        //NativeView* nativeView = (__bridge NativeView*)(void*)nativeWindowHandle;
        // todo: move Apple-specific EAGL setup code here
    }
    void commit() override {
        // todo: move Apple-specific swapBuffer stuff here
    }
    
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
            bitmap->_texture = new CoreVideoTexture(bitmap, this, _cvTextureCache);
        } else {
            Renderer::createTextureForBitmap(abitmap);
        }
    }


};

Renderer* Renderer::create(Window* window) {
    return new RendererApple(window);
}






#endif
#endif
