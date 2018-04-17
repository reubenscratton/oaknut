//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB && OAKNUT_WANT_CAMERA

#include <oaknut.h>


class WebCamera : public Bitmap  {
public:
    WebCamera(int cameraId);
    ~WebCamera();
    
    int _cameraId;
    CameraPreviewDelegate _delegate;
    val _texture;
    
    void startPreview(CameraPreviewDelegate delegate);
    void stopPreview();

    static void OnUpdate(WebCamera* webcam) {
        webcam->_delegate(webcam, 5);
    }

    void bind() {
        val::global("gl").call<void>("bindTexture", GL_TEXTURE_2D, _texture);
    }

};


WebCamera::WebCamera(int cameraId) : _texture(val::null()) {
    _cameraId = cameraId;
    _width = 640;
    _height = 480;
    _format = BITMAPFORMAT_RGBA32;
}
WebCamera::~WebCamera() {
}

void WebCamera::startPreview(CameraPreviewDelegate delegate) {
    _delegate = delegate;
    val gl = val::global("gl");
    _texture = gl.call<val>("createTexture");
    val got = val::global("GlobalObjectTracker");
    got.set(2, _texture);
    EM_ASM_({
        var webcam=$0;
        var onUpdate=$1;
        var texture = GlobalObjectTracker[2];

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
            Runtime.dynCall('vi', onUpdate, [webcam]);
            setTimeout(timerCallback, 30);
        }
        
        //var getUserMedia = navigator.mediaDevices.getUserMedia || navigator.mediaDevices.webkitGetUserMedia || navigator.mediaDevices.mozGetUserMedia;
        navigator.mediaDevices.getUserMedia({video: {
                width: { ideal: 640 },
                height: { ideal: 480 },
                facingMode: "user"
        }, audio: true}).then(function(stream) {
            if ("srcObject" in video) {
                video.srcObject = stream;
            } else {
                video.src = window.URL.createObjectURL(stream);
            }
            //video.onloadedmetadata = function(e) {log("onloadedmetadata"); timerCallback(); };
            setTimeout(timerCallback, 30);
        }).catch(function(e) {
            log("Failed to open webcam: " + e);
        });

    }, this, OnUpdate);
}
void WebCamera::stopPreview() {
    
}


void* oakCameraOpen(int cameraId) {
    return new WebCamera(cameraId);
}
void oakCameraPreviewStart(void* oscamera, CameraPreviewDelegate delegate) {
    WebCamera* webcam = (WebCamera*)oscamera;
    webcam->startPreview(delegate);
}
void oakCameraPreviewStop(void* oscamera) {
    WebCamera* webcam = (WebCamera*)oscamera;
    webcam->stopPreview();
}
void oakCameraClose(void* oscamera) {
    WebCamera* webcam = (WebCamera*)oscamera;
    delete webcam;
}


#endif
