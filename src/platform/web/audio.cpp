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
    val _audioContext;
    val _audioStream;
    val _audioInput;
    val _audioProcessor;
    
    AudioInputWeb() : _audioContext(val::null()),
                      _audioStream(val::null()),
                      _audioInput(val::null()),
                      _audioProcessor(val::null()) {
    }
    
    static void thunkStarted(AudioInputWeb* audioInput, int indexStream, int indexInput, int indexProcessor) {
        audioInput->_audioStream = val::global("gotGet")(indexStream);
        audioInput->_audioInput = val::global("gotGet")(indexInput);
        audioInput->_audioProcessor = val::global("gotGet")(indexProcessor);
    }
    static void thunkNewAudioSamples(AudioInputWeb* audioInput, void* ptr, int cb) {
        sp<AudioSamples> samples = new AudioWebSamples(ptr, cb, audioInput->_audioFormat);
        audioInput->onNewAudioSamples(samples);
    }
    void open(AudioFormat& preferredFormat) override {
        
        // Instantiate an AudioContext
        int gotIndexContext = EM_ASM_INT({
            var AudioContext = window.AudioContext || window.webkitAudioContext;
            return gotSet(new AudioContext());
        });
        _audioContext = val::global("gotGet")(gotIndexContext);

        // There is zero flexibility in web audio. Always float32, always mono, always 44100 or 48000.
        preferredFormat.sampleType = AudioFormat::Float32;
        preferredFormat.numChannels = 1;
        preferredFormat.sampleRate = _audioContext["sampleRate"].as<int>();
        _audioFormat = preferredFormat;

    }
    
    void start() override {
        EM_ASM_ ({
            var self=$0;
            var context=gotGet($1);
            var stream = navigator.mediaDevices.getUserMedia({audio: true}).then(function(stream) {
                var input = context.createMediaStreamSource(stream);
                var processor = context.createScriptProcessor(0, 1, 1);
                processor.onaudioprocess = function (e) {
                    var data = e.inputBuffer.getChannelData(0);
                    var cb = data.length * 4; // web audio is always float32 samples
                    if (cb > 0) {
                        var ptr = _malloc(cb);
                        HEAPU8.set(new Uint8Array(data.buffer), ptr, cb);
                        Runtime.dynCall("viii", $2, [self, ptr, cb]);
                    }
                };
                input.connect(processor);
                processor.connect(context.destination);
                Runtime.dynCall("viiii", $3, [self, gotSet(stream), gotSet(input), gotSet(processor)]);
            });
        }, this, val::global("gotSet")(_audioContext).as<int>(), thunkNewAudioSamples, thunkStarted);
    }
    
    void stop() override {
        EM_ASM_ ({
            var stream = gotGet($0);
            stream.getAudioTracks().forEach(function(track) {
                track.stop();
            });
        }, val::global("gotSet")(_audioStream).as<int>());
        _audioStream = val::null();
        if (!_audioInput.isNull()) {
            _audioInput.call<void>("disconnect");
            _audioInput = val::null();
        }
        if (!_audioProcessor.isNull()) {
            _audioProcessor.call<void>("disconnect");
            _audioProcessor = val::null();
        }
    }
    void close() override {
        if (!_audioContext.isNull()) {
            _audioContext.call<void>("close");
            _audioContext = val::null();
        }
    }
    
    
};

AudioInput* AudioInput::create() {
    return new AudioInputWeb();
}


#endif
