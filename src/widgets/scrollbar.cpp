//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

static const int DEFAULT_DURATION = 250;
static const int SCROLL_MODE = 0;
static const int FLING_MODE = 1;


static float sViscousFluidScale = 8.0f; // This controls the viscous fluid effect (how much of it)


static const float GRAVITY_EARTH = 9.80665f;
static const float SCROLL_FRICTION = 0.015f;

int signum(float x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}

static float viscousFluid(float x)
{
    x *= sViscousFluidScale;
    if (x < 1.0f) {
        x -= (1.0f - expf(-x));
    } else {
        float start = 0.36787944117f;   // 1/e == exp(-1)
        x = 1.0f - expf(1.0f - x);
        x = start + x * (1.0f - start);
    }
    return x;
}


Scrollbar::Scrollbar(View* view) {
    mCoeff = 1.0f;
    mFinished = true;

    mDeceleration = GRAVITY_EARTH   // g (m/s^2)
        * 39.37f                        // inch/meter
        * 160              // pixels per inch
        * SCROLL_FRICTION;
    
    _renderOp = new RoundRectRenderOp(view, 0xFF4E4E4E, 0,0, 2.5);
    view->addRenderOp(_renderOp);
    updateRect();
}

void Scrollbar::updateRect() {
    View* view = _renderOp->_view;
    float insets = view->_scrollInsets.top+view->_scrollInsets.bottom;
    float visibleContentHeight = view->_contentSize.height - insets;
    float scale = (view->_frame.size.height-insets) / visibleContentHeight;
    float scrollbarLength = (view->_frame.size.height-insets) * scale;
    scrollbarLength = fmaxf(scrollbarLength, app.dp(40));
    RECT rect = view->getBounds();
    rect.origin.x = rect.size.width - 9;
    rect.origin.y = view->_scrollInsets.top + view->_contentOffset.y * scale
        +view->_contentOffset.y; // Add the unscaled contentOffset because we have to correct for
                                 // contentOffset now being part of MVP translation!
    rect.size.width = 5;
    rect.size.height = scrollbarLength;
    _renderOp->setRect(rect);
}

/**
 * Call this when you want to know the new location.  If it returns true,
 * the animation is not yet finished.  loc will be altered to provide the
 * new location.
 */
bool Scrollbar::computeScrollOffset() {
    if (mFinished) {
        return false;
    }
    
    int timePassed = (int)(app.currentMillis() - mStartTime);
    
    if (timePassed < mDuration) {
        if (mMode == SCROLL_MODE) {
                float x = (float)timePassed * mDurationReciprocal;
                
                //if (mInterpolator == null)
                    x = viscousFluid(x);
                //else
                //    x = mInterpolator.getInterpolation(x);
                
                mCurr = mStart + x * mDelta;
            
        } else if (mMode == FLING_MODE) {
                float timePassedSeconds = timePassed / 1000.0f;
                float distance = (mVelocity * timePassedSeconds)
                - (mDeceleration * timePassedSeconds * timePassedSeconds / 2.0f);
                
                mCurr = mStart + distance * mCoeff;
                // Pin to mMinX <= mCurrX <= mMaxX
                mCurr = fminf(mCurr, mMax);
                mCurr = fmaxf(mCurr, mMin);
            
                if (mCurr == mFinal) {
                    mFinished = true;
                }
                
        }
    }
    else {
        mCurr = mFinal;
        mFinished = true;
    }
    return true;
}


void Scrollbar::startScroll(float start, float d) {
    startScroll(start, d, DEFAULT_DURATION);
}


void Scrollbar::startScroll(float start, float d, int duration) {
    mMode = SCROLL_MODE;
    mFinished = false;
    mDuration = duration;
    mStartTime = app.currentMillis();
    mStart = start;
    mFinal = start + d;
    mDelta = d;
    mDurationReciprocal = 1.0f / (float) mDuration;
}


void Scrollbar::fling(float start, float velocity, float min, float max) {
    mMode = FLING_MODE;
    mFinished = false;
    mCoeff = signum(velocity);//1.0f;
    velocity = hypotf(velocity, velocity);

    mVelocity = velocity;
    mDuration = (int) (1000 * velocity / mDeceleration); // Duration is in milliseconds
    //assert(mDuration > 0);
    mStartTime = app.currentMillis();
    mStart = start;
    
    float totalDistance = ((velocity * velocity) / (2 * mDeceleration));
    
    mMin = min;
    mMax = max;
    
    mFinal = start + totalDistance * mCoeff;
    // Pin to mMinX <= mFinalX <= mMaxX
    mFinal = fminf(mFinal, mMax);
    mFinal = fmaxf(mFinal, mMin);
}


/*
// Constant gravity value, used in the deceleration phase.
const static float INFLEXION = 0.35f; // Tension lines cross at (INFLEXION, 1)
const static float START_TENSION = 0.5f;
const static float END_TENSION = 1.0f;
const static float GRAVITY = 2000.0f;
const static float DECELERATION_RATE = (float) (log(0.78) / log(0.9));
const static float P1 = START_TENSION * INFLEXION;
const static float P2 = 1.0f - END_TENSION * (1.0f - INFLEXION);
const static int NB_SAMPLES = 100;
static float SPLINE_POSITION[NB_SAMPLES + 1];
static float SPLINE_TIME[NB_SAMPLES + 1];
const static int SPLINE = 0;
const static int CUBIC = 1;
const static int BALLISTIC = 2;



// Controls the viscous fluid effect (how much of it).
static const float VISCOUS_FLUID_SCALE = 8.0f;
static float VISCOUS_FLUID_NORMALIZE = 1.0f / viscousFluid(1.0f);
static float VISCOUS_FLUID_OFFSET = 1.0f - VISCOUS_FLUID_NORMALIZE * viscousFluid(1.0f);

class ViscousFluidInterpolator : public Interpolator {
public:
    float getInterpolation(float input) {
        const float interpolated = VISCOUS_FLUID_NORMALIZE * viscousFluid(input);
        if (interpolated > 0) {
            return interpolated + VISCOUS_FLUID_OFFSET;
        }
        return interpolated;
    }
};
*/


/** Overscroller */
/*
OverScroller::OverScroller(Interpolator* interpolator, bool flywheel) {
    if (!interpolator) {
        mInterpolator = new ViscousFluidInterpolator();
    } else {
        mInterpolator = interpolator;
    }
    mFlywheel = flywheel;
    mScrollerX = new SplineOverScroller();
    mScrollerY = new SplineOverScroller();
}
OverScroller::OverScroller(Interpolator* interpolator, float bounceCoefficientX, float bounceCoefficientY)
    : OverScroller(interpolator, true) {
}
OverScroller::OverScroller(Interpolator* interpolator, float bounceCoefficientX, float bounceCoefficientY, bool flywheel)
    : OverScroller(interpolator, flywheel) {
}
void OverScroller::setInterpolator(Interpolator* interpolator) {
    if (!interpolator) {
        mInterpolator = new ViscousFluidInterpolator();
    } else {
        mInterpolator = interpolator;
    }
}

void OverScroller::setFriction(float friction) {
    mScrollerX->setFriction(friction);
    mScrollerY->setFriction(friction);
}

bool OverScroller::isFinished() {
    return mScrollerX->mFinished && mScrollerY->mFinished;
}

void OverScroller::forceFinished(bool finished) {
    mScrollerX->mFinished = mScrollerY->mFinished = finished;
}

int OverScroller::getCurrX() {
    return mScrollerX->mCurr;
}
int OverScroller::getCurrY() {
    return mScrollerY->mCurr;
}

float OverScroller::getCurrVelocity() {
    return (float)hypot(mScrollerX->mCurrVelocity, mScrollerY->mCurrVelocity);
}

int OverScroller::getStartX() {
    return mScrollerX->mStart;
}

int OverScroller::getStartY() {
    return mScrollerY->mStart;
}

int OverScroller::getFinalX() {
    return mScrollerX->mFinal;
}

int OverScroller::getFinalY() {
    return mScrollerY->mFinal;
}

bool OverScroller::computeScrollOffset() {
    if (isFinished()) {
        return false;
    }
    switch (mMode) {
        case SCROLL_MODE: {
            long time = oakCurrentMillis();
            // Any scroller can be used for time, since they were started
            // together in scroll mode. We use X here.
            const long elapsedTime = time - mScrollerX->mStartTime;
            const int duration = mScrollerX->mDuration;
            if (elapsedTime < duration) {
                const float q = mInterpolator->getInterpolation(elapsedTime / (float) duration);
                mScrollerX->updateScroll(q);
                mScrollerY->updateScroll(q);
            } else {
                abortAnimation();
            }
        }
            break;
        case FLING_MODE:
            if (!mScrollerX->mFinished) {
                if (!mScrollerX->update()) {
                    if (!mScrollerX->continueWhenFinished()) {
                        mScrollerX->finish();
                    }
                }
            }
            if (!mScrollerY->mFinished) {
                if (!mScrollerY->update()) {
                    if (!mScrollerY->continueWhenFinished()) {
                        mScrollerY->finish();
                    }
                }
            }
            break;
    }
    return true;
}

void OverScroller::startScroll(int startX, int startY, int dx, int dy) {
    startScroll(startX, startY, dx, dy, DEFAULT_DURATION);
}
void OverScroller::startScroll(int startX, int startY, int dx, int dy, int duration) {
    mMode = SCROLL_MODE;
    mScrollerX->startScroll(startX, dx, duration);
    mScrollerY->startScroll(startY, dy, duration);
}

bool OverScroller::springBack(int startX, int startY, int minX, int maxX, int minY, int maxY) {
    mMode = FLING_MODE;
    // Make sure both methods are called.
    const bool spingbackX = mScrollerX->springback(startX, minX, maxX);
    const bool spingbackY = mScrollerY->springback(startY, minY, maxY);
    return spingbackX || spingbackY;
}

void OverScroller::fling(int startX, int startY, int velocityX, int velocityY,
                      int minX, int maxX, int minY, int maxY) {
    fling(startX, startY, velocityX, velocityY, minX, maxX, minY, maxY, 0, 0);
}

void OverScroller::fling(int startX, int startY, int velocityX, int velocityY,
                      int minX, int maxX, int minY, int maxY, int overX, int overY) {
    // Continue a scroll or fling in progress
    if (mFlywheel && !isFinished()) {
        float oldVelocityX = mScrollerX->mCurrVelocity;
        float oldVelocityY = mScrollerY->mCurrVelocity;
        if (signum(velocityX) == signum(oldVelocityX) &&
            signum(velocityY) == signum(oldVelocityY)) {
            velocityX += oldVelocityX;
            velocityY += oldVelocityY;
        }
    }
    mMode = FLING_MODE;
    mScrollerX->fling(startX, velocityX, minX, maxX, overX);
    mScrollerY->fling(startY, velocityY, minY, maxY, overY);
}

void OverScroller::notifyHorizontalEdgeReached(int startX, int constX, int overX) {
    mScrollerX->notifyEdgeReached(startX, constX, overX);
}

void OverScroller::notifyVerticalEdgeReached(int startY, int constY, int overY) {
    mScrollerY->notifyEdgeReached(startY, constY, overY);
}

bool OverScroller::isOverScrolled() {
        return ((!mScrollerX->mFinished &&
                 mScrollerX->mState != SPLINE) ||
                (!mScrollerY->mFinished &&
                 mScrollerY->mState != SPLINE));
}

void OverScroller::abortAnimation() {
    mScrollerX->finish();
    mScrollerY->finish();
}

int OverScroller::timePassed() {
    const long time = oakCurrentMillis();
    const long startTime = min(mScrollerX->mStartTime, mScrollerY->mStartTime);
    return (int) (time - startTime);
}

bool OverScroller::isScrollingInDirection(float xvel, float yvel) {
    const int dx = mScrollerX->mFinal - mScrollerX->mStart;
    const int dy = mScrollerY->mFinal - mScrollerY->mStart;
    return !isFinished() && signum(xvel) == signum(dx) && signum(yvel) == signum(dy);
}
*/

/*
static int staticInit() {
    float x_min = 0.0f;
    float y_min = 0.0f;
    for (int i = 0; i < NB_SAMPLES; i++) {
        const float alpha = (float) i / NB_SAMPLES;
        float x_max = 1.0f;
        float x, tx, coef;
        while (true) {
            x = x_min + (x_max - x_min) / 2.0f;
            coef = 3.0f * x * (1.0f - x);
            tx = coef * ((1.0f - x) * P1 + x * P2) + x * x * x;
            if (abs(tx - alpha) < 1E-5) break;
            if (tx > alpha) x_max = x;
            else x_min = x;
        }
        SPLINE_POSITION[i] = coef * ((1.0f - x) * START_TENSION + x) + x * x * x;
        float y_max = 1.0f;
        float y, dy;
        while (true) {
            y = y_min + (y_max - y_min) / 2.0f;
            coef = 3.0f * y * (1.0f - y);
            dy = coef * ((1.0f - y) * START_TENSION + y) + y * y * y;
            if (abs(dy - alpha) < 1E-5) break;
            if (dy > alpha) y_max = y;
            else y_min = y;
        }
        SPLINE_TIME[i] = coef * ((1.0f - y) * P1 + y * P2) + y * y * y;
    }
    SPLINE_POSITION[NB_SAMPLES] = SPLINE_TIME[NB_SAMPLES] = 1.0f;
    return 0;
}

static int _unused = staticInit();

void SplineOverScroller::setFriction(float friction) {
    mFlingFriction = friction;
}


SplineOverScroller::SplineOverScroller() {
    mState = SPLINE;
    mFlingFriction = SCROLL_FRICTION;
    mFinished = true;
    float ppi = app._window->_scale * 160.0f;
    mPhysicalCoeff = 9.80665 // SensorManager.GRAVITY_EARTH // g (m/s^2)
    * 39.37f // inch/meter
    * ppi
    * 0.84f; // look and feel tuning
}
void SplineOverScroller::updateScroll(float q) {
    mCurr = mStart + round(q * (mFinal - mStart));
}

static float getDeceleration(int velocity) {
    return velocity > 0 ? -GRAVITY : GRAVITY;
}

void SplineOverScroller::adjustDuration(int start, int oldconst, int newconst) {
    const int oldDistance = oldconst - start;
    const int newDistance = newconst - start;
    const float x = abs((float) newDistance / oldDistance);
    const int index = (int) (NB_SAMPLES * x);
    if (index < NB_SAMPLES) {
        const float x_inf = (float) index / NB_SAMPLES;
        const float x_sup = (float) (index + 1) / NB_SAMPLES;
        const float t_inf = SPLINE_TIME[index];
        const float t_sup = SPLINE_TIME[index + 1];
        const float timeCoef = t_inf + (x - x_inf) / (x_sup - x_inf) * (t_sup - t_inf);
        mDuration *= timeCoef;
    }
}
void SplineOverScroller::startScroll(int start, int distance, int duration) {
    mFinished = false;
    mCurr = mStart = start;
    mFinal = start + distance;
    mStartTime = oakCurrentMillis();
    mDuration = duration;
    // Unused
    mDeceleration = 0.0f;
    mVelocity = 0;
}
void SplineOverScroller::finish() {
    mCurr = mFinal;
    mFinished = true;
}
void SplineOverScroller::setFinalPosition(int position) {
    mFinal = position;
    mFinished = false;
}
void SplineOverScroller::extendDuration(int extend) {
    const long time = oakCurrentMillis();
    const int elapsedTime = (int) (time - mStartTime);
    mDuration = elapsedTime + extend;
    mFinished = false;
}
bool SplineOverScroller::springback(int start, int min, int max) {
    mFinished = true;
    mCurr = mStart = mFinal = start;
    mVelocity = 0;
    mStartTime = oakCurrentMillis();
    mDuration = 0;
    if (start < min) {
        startSpringback(start, min, 0);
    } else if (start > max) {
        startSpringback(start, max, 0);
    }
    return !mFinished;
}
void SplineOverScroller::startSpringback(int start, int end, int velocity) {
    // mStartTime has been set
    mFinished = false;
    mState = CUBIC;
    mCurr = mStart = start;
    mFinal = end;
    const int delta = start - end;
    mDeceleration = getDeceleration(delta);
    // TODO take velocity into account
    mVelocity = -delta; // only sign is used
    mOver = abs(delta);
    mDuration = (int) (1000.0 * sqrt(-2.0 * delta / mDeceleration));
}
void SplineOverScroller::fling(int start, int velocity, int min, int max, int over) {
    mOver = over;
    mFinished = false;
    mCurrVelocity = mVelocity = velocity;
    mDuration = mSplineDuration = 0;
    mStartTime = oakCurrentMillis();
    mCurr = mStart = start;
    if (start > max || start < min) {
        startAfterEdge(start, min, max, velocity);
        return;
    }
    mState = SPLINE;
    double totalDistance = 0.0;
    if (velocity != 0) {
        mDuration = mSplineDuration = getSplineFlingDuration(velocity);
        totalDistance = getSplineFlingDistance(velocity);
    }
    mSplineDistance = (int) (totalDistance * signum(velocity));
    mFinal = start + mSplineDistance;
    // Clamp to a valid const position
    if (mFinal < min) {
        adjustDuration(mStart, mFinal, min);
        mFinal = min;
    }
    if (mFinal > max) {
        adjustDuration(mStart, mFinal, max);
        mFinal = max;
    }
}
double SplineOverScroller::getSplineDeceleration(int velocity) {
    return log(INFLEXION * abs(velocity) / (mFlingFriction * mPhysicalCoeff));
}
double SplineOverScroller::getSplineFlingDistance(int velocity) {
    const double l = getSplineDeceleration(velocity);
    const double decelMinusOne = DECELERATION_RATE - 1.0;
    return mFlingFriction * mPhysicalCoeff * exp(DECELERATION_RATE / decelMinusOne * l);
}

int SplineOverScroller::getSplineFlingDuration(int velocity) {
    const double l = getSplineDeceleration(velocity);
    const double decelMinusOne = DECELERATION_RATE - 1.0;
    return (int) (1000.0 * exp(l / decelMinusOne));
}

void SplineOverScroller::fitOnBounceCurve(int start, int end, int velocity) {
    // Simulate a bounce that started from edge
    const float durationToApex = - velocity / mDeceleration;
    // The float cast below is necessary to avoid integer overflow.
    const float velocitySquared = (float) velocity * velocity;
    const float distanceToApex = velocitySquared / 2.0f / abs(mDeceleration);
    const float distanceToEdge = abs(end - start);
    const float totalDuration = (float) sqrt(2.0 * (distanceToApex + distanceToEdge) / abs(mDeceleration));
    mStartTime -= (int) (1000.0f * (totalDuration - durationToApex));
    mCurr = mStart = end;
    mVelocity = (int) (- mDeceleration * totalDuration);
}

void SplineOverScroller::startBounceAfterEdge(int start, int end, int velocity) {
    mDeceleration = getDeceleration(velocity == 0 ? start - end : velocity);
    fitOnBounceCurve(start, end, velocity);
    onEdgeReached();
}
void SplineOverScroller::startAfterEdge(int start, int min, int max, int velocity) {
    if (start > min && start < max) {
        mFinished = true;
        return;
    }
    const bool positive = start > max;
    const int edge = positive ? max : min;
    const int overDistance = start - edge;
    bool keepIncreasing = overDistance * velocity >= 0;
    if (keepIncreasing) {
        // Will result in a bounce or a to_boundary depending on velocity.
        startBounceAfterEdge(start, edge, velocity);
    } else {
        const double totalDistance = getSplineFlingDistance(velocity);
        if (totalDistance > abs(overDistance)) {
            fling(start, velocity, positive ? min : start, positive ? start : max, mOver);
        } else {
            startSpringback(start, edge, velocity);
        }
    }
}
void SplineOverScroller::notifyEdgeReached(int start, int end, int over) {
    // mState is used to detect successive notifications
    if (mState == SPLINE) {
        mOver = over;
        mStartTime = oakCurrentMillis();
        // We were in fling/scroll mode before: current velocity is such that distance to
        // edge is increasing. This ensures that startAfterEdge will not start a new fling.
        startAfterEdge(start, end, end, (int) mCurrVelocity);
    }
}
void SplineOverScroller::onEdgeReached() {
    // mStart, mVelocity and mStartTime were adjusted to their values when edge was reached.
    // The float cast below is necessary to avoid integer overflow.
    const float velocitySquared = (float) mVelocity * mVelocity;
    float distance = velocitySquared / (2.0f * abs(mDeceleration));
    const float sign = signum(mVelocity);
    if (distance > mOver) {
        // Default deceleration is not sufficient to slow us down before boundary
        mDeceleration = - sign * velocitySquared / (2.0f * mOver);
        distance = mOver;
    }
    mOver = (int) distance;
    mState = BALLISTIC;
    mFinal = mStart + (int) (mVelocity > 0 ? distance : -distance);
    mDuration = - (int) (1000.0f * mVelocity / mDeceleration);
}
bool SplineOverScroller::continueWhenFinished() {
    switch (mState) {
        case SPLINE:
            // Duration from start to null velocity
            if (mDuration < mSplineDuration) {
                // If the animation was clamped, we reached the edge
                mCurr = mStart = mFinal;
                // TODO Better compute speed when edge was reached
                mVelocity = (int) mCurrVelocity;
                mDeceleration = getDeceleration(mVelocity);
                mStartTime += mDuration;
                onEdgeReached();
            } else {
                // Normal stop, no need to continue
                return false;
            }
            break;
        case BALLISTIC:
            mStartTime += mDuration;
            startSpringback(mFinal, mStart, 0);
            break;
        case CUBIC:
            return false;
    }
    update();
    return true;
}

bool SplineOverScroller::update() {
    const long time = oakCurrentMillis();
    const long currentTime = time - mStartTime;
    if (currentTime == 0) {
        // Skip work but report that we're still going if we have a nonzero duration.
        return mDuration > 0;
    }
    if (currentTime > mDuration) {
        return false;
    }
    double distance = 0.0;
    switch (mState) {
        case SPLINE: {
            const float t = (float) currentTime / mSplineDuration;
            const int index = (int) (NB_SAMPLES * t);
            float distanceCoef = 1.f;
            float velocityCoef = 0.f;
            if (index < NB_SAMPLES) {
                const float t_inf = (float) index / NB_SAMPLES;
                const float t_sup = (float) (index + 1) / NB_SAMPLES;
                const float d_inf = SPLINE_POSITION[index];
                const float d_sup = SPLINE_POSITION[index + 1];
                velocityCoef = (d_sup - d_inf) / (t_sup - t_inf);
                distanceCoef = d_inf + (t - t_inf) * velocityCoef;
            }
            distance = distanceCoef * mSplineDistance;
            mCurrVelocity = velocityCoef * mSplineDistance / mSplineDuration * 1000.0f;
            break;
        }
        case BALLISTIC: {
            const float t = currentTime / 1000.0f;
            mCurrVelocity = mVelocity + mDeceleration * t;
            distance = mVelocity * t + mDeceleration * t * t / 2.0f;
            break;
        }
        case CUBIC: {
            const float t = (float) (currentTime) / mDuration;
            const float t2 = t * t;
            const float sign = signum(mVelocity);
            distance = sign * mOver * (3.0f * t2 - 2.0f * t * t2);
            mCurrVelocity = sign * mOver * 6.0f * (- t + t2);
            break;
        }
    }
    mCurr = mStart + (int)round(distance);
    return true;
}

*/

