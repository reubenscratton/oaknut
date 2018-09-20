//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class Camera : public Object {
public:
    
    // API
    static bool hasCamera(int cameraId);
    static Camera* create(int cameraId);
    std::function<void (Bitmap* cameraFrame, float brightness)> onNewCameraFrame;
    
    virtual void open()=0;
    virtual void start()=0;
    virtual void stop()=0;
    virtual void close()=0;
    
protected:
    Camera();
    
};

