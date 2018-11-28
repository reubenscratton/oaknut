//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


AudioInput::AudioInput() {
}


AudioSamples::AudioSamples() {
}
AudioSamples::AudioSamples(const bytearray& data) : _data(data) {
}

bytearray AudioSamples::getData() {
    return _data;
}

vector<float> AudioSamples::getDataFloat32() {
    bytearray data = getData();
    vector<float> vec;
    
    if (_format.sampleType == AudioFormat::Float32) {
        float* floats = (float*)data.data();
        int numSamples = data.size()>>2;
        vec.resize(numSamples);
        for (int i=0 ; i<numSamples ; i++) {
            vec[i] = floats[i];
        }
    }
    else if (_format.sampleType == AudioFormat::Int16) {
        int16_t* shorts = (int16_t*)data.data();
        int numSamples = data.size()>>1;
        vec.resize(numSamples);
        for (int i=0 ; i<numSamples ; i++) {
            vec[i] = shorts[i] / 32767.0f;
        }
    }
    return vec;
}
