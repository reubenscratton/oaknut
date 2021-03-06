//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


DECLARE_DYNCREATE(ProgressSpinner);


ProgressSpinner::ProgressSpinner() {
}


void ProgressSpinner::layout(RECT constraint) {
    CanvasView::layout(constraint);
    redraw();
}

void ProgressSpinner::attachToWindow(Window* window) {
    View::attachToWindow(window);
    retain();
    assert(!_animTimer);
    _animTimer = Timer::start([=]() {
        _phase++;
        redraw();
    }, 140, true);
}

void ProgressSpinner::detachFromWindow() {
    View::detachFromWindow();
    if (_animTimer) {
        _animTimer->stop();
        _animTimer = NULL;
        release();
    }
}

void ProgressSpinner::redraw() {
    _canvas->clear(0);
    COLOR colour = _effectiveTintColor;
    assert(colour); // this widget expects a tint colour
    _canvas->clearTransform();
    float dotSize = app->dp(5);
    float midX = getWidth() / 2;
    float midY = getHeight() / 2;
    float rx = (_rect.size.width - dotSize) / 2;
    float ry = (_rect.size.height - dotSize) / 2;
    for (uint32_t i=0 ; i<12 ; i++) {
        float a = 2*M_PI * (i/12.0);
        RECT rect(midX + cosf(a) * rx - dotSize/2,
                  midY + sinf(a) * ry - dotSize/2,
                  dotSize,dotSize);
        uint32_t alpha = 255 - (((i+_phase)%12) * 16);
        _canvas->setFillColor((colour & 0xFFFFFF) | (alpha<<24));
        _canvas->drawOval(rect);
    }
    setNeedsFullRedraw();
}



