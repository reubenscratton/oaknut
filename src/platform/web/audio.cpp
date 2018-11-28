//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if PLATFORM_WEB

#include <oaknut.h>


class AudioWebSamples : public AudioSamples {
public:
    AudioWebSamples(void* data, int cb, AudioFormat& format) {
        _bytes.assignNoCopy((uint8_t*)data, cb);
        _format = format;
    }
    bytearray getData() override {
        return _bytes;
    }
    
    bytearray _bytes;
};


class AudioInputWeb : public AudioInput {
public:
    AudioFormat _audioFormat;
    
    AudioInputWeb() {
    }
    
    static void thunk(AudioInputWeb* audioInput, void* ptr, int cb) {
        sp<AudioSamples> samples = new AudioWebSamples(ptr, cb, audioInput->_audioFormat);
        audioInput->onNewAudioSamples(samples);
    }
    void open(AudioFormat& preferredFormat) override {
        
        // There is zero flexibility in web audio. Always float32, always mono, always 44100.
        preferredFormat.sampleType = AudioFormat::Float32;
        preferredFormat.numChannels = 1;
        preferredFormat.sampleRate = EM_ASM_INT({
            var AudioContext = window.AudioContext || window.webkitAudioContext;
            return new AudioContext().sampleRate;
        });
        _audioFormat = preferredFormat;

        EM_ASM_({
            
            var self=$0;
            var AudioContext = window.AudioContext || window.webkitAudioContext;
            var context = new AudioContext();
            var stream = navigator.mediaDevices.getUserMedia({audio: true}).then(function(stream) {

                var input = context.createMediaStreamSource(stream);
                var processor = context.createScriptProcessor(0, 1, 1);
                processor.onaudioprocess = function (e) {
                    var data = e.inputBuffer.getChannelData(0);
                    var cb = data.length * 4;
                    var ptr = _malloc(cb);
                    HEAPU8.set(new Uint8Array(data.buffer), ptr, cb);
                    Runtime.dynCall("viii", $1, [self, ptr, cb]);
                };
                input.connect(processor);
                processor.connect(context.destination);
                
            });

        }, this, thunk);
    }
    void start() override {
    }
    
    void stop() override {
    }
    void close() override {
    }
    
    
};

AudioInput* AudioInput::create() {
    return new AudioInputWeb();
}


#endif
