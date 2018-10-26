//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_LINUX

#include <oaknut.h>


Bitmap* CameraFrameLinux::asBitmap() {
    return NULL;
}



class CameraLinux : public Camera {
public:

    CameraLinux(const Options& options) : Camera(options) {
    }
    
    void open() override {
    }
    
    void start() override {
        
    }
    

    void stop() override {
    }
    
    void close() override {
    }

};

Camera* Camera::create(const Options& options) {
    return new CameraLinux(options);
}




#endif
