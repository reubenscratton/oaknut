//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#ifdef OAKNUT_WANT_CAMERA

class Camera : public Object {
public:
    
    // API
    static bool hasCamera(int cameraId);
    static Camera* create(int cameraId);
    std::function<void ()> onNewCameraFrame;
    int _previewWidth;
    int _previewHeight;

    // Most-recently captured frame info
    GLuint _frameTextureId;
    long _frameTimestamp;
    int _frameWidth;
    int _frameHeight;
    float _frameTransform[16];

    virtual void open()=0;
    virtual void start()=0;
    virtual void stop()=0;
    virtual void close()=0;

    virtual Bitmap* lastFrameAsBitmap()=0;

protected:
    Camera();


};

#endif

