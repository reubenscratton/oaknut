//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB && OAKNUT_WANT_CAMERA


class CameraWeb : public Camera  {
public:
    CameraWeb(const Options& options);
    ~CameraWeb();
    
    void start() override;
    void stop() override;
    
    class CameraWebBitmap : public Bitmap {
        
        CameraWebBitmap();

        void create();
        void bind() override;
        
        val _texture;
    };
    
    
    CameraWebBitmap _bitmap;
};

#endif
