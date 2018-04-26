//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class Interpolator {
public:
    virtual float getInterpolation(float input) = 0;
};


class Scrollbar : public Object {
public:
    int mMode;
    
    float mStart;
    float mFinal;
    float mMin;
    float mMax;
    float mCurr;
    long mStartTime;
    int mDuration;
    float mDurationReciprocal;
    float mDelta;
    bool mFinished;
    //Interpolator mInterpolator;
    float mCoeff;
    float mVelocity;
    
    float mDeceleration;
    
    RoundRectRenderOp* _renderOp;

    Scrollbar(View* view);
    virtual bool computeScrollOffset();
    virtual void startScroll(float start, float d);
    virtual void startScroll(float start, float d, int duration);
    virtual void fling(float start, float velocity, float min, float max);
    
    void updateRect();
};

/*
class SplineOverScroller : public Scroller {
public:
    float mCurrVelocity;    // Current velocity
    long mStartTime; // Animation starting time, in system milliseconds
    int mSplineDuration; // Duration to complete spline component of animation
    int mSplineDistance;     // Distance to travel along spline animation
    int mOver;     // The allowed overshot distance before boundary is reached.
    float mFlingFriction;     // Fling friction
    int mState;     // Current state of the animation.
    float mPhysicalCoeff;     // A context-specific coefficient adjusted to physical values.

    static void staticInit();
    
    SplineOverScroller();
    
    void fling(int start, int velocity, int min, int max, int over);
    
    void setFriction(float friction);
    void updateScroll(float q);
    void adjustDuration(int start, int oldconst, int newconst);
    void startScroll(int start, int distance, int duration);
    void finish();
    void setFinalPosition(int position);
    void extendDuration(int extend);
    bool springback(int start, int min, int max);
    void startSpringback(int start, int end, int velocity);
    double getSplineDeceleration(int velocity);
    double getSplineFlingDistance(int velocity);
    int getSplineFlingDuration(int velocity);
    void fitOnBounceCurve(int start, int end, int velocity);
    void startBounceAfterEdge(int start, int end, int velocity);
    void startAfterEdge(int start, int min, int max, int velocity);
    void notifyEdgeReached(int start, int end, int over);
    void onEdgeReached();
    bool continueWhenFinished();
    bool update();
};
*/
/*
class OverScroller : public Object {
public:

    int mMode;
    SplineOverScroller* mScrollerX;
    SplineOverScroller* mScrollerY;
    Interpolator* mInterpolator;
    bool mFlywheel;

    OverScroller(Interpolator* interpolator, bool flywheel);
    OverScroller(Interpolator* interpolator, float bounceCoefficientX, float bounceCoefficientY);
    OverScroller(Interpolator* interpolator, float bounceCoefficientX, float bounceCoefficientY, bool flywheel);
    
    void setInterpolator(Interpolator* interpolator);
    void setFriction(float friction);
    bool isFinished();
    void forceFinished(bool finished);
    int getCurrX();
    int getCurrY();
    float getCurrVelocity();
    int getStartX();
    int getStartY();
    int getFinalX();
    int getFinalY();
    bool computeScrollOffset();
    void startScroll(int startX, int startY, int dx, int dy);
    void startScroll(int startX, int startY, int dx, int dy, int duration);
    bool springBack(int startX, int startY, int minX, int maxX, int minY, int maxY);
    void fling(int startX, int startY, int velocityX, int velocityY, int minX, int maxX, int minY, int maxY);
    void fling(int startX, int startY, int velocityX, int velocityY,
                             int minX, int maxX, int minY, int maxY, int overX, int overY);
    void notifyHorizontalEdgeReached(int startX, int finalX, int overX);
    void notifyVerticalEdgeReached(int startY, int finalY, int overY);
    bool isOverScrolled();
    void abortAnimation();
    int timePassed();
    bool isScrollingInDirection(float xvel, float yvel);
};

*/
