//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class AudioResampler : public Worker {
public:
    
    AudioResampler();
    
    // API
    void open(const AudioFormat& formatIn, const AudioFormat& formatOut);
    void process(AudioSamples* samples);
    void close();    
    std::function<void (AudioSamples* samples)> onNewAudioSamples;

protected:
    AudioFormat _formatIn, _formatOut;
    
};
