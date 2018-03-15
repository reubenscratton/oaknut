//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"


Animation::Animation() : Animation(0,1) {
}
Animation::Animation(float fromVal, float toVal) : _interpolater(linear),
												   _state(ANIMATION_STATE_STOPPED),
												   _fromVal(fromVal),
												   _toVal(toVal) {
}

Animation::~Animation() {
}

void Animation::start(Window* window, int duration) {
#if CONFIG_SLOW_ANIMATIONS
    duration *= 10;
#endif
    start(window, duration,0);
}
void Animation::start(Window* window, int duration, int delay) {
    if (_state == ANIMATION_STATE_STARTED || _state == ANIMATION_STATE_PAUSED) {
        return;
    }
    _duration = duration;
    _delay = delay;
    _state = ANIMATION_STATE_STARTED;
    _timeStarted = app.currentMillis();
	_window = window;
	_window->_animations.insert(_window->_animations.end(), this);
    _window->requestRedraw();
}

void Animation::stop() {
    if (_state != ANIMATION_STATE_STOPPED) {
        _window = NULL;
        _timeStarted = 0;
        _state = ANIMATION_STATE_STOPPED;
        // todo: delegate here?
    }
}

void Animation::pause() {
    if (_state != ANIMATION_STATE_STARTED) {
        return;
    }
    _state = ANIMATION_STATE_PAUSED;
    _elapsedAtPause = app.currentMillis() - _timeStarted;
}

void Animation::unpause() {
    if (_state != ANIMATION_STATE_PAUSED) {
        return;
    }
    _timeStarted = app.currentMillis() - _elapsedAtPause;
    _elapsedAtPause = 0;
    _state = ANIMATION_STATE_STARTED;
}


void Animation::tick(long now) {
	float elapsed = now - (_timeStarted + _delay);
	if (elapsed<0) { // not started yet
		return;
	}

	// Clamp elapsed time to duration. If duration has been exceeded set 'finished' true.
	bool finished = false;
	finished = (elapsed >= _duration) ;
	if (finished) {
		elapsed = _duration;
	}
        
	// Call apply()
    float val = _interpolater(elapsed, 0, 1, _duration);
	apply(_fromVal + (_toVal-_fromVal)*val);
        
	// If animation has finished
	if (finished) {
		stop();
		if (_onFinished) {
			_onFinished(this);
        }
    }
}


void DelegateAnimation::apply(float val) {
	_delegate(val);
}



AlphaAnimation::AlphaAnimation(View* view, float target) : _view(view) {
	_fromVal = view->getAlpha();
	_toVal = target;
}
void AlphaAnimation::apply(float val) {
    _view->setAlpha(val);
}
void AlphaAnimation::stop() {
	_view = NULL;
	Animation::stop();
}




float linear(float t, float b, float c, float d) {
	return b + c*(t/d);
}

//
// Tweening functions. The 4 parameters are :
//
//  t - time, ranges from 0 to d
//  b - begin, i.e. the initial value for the quantity being changed over time
//  c - change, the amount b will be changed by at the end
//  d - duration, of the transition, normally in milliseconds. 
//
// All were adapted from http://jstween.sourceforge.net/Tween.js 
//
float regularEaseInOut(float t, float b, float c, float d) {
	if ((t/=d/2) < 1) return c/2*t*t + b;
    t--;
	return -c/2 * ((t)*(t-2) - 1) + b;
}
float strongEaseInOut(float t, float b, float c, float d) {
    t/=d/2; 
    if (t < 1) return c/2*t*t*t*t*t + b;
    t-=2;
    return c/2*(t*t*t*t*t + 2) + b;
}
float regularEaseIn(float t, float b, float c, float d) {
    t/=d;
    return c*t*t + b;
}
float easeOut(float t, float b, float c, float d) {
    t/=d;
    return -c * t *(t-2) + b;
}
float strongEaseIn(float t, float b, float c, float d) {
    t/=d;
    return c*t*t*t*t*t + b;
}
float strongEaseOut(float t, float b, float c, float d) {
    t=t/d-1;
    return c*(t*t*t*t*t + 1) + b;
}
float nowhere(float t, float b, float c, float d) {
    t/=d/2; // t<0<=2
    if (t >= 1) t=1-(t-1);
    return c*t*t + b;
}
float bounceEaseOut(float t, float b, float c, float d) {
    if ((t/=d) < (1.0f/2.75f)) {
        return c*(7.5625f*t*t) + b;
    } else if (t < (2.0f/2.75f)) {
        t-=(1.5f/2.75f);
        return c*(7.5625f*(t)*t + .75f) + b;
    } else if (t < (2.5f/2.75f)) {
        t-=(2.25f/2.75f);
        return c*(7.5625f*(t)*t + .9375f) + b;
    } else {
        t-=(2.625/2.75);
        return c*(7.5625f*(t)*t + .984375f) + b;
    }
}
float bounceEaseIn (float t, float b, float c, float d) {
    return c - bounceEaseOut (d-t, 0, c, d) + b;
}
float bounceEaseInOut(float t, float b, float c, float d) {
    if (t < d/2) return bounceEaseIn (t*2, 0, c, d) * .5f + b;
    else return bounceEaseOut (t*2-d, 0, c, d) * .5f + c*.5f + b;
}
float elasticEaseIn (float t, float b, float c, float d) {
    if (t == 0)
        return b;
    if ((t /= d) == 1)
        return b + c;
    float p = d * .3f;
    float a = c;
    float s = p / 4;
    t -= 1;
    return -(a * (float) powf(2, 10 * t) * (float) sinf((t * d - s) * (2 * (float)M_PI) / p)) + b;
}
static int OVERSHOOT = -15; // increase magnitude => reduce overshoot
float elasticEaseOut(float t, float b, float c, float d) {
    if (t == 0)
        return b;
    if ((t /= d) == 1)
        return b + c;
    float p = d * .5f;
    float a = c;
    float s = p / 4;
    return (a * (float) powf(2, OVERSHOOT * t) * (float) sinf((t * d - s) * (2 * (float)M_PI) / p) + c + b);
}



