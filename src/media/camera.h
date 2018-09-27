//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#ifdef OAKNUT_WANT_CAMERA

class CameraFrame {
public:
    GLuint _textureId; // todo: this is android-specific?
    long _timestamp;
    int _width;
    int _height;
    float _transform[16];

    virtual Bitmap* asBitmap()=0;
};

class Camera : public Object {
public:
    
    // API
    static bool hasCamera(int cameraId);
    static Camera* create(int cameraId);
    std::function<void (CameraFrame* frame)> onNewCameraFrame;
    int _previewWidth;
    int _previewHeight;

    virtual void open()=0;
    virtual void start()=0;
    virtual void stop()=0;
    virtual void close()=0;

protected:
    Camera();


};

#endif

