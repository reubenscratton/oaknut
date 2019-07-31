//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


#include <oaknut.h>



AudioResampler::AudioResampler() : Worker("AudioResamplerWorker") {
    
}
void AudioResampler::open(const AudioFormat& formatIn, const AudioFormat& formatOut) {
    _formatIn = formatIn;
    _formatOut = formatOut;
    variant config;
    config.set("formatIn", formatIn.toVariant());
    config.set("formatOut", formatOut.toVariant());
    start(config);
}

void AudioResampler::process(AudioSamples* samples) {
    Worker::process(samples->getData(), [=](const variant& outdata) {
        sp<AudioSamples> outSamples = new AudioSamples(outdata.bytearrayRef());
        onNewAudioSamples(outSamples);
    });
}


