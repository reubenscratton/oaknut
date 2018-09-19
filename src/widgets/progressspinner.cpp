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


void ProgressSpinner::layout() {
    CanvasView::layout();
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
    _canvas->setAffineTransform(NULL);
    float stroke = app.dp(5);
    _canvas->setStrokeWidth(stroke);
    float rx = (_rect.size.width - stroke) / 2;
    float ry = (_rect.size.height - stroke) / 2;
    for (uint32_t i=0 ; i<12 ; i++) {
        float a = 2*M_PI * (i/12.0);
        POINT pt = {
            getWidth()/2 + cosf(a) * rx,
            getHeight()/2 + sinf(a) * ry
        };
        uint32_t alpha = 255 - (((i+_phase)%12) * 16);
        ObjPtr<Path> path = _canvas->createPath();
        _canvas->setStrokeColor((colour & 0xFFFFFF) | (alpha<<24));
        path->moveTo(pt);
        path->lineTo(pt);
        _canvas->drawPath(path);
    }
    setNeedsFullRedraw();
}



