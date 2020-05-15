//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>

class WebGLTexture : public Texture {
public:
    val _tex;
    int _textureId;
    
    WebGLTexture(val tex) : Texture(NULL, PIXELFORMAT_RGBA32), _tex(tex) {
        _textureId = tex["name"].as<int>();
    }
    void resize(int width, int height) {
        assert(0);
    }

};

class CameraWebBitmap : public BitmapWeb {
public:
    
    CameraWebBitmap(val texture) {
        _texture = new WebGLTexture(texture);
        _format = PIXELFORMAT_RGBA32;
    }

    
    /*void bind() override {
        val::global("gl").call<void>("bindTexture", GL_TEXTURE_2D, _texture);
    }*/
    
    void lock(PIXELDATA* pixelData, bool forWriting) override {
        
        int foo = EM_ASM_INT({
            var width = $0;
            var height = $1;
            var texture = gotGet($2);
            
            // Create a C++ buffer to hold the pixel data
            var cb = width*height*4;
            var ptr = _malloc(cb);
            var pixels = new Uint8Array(HEAPU8.buffer, ptr, cb);
            
            // Read the pixels from the texture
            var oldFb = gl.getParameter(gl.FRAMEBUFFER_BINDING);
            var fb = gl.createFramebuffer();
            gl.bindFramebuffer(gl.FRAMEBUFFER, fb);
            gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, texture, 0);
            gl.readPixels(0, 0, width, height, gl.RGBA, gl.UNSIGNED_BYTE, pixels);
            gl.bindFramebuffer(gl.FRAMEBUFFER, oldFb);
            gl.deleteFramebuffer(fb);
            
            return ptr;
        }, _width, _height, val::global("gotSet")(_texture).as<int>());

        pixelData->data = (uint8_t*)foo;
        pixelData->stride = _width*4;
        pixelData->cb = _width*4*_height;
    }
    void unlock(PIXELDATA* pixelData, bool changed) override {
        free(pixelData->data);
    }
    
    /*
    bytearray toJpeg(float quality) override {
        
        int gotIndex_tuple = EM_ASM_INT({
            var width = $0;
            var height = $1;
            var texture = gotGet($2);
            var quality = $3;
            
            // Create a buffer to hold the pixel data
            var cb = width*height*4;
            var pixels = new Uint8Array(cb);
            
            // Read the pixels from the texture
            var oldFb = gl.getParameter(gl.FRAMEBUFFER_BINDING);
            var fb = gl.createFramebuffer();
            gl.bindFramebuffer(gl.FRAMEBUFFER, fb);
            gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, texture, 0);
            gl.readPixels(0, 0, width, height, gl.RGBA, gl.UNSIGNED_BYTE, pixels);
            gl.bindFramebuffer(gl.FRAMEBUFFER, oldFb);
            gl.deleteFramebuffer(fb);

            // Create an ImageData around the pixel data without doing an expensive copy
            var imageData = new ImageData(new Uint8ClampedArray(pixels.buffer, 0, cb), width, height);

            // Put the ImageData into the canvas
            var canvas = document.createElement('canvas');
            canvas.width = width;
            canvas.height = height;
            var ctx = canvas.getContext('2d');
            ctx.putImageData(imageData, 0, 0);
 
            // Convert to JPEG using the absolutely hideous synchronous text-based browser API
            var dataURI = canvas.toDataURL('image/jpeg', quality); // Ugh, binary data in a string. Why??
            var jpegstr = atob(dataURI.split(',')[1]); // Double ugh. atob also returns a string rather than a byte array.
            
            // Malloc and copy the bytes to the C++ heap. It really sucks that we have to iterate a huge loop.
            var ptr = _malloc(jpegstr.length);
            for(var i = 0; i < jpegstr.length; i++) {
                HEAPU8[ptr+i] = jpegstr.charCodeAt(i);
            }
            
            // Return a tuple containing the C++ heap alloc info.
            var tuple = new Object();
            tuple.data = ptr;
            tuple.cb = jpegstr.length;
            return gotSet(tuple);

        }, _width, _height, val::global("gotSet")(_texture).as<int>(), quality);

        // Take ownership of the C++ allocation made by Javascript code
        val tuple = val::global("gotGet")(gotIndex_tuple);
        bytearray jpeg_bytes;
        jpeg_bytes.assignNoCopy((uint8_t*)tuple["data"].as<int>(), tuple["cb"].as<int>());
        
        return jpeg_bytes;
    }*/

};



class CameraFrameWeb : public CameraFrame {
public:
    CameraFrameWeb(CameraWebBitmap* bitmap) : _bitmap(bitmap) {
    }
    
    Bitmap* asBitmap() override {
        return _bitmap;
    }
    sp<CameraWebBitmap> _bitmap;
};


static void s_timerCallbackCPP(CameraWeb* camera, int textureGotId) {
    camera->timerCallbackCPP(textureGotId);
}
static void s_callbackStoreIntervalId(CameraWeb* camera, int intervalId) {
    camera->_intervalId = intervalId;
}


CameraWeb::CameraWeb(const Options& options) : Camera(options), _video(val::null()), _intervalId(-1) {
}
CameraWeb::~CameraWeb() {
}

void CameraWeb::open() {
    _video = val::global("document").call<val>("createElement", val("video"));
}

//video.autoplay = true;

void CameraWeb::start() {
    
    EM_ASM_({
        var camera=$0;
        var video = gotGet($1);
        var callback = $2;
        var callbackStoreIntervalId = $3;
        var opt_frontFacing = $4;
        var opt_frameSizeShort= $5;
        var opt_frameSizeLong = $6;
        var opt_frameRate = $7;
        if (opt_frameRate<=0) {
            opt_frameRate = 30;
        }
        video.setAttribute("autoplay", true);
        video.setAttribute("playsinline", true);
        navigator.mediaDevices.getUserMedia({video: {
            facingMode: opt_frontFacing ? "user" : "environment"
        }, audio: true}).then(function(stream) {
            video.onloadedmetadata = function(e) {
                var intervalId = setInterval(function() {
                    
                    // Create new texture
                    var texture = gl.createTexture();
                    var id = GL.getNewId(GL.textures);
                    texture.name = id;
                    GL.textures[id] = texture;
                    gl.bindTexture(gl.TEXTURE_2D, texture);
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
                    
                    // Set the texture data to be the current video frame (i.e. magic)
                    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, video);
                    dynCall('vii', callback, [camera, gotSet(texture)]);
                }, 1000/opt_frameRate);
                dynCall('vii', callbackStoreIntervalId, [camera, intervalId]);
            };
            if ("srcObject" in video) {
                video.srcObject = stream;
            } else {
                video.src = window.URL.createObjectURL(stream);
            }
        }).catch(function(e) {
            log_error("Failed to open webcam: " + e);
        });
    }, this, val::global("gotSet")(_video).as<int>(), s_timerCallbackCPP, s_callbackStoreIntervalId,
            _options.frontFacing, _options.frameSizeShort, _options.frameSizeLong, _options.frameRate);
    
}


void CameraWeb::timerCallbackCPP(int textureGotId) {
    val texture = val::global("gotGet")(textureGotId);
    CameraWebBitmap* bitmap = new CameraWebBitmap(texture);
    bitmap->_width =  _video["videoWidth"].as<int>();
    bitmap->_height = _video["videoHeight"].as<int>();
    CameraFrameWeb frame(bitmap);
    onNewCameraFrame(&frame);
}


// Mute the audio track to prevent feedback hell
// THIS PAUSES WHOLE VIDEO ON SAFARI WTF APPLE AAAARGH!
/*if (window.chrome) {
 var audioTracks = stream.getAudioTracks();
 audioTracks[0].enabled = false;
 }*/


void CameraWeb::stop() {
    if (!_started) {
        return;
    }
    _started = false;
    if (_intervalId >= 0) {
        EM_ASM({
            clearInterval($0);
        }, _intervalId);
        _intervalId = -1;
    }
    _video.call<void>("pause");
}

void CameraWeb::close() {
    _video = val::null();
}


Camera* Camera::create(const Options& options) {
    return new CameraWeb(options);
}


#endif
