//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE && OAKNUT_WANT_CAMERA

#pragma once
//#import <CoreImage/CoreImage.h>
#include <CoreMedia/CoreMedia.h>

class CameraFrameApple : public CameraFrame {
public:
    CMSampleBufferRef sampleBuffer;
    
    virtual Bitmap* asBitmap() override;
};

#endif
