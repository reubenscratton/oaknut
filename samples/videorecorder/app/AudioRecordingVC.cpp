//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#include "AudioRecordingVC.h"


AudioRecordingViewController::AudioRecordingViewController() {
    inflate("layout/audio.res");
    bind(_instruction, "instruction");
    bind(_phrase, "phrase");
    bind(_recordButton, "record");
    bind(_recordButtonLabel, "recordLabel");
    bind(_rec[0], "rec1");
    bind(_rec[1], "rec2");
    bind(_rec[2], "rec3");
    for (int i=0 ; i<3 ; i++) {
        _rec[i]->_redo->onClick = [=]() {
            onRedoButtonPressed(i);
        };
    }
    
    _recordButton->onInputEvent = [=](View* view, INPUTEVENT* event) -> bool {
        if (event->type == INPUT_EVENT_DOWN) {
            startRecording();
        }
        if (event->type == INPUT_EVENT_UP) {
            stopRecording();
        }
        return true;
    };
    
}
    
void AudioRecordingViewController::autoprogress() {
    _current = -1;
    for (int i=0 ; i<3 ; i++) {
        if (_rec[i]->_state != RecordingView::Complete) {
            _current = i;
            _rec[_current]->setState(RecordingView::ActivePending);
            break;
        }
    }
    if (_current >= 0) {
        if (!_timer) {
            _timer = Timer::start([=]() {
                tick();
            }, 16, true);
        }
    } else {
        if (_timer) {
            _timer->stop();
            _timer = NULL;
        }
        _audioInput->stop();
        _audioInput->close();
        _audioInput = NULL;
    }
}
    
void AudioRecordingViewController::tick() {
    assert(_current >= 0);
    if (_rec[_current]->tick()) {
        _rec[_current]->setState(RecordingView::Complete);
        autoprogress();
    }
}
    
void AudioRecordingViewController::onWindowAttached() {
    ViewController::onWindowAttached();
    _recordButton->setEnabled(false);
    
    // Do permissions check
    getWindow()->runWithPermissions({PermissionMic}, [=](vector<bool> granted) {
        if (granted[0]) {
            _recordButton->setEnabled(true);
            
            // Open the mic
            _audioInput = AudioInput::create();
            AudioFormat audioFormat;
            audioFormat.sampleRate = 16000;
            audioFormat.numChannels = 1;
            audioFormat.sampleType = AudioFormat::Int16;
            _audioInput->open(audioFormat);
            _audioInput->onNewAudioSamples = [=](AudioSamples* samples) {
                if (_current<0) {
                    return;
                    
                }
                // Process a new batch of audio samples. We capture peaks at a fast rate
                // so they whizz across the screen
                vector<float> floats = samples->getDataFloat32();
                _rec[_current]->handleNewAudioSamples(floats,  samples->_format.sampleRate);
                
            };
            _audioInput->start();
            
            autoprogress();
            
        }
    });
}

void AudioRecordingViewController::onWindowDetached() {
    ViewController::onWindowDetached();
    if (_audioInput) {
        _audioInput->stop();
        _audioInput->close();
        _audioInput = NULL;
    }
}
    
void AudioRecordingViewController::startRecording() {
    //_instruction->setVisibility(Visibility::Invisible);
    //_recordButtonLabel->setVisibility(Visibility::Invisible);
    _phrase->setTextColor(0xFF29ca92);
    _isRecording = true;
    _rec[_current]->setState(RecordingView::ActiveRecording);
}
    
void AudioRecordingViewController::stopRecording() {
    _isRecording = false;
    _rec[_current]->setState(RecordingView::Complete);
}

void AudioRecordingViewController::onRedoButtonPressed(int num) {
    _rec[num]->setState(RecordingView::Empty);
    if (_current<3 && num!=_current) {
        _rec[_current]->setState(RecordingView::Empty);
    }
    autoprogress();
}

