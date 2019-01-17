//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


Window::Window() : _rootViewController(NULL), _scale(1) {
    _renderer = Renderer::create(this);
    _surface = _renderer->getPrimarySurface();
    _window = this;
    _effectiveAlpha = 1;
}

void Window::show() {
}


void Window::setRootViewController(ViewController* viewController) {
    if (viewController == _rootViewController) {
        return;
    }
    if (_rootViewController) {
        detachViewController(_rootViewController);
    }
    _rootViewController = viewController;
    attachViewController(_rootViewController);
}


void Window::attachViewController(ViewController* viewController) {
    assert(!viewController->getView()->getWindow());
    _viewControllers.push_back(viewController);
    addSubview(viewController->getView());
    viewController->applySafeInsets(_safeInsetsTotal);
    viewController->onWindowAttached();
}
void Window::detachViewController(ViewController* viewController) {
    assert(viewController->getView()->getWindow() == this);
    removeSubview(viewController->getView());
    for (int i=0 ; i<_viewControllers.size() ; i++) {
        if (_viewControllers[i] == viewController) {
            _viewControllers.erase(_viewControllers.begin()+i);
            break;
        }
    }
    viewController->onWindowDetached();
}

void Window::layout(RECT constraint) {
    _rect = constraint;
    layoutSubviews(_rect);
}
void Window::resizeSurface(int width, int height, float scale) {
    if (_surface->_size.width==width && _surface->_size.height==height) {
        return;
    }
	//app.log("Window::resize %d %d %f", width, height, scale);
    _rect = RECT(0,0,width,height);
	_scale = scale;
    _surface->setSize({(float)width, (float)height});
    updateDecorOp(false, _safeInsetsTotal.top);
    updateDecorOp(true, _safeInsetsTotal.bottom);
    for (auto vc : _viewControllers) {
        vc->applySafeInsets(_safeInsetsTotal);
    }
    setNeedsLayout();
}
void Window::destroySurface() {
    _renderer->reset();
}

Window::MotionTracker::MotionTracker(int source) {
    this->source = source;
    isDragging = false;
    touchedView = NULL;
    timeOfDownEvent = 0;
    pastIndex = pastCount = 0;
    numClicks = 0;
}

void Window::MotionTracker::dispatchInputEvent(INPUTEVENT& event, ViewController* topVC) {
    if (touchedView && !touchedView->_window) {
        touchedView = NULL; // avoid sending events to detached views
    }
    if (event.type == INPUT_EVENT_DOWN) {
        isDragging = false;
        if (multiclickTimer) {
            multiclickTimer->stop();
            multiclickTimer = NULL;
            numClicks++;
        }
        pastIndex = pastCount = 0;
        ptDown = event.pt;
        timeOfDownEvent = event.time;
        touchedView = topVC->getView()->dispatchInputEvent(&event);
        if (!touchedView) {
            touchedView = topVC->getView();
        }
        if (touchedView && event.deviceType!=INPUTEVENT::ScrollWheel) {
            _didSendLongpressEvent = false;
            _longpressTimer = Timer::start([=] {
                if (touchedView && touchedView->_window) {
                    INPUTEVENT lpEv = event;
                    lpEv.type = INPUT_EVENT_LONG_PRESS;
                    lpEv.pt += touchedView->_surfaceOrigin;
                    touchedView->dispatchInputEvent(&lpEv);
                    _didSendLongpressEvent = true;
                }
                _longpressTimer = NULL;
            }, LONG_PRESS_THRESHOLD, false);
        }
        pastTime[pastIndex] = event.time;
        pastPts[pastIndex] = (event.deviceType==INPUTEVENT::ScrollWheel) ? event.delta : event.pt;
        pastIndex = (pastIndex + 1) % NUM_PAST;
        pastCount++;
    }

    if (event.type == INPUT_EVENT_MOVE) {
        
        // Filter out spurious move events (seen on iOS 10)
        if (event.deviceType != INPUTEVENT::ScrollWheel) {
            int prevIndex = pastIndex - 1;
            if (prevIndex < 0) prevIndex += NUM_PAST;
            if (pastPts[prevIndex].equals(event.pt)) {
                pastTime[prevIndex] = event.time;
                return;
            }
        }
        pastTime[pastIndex] = event.time;
        pastPts[pastIndex] = (event.deviceType==INPUTEVENT::ScrollWheel) ? event.delta : event.pt;
        pastIndex = (pastIndex + 1) % NUM_PAST;
        pastCount++;
        if (timeOfDownEvent && !isDragging && event.deviceType!=INPUTEVENT::ScrollWheel) {
            float dx = event.pt.x - ptDown.x;
            float dy = event.pt.y - ptDown.y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (app.idp(dist) >= TOUCH_SLOP) {
                if (touchedView) {
                    INPUTEVENT cancelEvent = event;
                    cancelEvent.type = INPUT_EVENT_CANCEL;
                    touchedView->dispatchInputEvent(&cancelEvent);
                }
                isDragging = true;
                INPUTEVENT dragEvent = event;
                dragEvent.type = INPUT_EVENT_DRAG;
                View *interceptView = topVC->getView()->dispatchInputEvent(&dragEvent);
                if (interceptView) {
                    touchedView = interceptView;
                }
                if (_longpressTimer) {
                    _longpressTimer->stop();
                    _longpressTimer = NULL;
                }
            }
        }
        if (touchedView) {
            touchedView->dispatchInputEvent(&event);
        }
    } else if (event.type == INPUT_EVENT_UP) {
        if (_longpressTimer) {
            _longpressTimer->stop();
            _longpressTimer = NULL;
        }
        if (touchedView) {
            touchedView->dispatchInputEvent(&event);
        }
            if (!_didSendLongpressEvent) {
                if (!isDragging && (event.deviceType!=INPUTEVENT::ScrollWheel)) {
                    // TAP!
                    if (touchedView) {
                        INPUTEVENT tapEvent = event;
                        tapEvent.type = INPUT_EVENT_TAP;
                        touchedView->dispatchInputEvent(&tapEvent);
                        app.log("tap %d", numClicks);
                    }
                    multiclickTimer = Timer::start([=] {
                        if (touchedView && touchedView->_window) {
                            INPUTEVENT tapEvent = event;
                            tapEvent.type = INPUT_EVENT_TAP_CONFIRMED;
                            touchedView->dispatchInputEvent(&tapEvent);
                        }
                        multiclickTimer = NULL;
                        app.log("tap confirmed at %d", numClicks);
                        numClicks = 0;
                        touchedView = NULL;
                    }, MULTI_CLICK_THRESHOLD, false);
                } else {
                    // FLING!

                    // Work out the drag velocity by getting the distance travelled from the oldest historical point
                    // and extrapolating an average distance per second.
                    int numPoints = MIN(pastCount, NUM_PAST);
                    float dx=0.f,dy=0.f;
                    int dTime = 1;
                    for (int i=1 ; i<=numPoints ; i++) {
                        int index = pastIndex - i;
                        if (index < 0) index += NUM_PAST;
                        POINT ptFrom = pastPts[index];
                        TIMESTAMP timeFrom = pastTime[index];
                        if (event.time - timeFrom >= 333) { // ignore historical points that are too old
                            continue;
                        }
                        dTime = (int) (event.time - timeFrom);
                        if (dTime <= 0) continue;
                        if (event.deviceType == INPUTEVENT::ScrollWheel) {
                            dx += ptFrom.x;
                            dy += ptFrom.y;
                        } else {
                            dx = event.pt.x - ptFrom.x;
                            dy = event.pt.y - ptFrom.y;
                        }
                    }
                    float thisVeloX = dx * 1000.0f / dTime;
                    float thisVeloY = dy * 1000.0f / dTime;
                    POINT velocity = {0, 0};
                    velocity.x = (velocity.x == 0) ? thisVeloX : ((velocity.x + thisVeloX) / 2);
                    velocity.y = (velocity.y == 0) ? thisVeloY : ((velocity.y + thisVeloY) / 2);
                    if (touchedView) {
                        INPUTEVENT flingEvent = event;
                        flingEvent.type = INPUT_EVENT_FLING;
                        flingEvent.velocity = velocity;
                        touchedView->dispatchInputEvent(&flingEvent);
                    }
                }
        }
        //touchedView = NULL;
        timeOfDownEvent = 0;

    }
}

void Window::dispatchInputEvent(INPUTEVENT event) {

    //if (event->deviceType == INPUTEVENT::Mouse || event->deviceType == INPUTEVENT::Touch) {

        // Start or lookup motion tracker
        MotionTracker *tracker = NULL;
        for (auto it=_motionTrackers.begin() ; it!=_motionTrackers.end() ; it++) {
            if ((*it)->source == event.deviceIndex) {
                tracker = *it;
                break;
            }
        }
        if (!tracker) {
            tracker = new MotionTracker(event.deviceIndex);
            //glInsertEventMarkerEXT(0, "com.apple.GPUTools.event.debug-frame");
            _motionTrackers.push_back(tracker);
        }

        // Let the tracker process the new input event
        tracker->dispatchInputEvent(event,  *_viewControllers.rbegin());

    //}
}

static int numFrames;
static time_t timeBase;

void incFrames() {
    time_t now = time(NULL);
    if (now == timeBase) {
        numFrames++;
    } else {
#ifdef EMSCRIPTEN
		EM_ASM_({
			updateFps($0);
		}, numFrames);
#endif
        printf("%d fps\n", numFrames);
        timeBase = now;
        numFrames = 1;
    }
}

void Window::draw() {
    _redrawNeeded = false;

	_renderer->prepareToDraw();

    if (!_layoutValid) {
        _layoutValid = true;
        layout(_rect);
        ensureFocusedViewIsInSafeArea();
    }
    _surface->render(this, _renderer);

    _renderer->commit();
	
	incFrames();

    // Tick animations
    TIMESTAMP now = app.currentMillis();
    auto it = _animations.begin();
    _animationsModified = false;
    while (it != _animations.end()) {
        sp<Animation> anim = *it++;
        if (!anim->_paused) {
            bool stillGoing = anim->tick(now);
            if (!stillGoing) {
                stopAnimation(anim);
            }
            if (_animationsModified) {
                _animationsModified = false;
                it = _animations.begin(); // bit crap
            }
        }
    }
    
    Object::flushAutodeletePool();

    // If there are any animations running, request a redraw immediately
    if (_animations.size()) {
        requestRedraw();
    }
    
}

void Window::startAnimation(Animation* animation, int duration) {
    startAnimation(animation, duration, 0);
}
void Window::startAnimation(Animation* animation, int duration, int delay) {
    if (animation->_window) {
        return;
    }
#if CONFIG_SLOW_ANIMATIONS
    duration *= 10;
#endif
    animation->_duration = duration;
    animation->_delay = delay;
    animation->_paused = false;
    animation->_timeStarted = app.currentMillis();
    animation->_window = this;
    animation->_windowAnimationsListIterator = _animations.insert(_animations.end(), animation);
    if (animation->_view) {
        animation->_view->_animationCount++;
    }
    
    // If animation has zero duration then it's effectively finished before it starts.
    // Applying the final value now rather than on next frame avoids unwanted in-between frames.
    if (delay<=0 && duration<=0) {
        animation->apply(1.0);
    }
    requestRedraw();
}

void Window::stopAnimation(Animation* animation) {
    if (animation->_window) {
        assert(animation->_window == this);
        animation->retain();
        //animation->apply(1.0);
        _animations.erase(animation->_windowAnimationsListIterator);
        _animationsModified = true;
        animation->_window = NULL;
        animation->_timeStarted = 0;
        if (animation->_view) {
            animation->_view->_animationCount--;
        }
        animation->release();
    }
}

void Window::detachView(View* view) {
    if (_textInputReceiver && _textInputReceiver==view->getTextInputReceiver()) {
        setFocusedView(NULL);
        _textInputReceiver = NULL;
        _keyboardHandler = NULL;
    }
    if (view->_animationCount > 0) {
        for (auto it=_animations.begin() ; it!=_animations.end(); ) {
            auto anim = *it++;
            if (anim->_view == view) {
                stopAnimation(anim);
            }
        }
        assert(view->_animationCount == 0);
    }
}

void Window::requestRedraw() {
	if (_redrawNeeded) {
		return;
	}
	_redrawNeeded = true;
    requestRedrawNative();
}

void Window::requestRedrawNative() {
    // no-op
}

POINT Window::offsetToView(View* view) {
	POINT pt = {0,0};
	ViewController* rootVC = _rootViewController;
	while (view && view != rootVC->getView()) {
		pt.x += view->_rect.origin.x;
		pt.y += view->_rect.origin.y;
		view = view->_parent;
	}
	return pt;
}

void Window::keyboardShow(bool show) {
    // no-op
}
void Window::keyboardNotifyTextChanged() {
    // no-op
}
void Window::keyboardNotifyTextSelectionChanged() {
    // no-op
}

bool Window::setFocusedView(View* view) {
    bool settingToExisting = (_focusedView == view);
    if (_focusedView && !settingToExisting) {
        _focusedView->setFocused(false);
    }
    _focusedView = view;
    if (view) {
        if (!settingToExisting) {
            view->setFocused(true);
            _keyboardHandler = view->getKeyboardInputHandler();
            auto newTextInputReceiver = view->getTextInputReceiver();
            if (newTextInputReceiver != _textInputReceiver) {
                _textInputReceiver = newTextInputReceiver;
                keyboardNotifyTextChanged();
            }
        }
        ensureFocusedViewIsInSafeArea();
        keyboardShow(_textInputReceiver != NULL);
    } else {
        keyboardShow(false);
        _keyboardHandler = NULL;
    }
    return true;
}









void Window::setSafeInsets(SafeInsetsType type, const EDGEINSETS& insets) {
    if (_safeInsets[type] != insets) {
        _safeInsets[type] = insets;
        _safeInsetsTotal = {0,0,0,0};
        for (int i=0 ; i<sizeof(_safeInsets)/sizeof(_safeInsets[0]) ; i++) {
            const EDGEINSETS& inset = _safeInsets[i];
            _safeInsetsTotal.left = MAX(_safeInsetsTotal.left, inset.left);
            _safeInsetsTotal.right = MAX(_safeInsetsTotal.right, inset.right);
            _safeInsetsTotal.top = MAX(_safeInsetsTotal.top, inset.top);
            _safeInsetsTotal.bottom = MAX(_safeInsetsTotal.bottom, inset.bottom);
        }
        
        // Window decor
        if (type==SafeInsetsType::BottomNavBar) {
            updateDecorOp(true, insets.bottom);
        }
        if (type==SafeInsetsType::StatusBar) {
            updateDecorOp(false, insets.top);
        }

        
        
        for (auto vc : _viewControllers) {
            vc->applySafeInsets(_safeInsetsTotal);
        }
        ensureFocusedViewIsInSafeArea();
        requestRedraw();
    }
}

void Window::updateDecorOp(bool bottom, float height) {
    RectRenderOp*& op = bottom?_renderOpDecorBottom:_renderOpDecorTop;
    COLOR color = app.getStyleColor(bottom ? "window.safeInsetBackgrounds.bottom" : "window.safeInsetBackgrounds.top");
    if (color!=0 && height>0) {
        if (!op) {
            op = new RectRenderOp();
            op->setFillColor(color);
            addDecorOp(op);
        }
        RECT rect = {0,bottom?(_rect.size.height-height):0,_rect.size.width, height};
        op->setRect(rect);
    } else  {
        if (op) {
            removeDecorOp(op);
            op = NULL;
        }
    }
}



// Permissions. By default there is no runtime permissions system.
// (NB: Of course iOS and Android implement these differently...)
bool Window::hasPermission(Permission permission) {
    return true;
}
void Window::runWithPermission(Permission permission, std::function<void(bool)> callback) {
    runWithPermissions({permission}, [=](vector<bool> callback2) {
        callback(callback2[0]);
    });
}
void Window::runWithPermissions(vector<Permission> permissions, std::function<void(vector<bool>)> callback) {
    vector<bool> results;
    for (auto p : permissions) {
        results.push_back(true);
    }
    callback(results);
}


void Window::presentModalViewController(ViewController *viewController) {
    
    // Fade in a 'scrim' view that also prevents the UI underneath being touchable
    View* scrimView = new View();
    scrimView->setMeasureSpecs(MEASURESPEC::Fill(), MEASURESPEC::Fill());
    addSubview(scrimView);
    setNeedsLayout();
    COLOR scrimColor = app.getStyleColor("window.scrim");
    Animation::start(scrimView, 333, [=](float val) {
        scrimView->setBackgroundColor(COLOR::interpolate(0, scrimColor, val));
    });

    // Attach the new VC and animate it in from below
    attachViewController(viewController);
    viewController->getView()->setTranslate({0, _rect.size.height}); // i.e. start off below screen
    viewController->getView()->animateInFromBottom(333);
}

void Window::dismissModalViewController(ViewController* viewController, std::function<void()> onComplete) {
    auto currentTop = _viewControllers.rbegin();
    assert(*currentTop == viewController);
    View* view = viewController->getView();
    View* scrimView = getSubview(indexOfSubview(view)-1);
    
    // Animate out
    COLOR scrimColor = app.getStyleColor("window.scrim");
    Animation::start(view, 333, [=](float val) {
        scrimView->setBackgroundColor(COLOR::interpolate(scrimColor, 0, val));
        if (val >= 1.0f) {
            if (onComplete) {
                onComplete();
            }
            detachViewController(viewController);
            scrimView->removeFromParent();
            requestRedraw();
        }
    });
    view->animateOutToBottom(333);
    
}

void Window::ensureFocusedViewIsInSafeArea() {
    if (_focusedView != nullptr) {
        
        // Get the focused view's window rect
        RECT rect = _focusedView->getOwnRect();
        rect.origin = _focusedView->mapPointToWindow(rect.origin);
        
        // Calculate the extent it is below the safe area
        float dx=0,dy=0;
        RECT safeArea = _rect;
        _safeInsetsTotal.applyToRect(safeArea);
        float d = rect.bottom() - safeArea.bottom();
        if (d > 0) {
            dy = -d;
        }
        d = rect.top() - safeArea.top();
        if (d < 0) {
            dy = -d;
        }
        d = rect.right() - safeArea.right();
        if (d > 0) {
            dx = -d;
        }
        d = rect.left() - safeArea.left();
        if (d < 0) {
            dx = -d;
        }
        if (dx!=0 || dy!=0) {
            _rootViewController->requestScroll(-dx, -dy);
        }
    }
}
