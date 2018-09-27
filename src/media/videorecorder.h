//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class VideoRecorder : public Object {
public:
    
    // API
    static VideoRecorder* create();
    virtual void start(SIZE size, int frameRate, int keyframeRate, int audioSampleRate)=0;
    virtual void handleNewCameraFrame(CameraFrame* frame)=0;
    virtual void handleNewAudioSamples(AudioInputSamples* audioSamples)=0;
    virtual void stop()=0;
    
protected:
    VideoRecorder();
    
};
