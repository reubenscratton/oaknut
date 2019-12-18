//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "RecordingView.h"


const int PEAK_FAST_HZ=100;
const int PEAK_SLOW_HZ=20;

static float BAR_WIDTH;
static float BAR_SPACING;

DECLARE_DYNCREATE(RecordingView);

void BarRenderOp::update(float viewHeight, float dx) {
    setFillColor(_addedToBigOps ? 0xFFCc0000 : 0xFF800000);
    float barHeight = _peak * viewHeight;
    barHeight = fmax(barHeight, app->dp(1));
    setRect({_rect.origin.x + dx, (viewHeight-barHeight)/2, BAR_WIDTH, barHeight});
    invalidateBatchGeometry();
}



RecordingView::RecordingView() {
    inflate("layout/recording.res");
    bind(_label, "label");
    bind(_redo, "redo");
    setState(Empty);
    BAR_WIDTH = app->dp(3);
    BAR_SPACING = app->dp(5);
}

bool RecordingView::applySingleStyle(const string& name, const style& value) {
    if (name=="label") {
        _label->setText(value.stringVal());
        return true;
    }
    return View::applySingleStyle(name, value);
}

void RecordingView::layout(RECT constraint) {
    View::layout(constraint);
    _wavArea = _rect;
    _wavArea.origin.x = _label->getRight();
    _wavArea.size.width = _rect.size.width - (_label->getWidth() + _redo->getWidth());
}

void RecordingView::setState(State state) {
    _state = state;
    _label->setTextColor((state==Empty) ? 0xFF888888 : 0xFFFFFFFF);
    _redo->setVisibility((state==Complete) ? Visible : Invisible);
    if (state == Empty) {
        for (auto op : _barOps) {
            removeRenderOp(op);
        }
        _barOps.clear();
        _barOpsRec.clear();
        setNeedsFullRedraw();
    }
    _numSamplesRead = 0;
    _nextTargetPeak = 0;
    _timestamp = 0;
}

void RecordingView::handleNewAudioSamples(const vector<float>& samples, int sampleRate) {
    float samplesPerFrame = sampleRate / PEAK_FAST_HZ;
    for (int i=0 ; i<samples.size() ; i++) {
        float val = fabsf(samples[i]);
        _nextTargetPeak = fmaxf(_nextTargetPeak, val);
        if (++_numSamplesRead>= samplesPerFrame) {
            
            // Weight the peak a bit so it fills the available space better. Ideally this'd be a log fn
            float peak = fminf(_nextTargetPeak, 0.75) * (1/0.75f);
            
            // Add the new bar op to the right edge (unless complete)
            if (_state != Complete) {
                BarRenderOp* op = new BarRenderOp(peak, _state==ActiveRecording);
                _barOpsIncoming.push_back(op);
            }
            
            setNeedsFullRedraw();

            _nextTargetPeak = 0;
            _numSamplesRead = 0;
        }
    }
}

bool RecordingView::tick() {
    
    // Determine the time elapsed since the previous tick
    TIMESTAMP now = app->currentMillis();
    bool isFirst = (_timestamp==0);
    TIMESTAMP elapsed = now - _timestamp;
    _timestamp = now;
    if (isFirst) {
        return false;
    }
    
    // Calculate the amount that the peaks should move across the screen
    float pixelsPerMillisecond = (BAR_SPACING * PEAK_FAST_HZ) / 1000.0;
    float distanceThisTick = elapsed * pixelsPerMillisecond;
    
    float recWidth = BAR_SPACING * _barOpsRec.size();
    float recRightEdge =  _wavArea.left() + recWidth;
    
    
    bool didDoFastMove = false;
    for (int i=0 ; i<_barOps.size() ; i++) {
        BarRenderOp* op = _barOps[i];
        
        // Fast move from right -> left unless it's been added to the recorded op area
        if (!op->_addedToBigOps) {
            op->_rect.origin.x -= distanceThisTick;
            op->invalidateBatchGeometry();
            didDoFastMove = true;
        }
        
        // If it's a recording op that has moved into the big-op area
        if (op->_inRecording && !op->_addedToBigOps) {
            if (op->_rect.origin.x < recRightEdge) {
                op->_addedToBigOps = true;
                BarRenderOp* rightmostBigOp = NULL;
                if (_barOpsRec.size() > 0) {
                    rightmostBigOp = *_barOpsRec.rbegin();
                    if (rightmostBigOp->_acc>=(PEAK_FAST_HZ/PEAK_SLOW_HZ)) {
                        rightmostBigOp = NULL;
                    }
                }
                if (rightmostBigOp) {
                    rightmostBigOp->_peak = fmaxf(rightmostBigOp->_peak, op->_peak);
                    rightmostBigOp->_acc++;
                    rightmostBigOp->update(_rect.size.height, 0);
                    _barOps.erase(_barOps.begin()+i);
                    removeRenderOp(op);
                    i--;
                    continue;
                    
                } else {
                    op->update(_rect.size.height, (recRightEdge - op->_rect.origin.x));
                    _barOpsRec.push_back(op);
                }
            }
        }
        
        // Remove if flew off the left edge
        if (op->_rect.right()<_wavArea.left()) {
            _barOps.erase(_barOps.begin()+i);
            removeRenderOp(op);
            i--;
            continue;
        }
        
    }
    
    
    // Bring in incoming peaks
    if (_barOpsIncoming.size() > 0) {
        float x = 0;
        //if (_barOps.size()==0) {
            x = _wavArea.right();
        //} else {
        //    BarRenderOp* prevOp = *_barOps.rbegin();
        //    x = prevOp->_rect.origin.x + BAR_SPACING;
        //}
        for (auto op : _barOpsIncoming) {
            op->update(_rect.size.height, x);
            addRenderOp(op);
            _barOps.push_back(op);
            x += BAR_SPACING;
        }
        _barOpsIncoming.clear();
    }

    
    // Return true if finished building the recorded peaks set
    return !didDoFastMove && (_state==Complete);
}

    
