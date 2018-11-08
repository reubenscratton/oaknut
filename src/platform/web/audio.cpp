//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if PLATFORM_WEB

#include <oaknut.h>


class AudioInputWebSamples : public AudioInputSamples {
public:
    AudioInputWebSamples(void* data, int cb) {
        _bytes.assignNoCopy((uint8_t*)data, cb);
    }
    bytearray getData() override {
        return _bytes;
    }
    
    bytearray _bytes;
};


class AudioInputWeb : public AudioInput {
public:
    
    AudioInputWeb() {
    }
    
    static void thunk(AudioInputWeb* audioInput, void* ptr, int cb) {
        sp<AudioInputSamples> samples = new AudioInputWebSamples(ptr, cb);
        audioInput->onNewAudioSamples(samples);
    }
    void open(int sampleRate) override {
        EM_ASM_({
            
            var self=$0;
            var AudioContext = window.AudioContext || window.webkitAudioContext;
            var stream = navigator.mediaDevices.getUserMedia({audio: true}).then(function(stream) {

                var context = new AudioContext();
                //console.log("The sample rate is " + context.sampleRate);
                var input = context.createMediaStreamSource(stream);
                var processor = context.createScriptProcessor(2048, 1, 1);
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
