//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#pragma once
#if PLATFORM_APPLE
#import <AVFoundation/AVFoundation.h>
#import <AVFoundation/AVCaptureDevice.h>
#import <AVFoundation/AVCaptureOutput.h>

class AudioSamplesApple : public AudioSamples {
public:
    AudioSamplesApple(CMSampleBufferRef samples);
    ~AudioSamplesApple();
    
    bytearray getData() override;
    

    CMSampleBufferRef _sampleBuffer;
};

#endif
