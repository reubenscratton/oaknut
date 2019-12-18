//
//  IdaaS
//
//  Copyright © 2018 PQ. All rights reserved.
//

#pragma once
#include <oaknut.h>

class BarRenderOp : public RectRenderOp {
public:
    float _peak;
    bool _inRecording;
    bool _addedToBigOps;
    int _acc;
    
    BarRenderOp(float peak, bool inRecording) : _peak(peak), _inRecording(inRecording) {
        //setCornerRadius(app.dp(2));
    }
    
    void update(float viewHeight, float dx);
};




class RecordingView : public View {
public:
    Label* _label;
    ToolbarButton* _redo;
    vector<BarRenderOp*> _barOps;
    vector<BarRenderOp*> _barOpsRec;
    vector<BarRenderOp*> _barOpsIncoming;
    float _nextTargetPeak;
    int _numSamplesRead;
    TIMESTAMP _timestamp;
    RECT _wavArea;
    
    enum State {
        Empty,
        ActivePending,
        ActiveRecording,
        Complete
    } _state;
    
    RecordingView();
    void setState(State state);
    bool tick();
    void handleNewAudioSamples(const vector<float>& samples, int sampleRate);

    bool applySingleStyle(const string& name, const style& value) override;
    void layout(RECT constraint) override;
};

