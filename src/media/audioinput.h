//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

struct AudioFormat {
    enum SampleType {
        Int16,
        Float32
    } sampleType;
    int numChannels;
    int sampleRate;
};

class AudioInputSamples : public Object {
public:
    AudioFormat _format;
    
    virtual bytearray getData()=0;
};

class AudioInput : public Object {
public:
    
    // API
    static AudioInput* create();
    std::function<void (AudioInputSamples* samples)> onNewAudioSamples;
    
    virtual void open(AudioFormat& preferredFormat)=0;
    virtual void start()=0;
    virtual void stop()=0;
    virtual void close()=0;

protected:
    AudioInput();

};
