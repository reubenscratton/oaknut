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
    
    void open() override;
    void start() override;
    void stop() override;
    void close() override;
    
    class CameraWebBitmap : public Bitmap {
    public:
        
        CameraWebBitmap();

        void create();
        void bind() override;
        
        val _texture;
    };
    
    
    CameraWebBitmap _bitmap;
};

#endif
