//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

static const float SCROLL_FRICTION = 0.015f;

static const float DECELERATION = 9.80665f         // Earth's gravity (m/s^2)
                                * 39.37f           // inch/meter
                                * 160              // pixels per inch
                                * SCROLL_FRICTION;

static int inline signum(float x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}



Fling::Fling(float start, float velocity, float min, float max) {
    _coeff = signum(velocity);
    velocity = hypotf(velocity, velocity);
    _velocity = velocity;
    _duration = (int) (1000 * velocity / DECELERATION);
    _startTime = app.currentMillis();
    _start = start;
    _min = min;
    _max = max;
    float totalDistance = ((velocity * velocity) / (2 * DECELERATION));
    _final = start + totalDistance * _coeff;
    _final = fminf(_final, max);
    _final = fmaxf(_final, min);
}

void Fling::update(float timePassedSeconds) {
    float distance = (_velocity * timePassedSeconds)
                   - (DECELERATION * timePassedSeconds * timePassedSeconds / 2.0f);
    
    _curr = _start + distance * _coeff;
    _curr = fminf(_curr, _max);
    _curr = fmaxf(_curr, _min);
}

class Bounce : public Fling {
public:
    Bounce(float start, float end) : Fling(0,0,0,0) {
        _startTime = app.currentMillis();
        _duration = 500;
        _start = start;
        _final = end;
    }
    void update(float timePassedSeconds) {
        _curr = strongEaseOut(timePassedSeconds, _start, _final-_start, _duration/1000.f);
    }
};


/*
 Called during layout and when contentOffset changes.
 */
bool ScrollInfo::canScroll(View* view, bool isVertical) {
    float visibleSize = isVertical ? view->_rect.size.height : view->_rect.size.width;
    float scrollableSize = isVertical
                        ? (view->_padding.top + view->_contentSize.height + view->_padding.bottom + (view->_scrollInsets.top+view->_scrollInsets.bottom))
                        :  view->_contentSize.width + (view->_scrollInsets.left+view->_scrollInsets.right);
    return scrollableSize>0 && visibleSize < scrollableSize;
}

void ScrollInfo::updateVisibility(View* view, bool isVertical) {
    if (canScroll(view, isVertical)) {
        if (!_renderOp) {
            _renderOp = new RoundRectRenderOp(NULL, 0xFF4E4E4E, 0,0, 2.5); // todo: STYLE!
            view->addScrollbarOp(_renderOp);
        }
        
        // If there's no fade-in animation and the alpha isn't yet one, run the
        if (_alpha<1 && (!_fadeAnim || _fadeAnim->_toVal != 1.0f)) {
            if (_fadeTimer) {
                _fadeTimer->stop();
            }
            _fadeTimer = Timer::start([=]() {
                startFadeAnim(0.0f);
            }, 500, false); // todo: style!
            startFadeAnim(1.0f);
        }
        
        updateRect(view);
    } else {
        if (_renderOp) {
            view->removeScrollbarOp(_renderOp);
            _renderOp = NULL;
        }
        detach();
    }
}


void ScrollInfo::detach() {
    if (_fadeAnim) {
        _fadeAnim = NULL;
    }
    if (_fadeTimer) {
        _fadeTimer->stop();
        _fadeTimer = NULL;
    }
}


void ScrollInfo::startFadeAnim(float targetAlpha) {
    View* view = _renderOp->_view;
    if (!view || !view->_window) {
        return;
    }
    if (!_fadeAnim) {
        _fadeAnim = Animation::start(view, 300, [=](float val) {
            _alpha = val;
            if (_renderOp) {
                _renderOp->setAlpha(val);
                view->invalidateRect(_renderOp->_rect);
            }
        });
    }
    _fadeAnim->stop();
    _fadeAnim->_fromVal = _alpha;
    _fadeAnim->_toVal = targetAlpha;
    _fadeAnim->start(300);
}

bool ScrollInfo::handleEvent(View* view, bool isVertical, INPUTEVENT* event) {

    bool rv = false;
    float offset = isVertical ? view->_contentOffset.y : view->_contentOffset.x;
    float contentSize = isVertical ? (view->_padding.top + view->_contentSize.height + view->_padding.bottom)
                                   : (view->_padding.left + view->_contentSize.width + view->_padding.right);
    float scrollableSize = contentSize + (isVertical
                            ?  (view->_scrollInsets.top+view->_scrollInsets.bottom)
                            :  (view->_scrollInsets.left+view->_scrollInsets.right));
    float viewSize = isVertical ? view->_rect.size.height : view->_rect.size.width;
    float maxScroll = fmax(0, scrollableSize - viewSize);

    float val, d=0.0f;
    val = isVertical ? event->pt.y : event->pt.x;
    if (event->deviceType == INPUTEVENT::ScrollWheel) {
        d = isVertical ? event->delta.y : event->delta.x;
        val = d;
    }
    
    if (event->type == INPUT_EVENT_DOWN) {
        _dragStart = _dragLast = val;
        _offsetStart = offset;
        _dragTotal = 0;
        flingCancel();
    }
    if (event->type == INPUT_EVENT_MOVE) {
        if (event->deviceType == INPUTEVENT::Mouse) {
            d = (val - _dragLast);
        }
        _dragLast = val;
    }

    if (d != 0.0) {
        _dragTotal += d;
        if (isVertical) {
            if (view->canScrollVertically()) {
                POINT newContentOffset = view->_contentOffset;
                newContentOffset.y = _offsetStart - _dragTotal;
                // Overscroll handling
                if (newContentOffset.y < 0) {
                    newContentOffset.y /= fmax(1.0f, log10(_dragTotal));
                } else if (newContentOffset.y > maxScroll) {
                    float overshoot = newContentOffset.y - maxScroll;
                    newContentOffset.y = maxScroll + overshoot / fmax(1.0f, log10(overshoot));
                }
                view->setContentOffset(newContentOffset);
                rv = true;
            }
        }
    }
    
    offset = isVertical ? view->_contentOffset.y : view->_contentOffset.x;
    bool overscroll = false;
    if (offset < 0) {
        overscroll = true;
    } else if (offset > maxScroll) {
        overscroll = true;
    }


    if (event->type == INPUT_EVENT_UP) {
        if (overscroll) {
            flingCancel();
            offset = isVertical ? view->_contentOffset.y : view->_contentOffset.x;
            _fling = new Bounce(offset, (offset<=0)?0:contentSize-viewSize);
            view->setNeedsFullRedraw();
        }
    }

    if (event->type == INPUT_EVENT_FLING && !overscroll) {
        bool canScroll = isVertical ? view->canScrollVertically() : view->canScrollHorizontally();
        if (canScroll) {
            flingCancel();
            val = isVertical ? event->velocity.y : event->velocity.x;
            _fling = new Fling(offset, -val, 0, contentSize-viewSize);
            view->setNeedsFullRedraw();
        }
    }
    return rv;
}



void ScrollInfo::flingCancel() {
    if (_fling) {
        delete _fling;
        _fling = NULL;
    }
}

float ScrollInfo::flingUpdate() {
    TIMESTAMP timePassed = (app.currentMillis() - _fling->_startTime);
    float retval;
    if (timePassed < _fling->_duration) {
        _fling->update(timePassed / 1000.0f);
        retval = _fling->_curr;
    } else {
        retval = _fling->_final;
    }
    if (retval == _fling->_final) {
        flingCancel();
    }
    return retval;
}

void ScrollInfo::updateRect(View* view) {
    SIZE contentSize = view->getContentSize();
    POINT contentOffset = view->getContentOffset();
    float insets = view->_scrollInsets.top+view->_scrollInsets.bottom;
    float visibleContentHeight = contentSize.height - insets;
    float scale = (view->getHeight()-insets) / visibleContentHeight;
    float scrollbarLength = (view->getHeight()-insets) * scale;
    scrollbarLength = fmaxf(scrollbarLength, app.dp(40)); // todo: style!
    RECT rect = view->getOwnRect();
    rect.origin.x = rect.size.width - (5+4); // todo: style!
    rect.origin.y = view->_scrollInsets.top + contentOffset.y * scale
    +contentOffset.y; // Add the unscaled contentOffset because we have to correct for
                      // contentOffset now being part of MVP translation!
    rect.size.width = 5; // todo: style!
    rect.size.height = scrollbarLength;
    _renderOp->setRect(rect);
}



