//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

typedef std::function<void(class Animation*)> OnAnimationFinishedDelegate;

typedef float (*InterpolateFunc)(float t, float b, float c, float d);

/**
 *  @ingroup views
 *  @brief Animation base class. Animations are associated with a View and
 * are owned and updated by the Window attached to the View.
 */
class Animation : public Object {
public:
    
    /** @name Construction
     * @{
     */
    Animation(View* view);
    Animation(View* view, float fromVal, float toVal);
    /** @} */
    
    /**  @cond INTERNAL */
    ~Animation();
    /**  @endcond */
    
    /** @name Starting & Stopping
     * @{
     */
    virtual void start(int duration);
    virtual void start(int duration, int delay);
    virtual void stop();
    virtual void pause();
    virtual void unpause();
    /** @} */

    
    static Animation* start(View* view, int duration, std::function<void(float)> callback, InterpolateFunc interpolater = linear);

    /** @name Interpolater
     * @{
     */
    virtual void setInterpolater(InterpolateFunc interpolator);
    virtual InterpolateFunc getInterpolater() const;
    /** @} */

    /** @name Interpolation functions
     * @{
     */
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
    /** @} */

    /** @name Handlers
     * @{
     */
    OnAnimationFinishedDelegate onFinished;
    /** @} */

protected:
    virtual bool tick(TIMESTAMP now);
    virtual void apply(float val) = 0;

    friend class Window;
    friend class ScrollInfo;
    
    View* _view;
    Window* _window;
    list<sp<Animation>>::iterator _windowAnimationsListIterator;
    TIMESTAMP _timeStarted;
    TIMESTAMP _elapsedAtPause;
    int _duration;
    int _delay;
    int _flags;
    bool _paused;
    InterpolateFunc _interpolator;
    float _fromVal;
    float _toVal;
    
};


/**
 *  @ingroup views
 *  @brief Animation that works on the `alpha` property of a View.
 */
class AlphaAnimation : public Animation {
public:
	
	AlphaAnimation(View* view, float target);
    void apply(float val) override;
};


/**
 *  @ingroup views
 *  @brief Animation that works on a View's position and updates the View's layout properties at the end,
 * i.e. it is for persistent animations.
 */
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
    
    void apply(float val) override;
    
};



