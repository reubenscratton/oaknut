//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if OAKNUT_WANT_AUDIO_INPUT

class AudioInput : public Object {
public:
    
    // NB: Samples are always int16_t, only mono supported

    // API
    static AudioInput* create();
    int sampleRate;
    std::function<int (int numSamples, int16_t* samples)> onNewAudioSamples;
    
    virtual void open()=0;
    virtual void start()=0;
    virtual void stop()=0;
    virtual void close()=0;

protected:
    AudioInput();

};

#endif
