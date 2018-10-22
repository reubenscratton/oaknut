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
    
    View* _view;
	Window* _window;
    list<sp<Animation>>::iterator _windowAnimationsListIterator;
    OnAnimationFinishedDelegate _onFinished;
    TIMESTAMP _timeStarted;
    TIMESTAMP _elapsedAtPause;
    int _duration;
    int _delay;
    int _flags;
    bool _paused;
	InterpolateFunc _interpolator;
	float _fromVal;
	float _toVal;
    
    Animation(View* view);
    Animation(View* view, float fromVal, float toVal);
    ~Animation();
    virtual void start(int duration);
    virtual void start(int duration, int delay);
    virtual void stop();
    virtual void pause();
    virtual void unpause();
	virtual bool tick(TIMESTAMP now);
    virtual void apply(float val) = 0;
    
    static Animation* start(View* view, int duration, std::function<void(float)> callback, InterpolateFunc interpolater = linear);
    
    // Interpolators
    static float linear(float t, float b, float c, float d);
    static float strongEaseInOut(float t, float b, float c, float d);
    static float regularEaseIn(float t, float b, float c, float d);
    static float regularEaseInOut(float t, float b, float c, float d);
    static float easeOut(float t, float b, float c, float d);
    static float strongEaseIn(float t, float b, float c, float d);
    static float strongEaseOut(float t, float b, float c, float d);
    static float nowhere(float t, float b, float c, float d);
    static float bounceEaseOut(float t, float b, float c, float d);
    static float bounceEaseIn (float t, float b, float c, float d);
    static float bounceEaseInOut(float t, float b, float c, float d);
    static float elasticEaseIn (float t, float b, float c, float d);
    static float elasticEaseOut(float t, float b, float c, float d);

};


class AlphaAnimation : public Animation {
public:
	
	AlphaAnimation(View* view, float target);
    virtual void apply(float val);
};

class LayoutAnimation : public Animation {
public:
    
    static LayoutAnimation* startHorizontal(View* view, ALIGNSPEC newAlignspec, int duration, InterpolateFunc interpolator = linear);
    static LayoutAnimation* startVertical(View* view, ALIGNSPEC newAlignspec, int duration, InterpolateFunc interpolator = linear);
    static LayoutAnimation* startPositional(View* view, ALIGNSPEC newAlignspecHorz, ALIGNSPEC newAlignspecVert, int duration, InterpolateFunc interpolator = linear);
    
protected:
    
    LayoutAnimation(View* view, InterpolateFunc interpolator = linear);
    
    bool _affectsAlignspecHorz;
    bool _affectsAlignspecVert;
    ALIGNSPEC _newAlignspecHorz;
    ALIGNSPEC _newAlignspecVert;
    float _originHorzStart;
    float _originHorzEnd;
    float _originVertStart;
    float _originVertEnd;
    bool _valid;
    
    void apply(float val);
    
};



