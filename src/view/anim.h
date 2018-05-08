//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

typedef std::function<void(class Animation*)> OnAnimationFinishedDelegate;

typedef float (*InterpolateFunc)(float t, float b, float c, float d);


class Animation : public Object {
public:
    
    friend class Window;
    
	Window* _window;
    list<ObjPtr<Animation>>::iterator _windowAnimationsListIterator;
    View* _view;
    OnAnimationFinishedDelegate _onFinished;
    long _timeStarted;
    long _elapsedAtPause;
    int _duration;
    int _delay;
    int _flags;
    bool _paused;
	InterpolateFunc _interpolater;
	float _fromVal;
	float _toVal;
    
    Animation();
    Animation(float fromVal, float toVal);
    ~Animation();
    virtual void start(Window* window, int duration);
    virtual void start(Window* window, int duration, int delay);
    virtual void stop();
    virtual void pause();
    virtual void unpause();
	virtual bool tick(long now);
    virtual void apply(float val) = 0;
};

class DelegateAnimation : public Animation {
public:
    std::function<void(float)> _delegate;
    
    virtual void apply(float val);
};

class AlphaAnimation : public Animation {
public:
	
	AlphaAnimation(View* view, float target);
    virtual void apply(float val);
};


float linear(float t, float b, float c, float d);
float strongEaseInOut(float t, float b, float c, float d);
float regularEaseIn(float t, float b, float c, float d);
float regularEaseInOut(float t, float b, float c, float d);
float easeOut(float t, float b, float c, float d);
float strongEaseIn(float t, float b, float c, float d);
float strongEaseOut(float t, float b, float c, float d);
float nowhere(float t, float b, float c, float d);
float bounceEaseOut(float t, float b, float c, float d);
float bounceEaseIn (float t, float b, float c, float d);
float bounceEaseInOut(float t, float b, float c, float d);
float elasticEaseIn (float t, float b, float c, float d);
float elasticEaseOut(float t, float b, float c, float d);

