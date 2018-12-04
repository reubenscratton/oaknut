//
//  IdaaS
//
//  Copyright © 2018 PQ. All rights reserved.
//

#pragma once
#include <oaknut.h>
#include "RecordingView.h"

class AudioRecordingViewController : public ViewController {
public:
    Label* _instruction;
    Label* _phrase;
    ImageView* _recordButton;
    Label* _recordButtonLabel;
    sp<AudioInput> _audioInput;
    bool _isRecording;
    RecordingView* _rec[3];
    int _current;
    Timer* _timer;
    
    
    AudioRecordingViewController();
    void autoprogress();
    void tick();
    void onWindowAttached() override;
    void onWindowDetached() override;
    void startRecording();
    void stopRecording();
    void onRedoButtonPressed(int num);
    
};
