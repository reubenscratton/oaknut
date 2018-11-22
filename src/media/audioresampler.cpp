//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


#include <oaknut.h>
#include "../../thirdparty/smarc/smarc.h"


class AudioSamplesDumma : public AudioSamples {
public:
    AudioSamplesDumma(const bytearray& data) : _data(data) {
    }
    
    bytearray getData() {
        return _data;
    }
    
    bytearray _data;
};

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
        sp<AudioSamplesDumma> outSamples = new AudioSamplesDumma(outdata.bytearrayVal());
        onNewAudioSamples(outSamples);
    });
}



#define BANDWIDTH 0.95
#define RP 0.1
#define RS 140
#define TOL 0.000001

class AudioResamplerWorker : public WorkerImpl {
public:
    AudioFormat _formatIn, _formatOut;
    struct PFilter* pfilt;
    struct PState* pstate;

    void start_(const variant& config) override {
        _formatIn.fromVariant(*config.get("formatIn"));
        _formatOut.fromVariant(*config.get("formatOut"));
        pfilt = smarc_init_pfilter(_formatIn.sampleRate, _formatOut.sampleRate,
                                   BANDWIDTH, RP, RS, TOL,
                                   NULL, 1);
        
        pstate = smarc_init_pstate(pfilt);
    }
    
    variant process_(const variant& dataIn) override {
        /**
         * resample mono file, output mono file
         */
        
        // This code is dumb and should be optimised.
        const bytearray& bytesIn = dataIn.bytearrayVal();
        vector<double> samplesIn;
        if (_formatIn.sampleType == AudioFormat::Float32) {
            float* floats = (float*)bytesIn.data();
            for (int i=0 ; i <bytesIn.size()/4 ; i++) {
                samplesIn.push_back(floats[i]);
            }
        }
        else if (_formatIn.sampleType == AudioFormat::Int16) {
            int16_t* ints = (int16_t*)bytesIn.data();
            for (int i=0 ; i <bytesIn.size()/2; i++) {
                samplesIn.push_back(ints[i]/32767.0);
            }
        } else {
            assert(0);
        }
        
        
        // Resample
        int inbufSize = (int)samplesIn.size();
        int outbufSize = smarc_get_output_buffer_size(pfilt,inbufSize);
        double* outbuf = new double[outbufSize];
        double* inbuf = samplesIn.data();
        int written = (int)smarc_resample(pfilt, pstate, inbuf, inbufSize, outbuf, outbufSize);
        //app.log("written %d", written);
        
        // Convert to expected output type
        bytearray outbytes;
        if (_formatOut.sampleType == AudioFormat::Float32) {
            outbytes.resize(written*4);
            float* outfloats = (float*)outbytes.data();
            for (int i=0 ; i <written ; i++) {
                outfloats[i] = outbuf[i];
            }
        }
        else if (_formatOut.sampleType == AudioFormat::Int16) {
            outbytes.resize(written*2);
            int16_t* outshorts = (int16_t*)outbytes.data();
            for (int i=0 ; i <written ; i++) {
                outshorts[i] = outbuf[i] * 32767;
            }
        } else {
            assert(0);
        }
        free(outbuf);
        
        
        return outbytes;
        
        /*   // flushing last values
         while (1) {
         written = smarc_resample_flush(pfilt, pstate, outbuf,
         OUT_BUF_SIZE);
         sf_writef_double(fout, outbuf, written);
         if (written<OUT_BUF_SIZE)
         break;
         }*/
        
        
    }

    void stop_() override {
        smarc_destroy_pstate(pstate);
    }
};

DECLARE_DYNCREATE(AudioResamplerWorker);








#include "../../thirdparty/smarc/filtering.cpp"
#include "../../thirdparty/smarc/multi_stage.cpp"
#include "../../thirdparty/smarc/polyfilt.cpp"
#include "../../thirdparty/smarc/remez_lp.cpp"
#include "../../thirdparty/smarc/smarc.cpp"
#include "../../thirdparty/smarc/stage_impl.cpp"
