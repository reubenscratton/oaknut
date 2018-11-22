//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

struct AudioFormat {
    enum SampleType {
        Int16=0,
        Float32=1
    } sampleType;
    int numChannels;
    int sampleRate;
    
    void fromVariant(const variant& v) {
        numChannels=v.intVal("numChannels");
        sampleRate=v.intVal("sampleRate");
        sampleType=(SampleType)v.intVal("sampleType");
    }
    variant toVariant() const {
        variant v;
        v.set("numChannels", numChannels);
        v.set("sampleRate", sampleRate);
        v.set("sampleType", sampleType);
        return v;
    }
};

class AudioSamples : public Object {
public:
    AudioFormat _format;
    
    virtual bytearray getData()=0;
};

class AudioInput : public Object {
public:
    
    // API
    static AudioInput* create();
    std::function<void (AudioSamples* samples)> onNewAudioSamples;
    
    virtual void open(AudioFormat& preferredFormat)=0;
    virtual void start()=0;
    virtual void stop()=0;
    virtual void close()=0;

protected:
    AudioInput();

};
