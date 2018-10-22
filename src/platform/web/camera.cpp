//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>

class CameraFrameWeb : public CameraFrame {
public:
    CameraFrameWeb(CameraWeb* cam) : _cam(cam) {
    }
    
    Bitmap* asBitmap() override {
        return &_cam->_bitmap;
    }

    CameraWeb* _cam;
};

CameraWeb::CameraWebBitmap::CameraWebBitmap() : _texture(val::null())  {
    _width = 1;
    _height = 1;
    _format = BITMAPFORMAT_RGBA32;
}
void CameraWeb::CameraWebBitmap::bind() {
    val::global("gl").call<void>("bindTexture", GL_TEXTURE_2D, _texture);
}

void CameraWeb::CameraWebBitmap::create() {
    int gotIndex = EM_ASM_INT({
        var tex = gl.createTexture();
        var id = GL.getNewId(GL.textures);
        tex.name = id;
        GL.textures[id] = tex;
        return gotSet(tex);
    });
    _texture = val::global("gotGet")(gotIndex);
    _textureId = _texture["name"].as<int>();
}

static void OnCameraWebUpdate(CameraWeb* webcam, int width, int height) {
    webcam->_bitmap._width = width;
    webcam->_bitmap._height = height;
    CameraFrameWeb frame(webcam);
    webcam->onNewCameraFrame(&frame);
}



CameraWeb::CameraWeb(const Options& options) : Camera(options) {
}
CameraWeb::~CameraWeb() {
}

void CameraWeb::open() {
    
}
void CameraWeb::start() {
    _bitmap.create();
    int gotIndex = val::global("gotSet")(_bitmap._texture).as<int>();
    EM_ASM_({
        var webcam=$0;
        var onUpdate=$1;
        var texture = gotGet($2);

        var video = document.createElement('video');
        video.autoplay = true;
        
        // Create a texture for the camera frames
        gl.bindTexture(gl.TEXTURE_2D, texture);
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, new Uint8Array([0, 0, 255, 255]));
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        
        function timerCallback() {
            if (video.paused || video.ended) {
                return;
            }
            gl.bindTexture(gl.TEXTURE_2D, texture);
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, video);
            Runtime.dynCall('viii', onUpdate, [webcam, video.videoWidth, video.videoHeight]);
            setTimeout(timerCallback, 30);
        }
        
        //var getUserMedia = navigator.mediaDevices.getUserMedia || navigator.mediaDevices.webkitGetUserMedia || navigator.mediaDevices.mozGetUserMedia;
        navigator.mediaDevices.getUserMedia({video: {
                width: { ideal: 320 },
                height: { ideal: 240 },
                facingMode: "user"
        }, audio: true}).then(function(stream) {
            video.onloadedmetadata = function(e) {log("onloadedmetadata"); timerCallback(); };
            if ("srcObject" in video) {
                video.srcObject = stream;
            } else {
                video.src = window.URL.createObjectURL(stream);
            }
        }).catch(function(e) {
            log("Failed to open webcam: " + e);
        });

    }, this, OnCameraWebUpdate, gotIndex);
}
void CameraWeb::stop() {
    
}
void CameraWeb::close() {
    
}


Camera* Camera::create(const Options& options) {
    return new CameraWeb(options);
}


#endif
