//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>

class CameraWebBitmap : public BitmapWeb {
public:
    
    CameraWebBitmap(val texture) : _texture(texture)  {
        _format = BITMAPFORMAT_RGBA32;
        _textureId = _texture["name"].as<int>();
    }

    
    void bind() override {
        val::global("gl").call<void>("bindTexture", GL_TEXTURE_2D, _texture);
    }
    
    bytearray toJpeg(float quality) override {
        
        int gotIndex_tuple = EM_ASM_INT({
            var width = $0;
            var height = $1;
            var texture = gotGet($2);
            
            // Create a buffer to hold the pixel data
            var cb = width*height*4;
            var pixels = new Uint8Array(cb);
            
            // Read the pixels from the texture
            var oldFb = gl.getParameter(gl.FRAMEBUFFER_BINDING);
            var fb = gl.createFramebuffer();
            gl.bindFramebuffer(gl.FRAMEBUFFER, fb);
            gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, texture, 0);
            gl.readPixels(0, 0, $0, $1, gl.RGBA, gl.UNSIGNED_BYTE, pixels);
            gl.bindFramebuffer(gl.FRAMEBUFFER, oldFb);
            gl.deleteFramebuffer(fb);

            // Create an ImageData around the pixel data without doing an expensive copy
            var imageData = new ImageData(new Uint8ClampedArray(pixels.buffer, 0, cb), width, height);

            // Put the ImageData into the canvas
            var canvas = document.createElement('canvas');
            var ctx = canvas.getContext('2d');
            ctx.putImageData(imageData, 0, 0);
 
            // Convert to JPEG using the absolutely hideous synchronous text-addled browser API
            var dataURI = canvas.toDataURL('image/jpeg', quality);
            var jpegstr = atob(dataURI.split(',')[1]); // returns a String! not a byte array
            
            // Malloc and copy the bytes to the C++ heap
            var ptr = _malloc(jpegstr.length);
            for(var i = 0; i < jpegstr.length; i++) {
                HEAPU8[ptr+i] = jpegstr.charCodeAt(i);
            }
            
            // Return a tuple containing the C++ heap alloc info
            var tuple = new Object();
            tuple.data = ptr;
            tuple.cb = jpegstr.length;
            return gotSet(tuple);

        }, _width, _height, val::global("gotSet")(_texture).as<int>());

        // Take ownership of the C++ allocation made by Javascript code
        val tuple = val::global("gotGet")(gotIndex_tuple);
        bytearray jpeg_bytes;
        jpeg_bytes.assignNoCopy((uint8_t*)tuple["data"].as<int>(), tuple["cb"].as<int>());
        
        return jpeg_bytes;
    }

    val _texture;
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


static void OnCameraWebUpdate(CameraWeb* webcam, int textureGotId, int width, int height) {
    val texture = val::global("gotGet")(textureGotId);
    CameraWebBitmap* bitmap = new CameraWebBitmap(texture);
    bitmap->_width = width;
    bitmap->_height = height;
    CameraFrameWeb frame(bitmap);
    webcam->onNewCameraFrame(&frame);
}



CameraWeb::CameraWeb(const Options& options) : Camera(options) {
}
CameraWeb::~CameraWeb() {
}

void CameraWeb::open() {
    
}
void CameraWeb::start() {
    EM_ASM_({
        var webcam=$0;
        var onUpdate=$1;

        var video = document.createElement('video');
        video.autoplay = true;

        
        function timerCallback() {
            if (video.paused || video.ended) {
                return;
            }
            
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
            Runtime.dynCall('viiii', onUpdate, [webcam, gotSet(texture), video.videoWidth, video.videoHeight]);
            setTimeout(timerCallback, 30);
        }
        
        navigator.mediaDevices.getUserMedia({video: {
                width: { ideal: 320 },
                height: { ideal: 240 },
                facingMode: "user"
        }, audio: true}).then(function(stream) {
            
            video.onloadedmetadata = function(e) {
                timerCallback();
            };

            // Mute the audio track
            var audioTracks = stream.getAudioTracks();
            audioTracks[0].enabled = false;
            
            if ("srcObject" in video) {
                video.srcObject = stream;
            } else {
                video.src = window.URL.createObjectURL(stream);
            }
            
        }).catch(function(e) {
            log("Failed to open webcam: " + e);
        });

    }, this, OnCameraWebUpdate);
}
void CameraWeb::stop() {
    
}
void CameraWeb::close() {
    
}


Camera* Camera::create(const Options& options) {
    return new CameraWeb(options);
}


#endif
