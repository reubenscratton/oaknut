//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


Window::Window() : _rootViewController(NULL), _scale(1) {
}

void Window::show() {
}


void Window::setRootViewController(ViewController* viewController) {
    if (viewController == _rootViewController) {
        return;
    }
    ViewController *currentRootVC = _rootViewController;
    if (currentRootVC) {
        currentRootVC->onWillPause();
        currentRootVC->detachFromWindow();
        currentRootVC->onDidPause();
        _viewControllers.erase(_viewControllers.begin());
    }
    _rootViewController = viewController;
    _viewControllers.insert(_viewControllers.begin(), viewController);
    attachViewController(_rootViewController);
}


void Window::attachViewController(ViewController *vc) {
	if (_surface && !vc->_window) {
        vc->onWillResume();
		prepareToDraw();
        vc->attachToWindow(this);
        updateSafeArea();
        vc->onDidResume();
	}
}
void Window::detachViewController(ViewController* vc) {
    if (vc->_window) {
        vc->detachFromWindow();
    }
}

void Window::resizeSurface(int width, int height, float scale) {
    if (!_surface) {
        _surface = new Surface();
        _quadBuffer = new QuadBuffer();
    }
    if (_surface->_size.width==width && _surface->_size.height==height) {
        return;
    }
	//app.log("Window::resize %d %d %d", width, height, scale);
    _surfaceRect = RECT(0,0,width,height);
	_scale = scale;
    _surface->setSize({(float)width, (float)height});
    for (auto vc : _viewControllers) {
        if (!vc->_window) {
            attachViewController(vc);
        }
        updateSafeArea();
        vc->getView()->setNeedsLayout();
	}
}
void Window::destroySurface() {
    if (_surface) {
        for (auto vc : _viewControllers) {
            detachViewController(vc);
        }
        _surface = NULL;
        _doneGlInit = false;
        //delete _quadBuffer; todo: fix this leak
    }
    for (auto bitmap : _loadedTextures) {
        bitmap->onRenderContextDestroyed();
    }
    _loadedTextures.clear();
    for (auto prog : _loadedProgs) {
        prog->unload();
    }
    _loadedProgs.clear();
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

	prepareToDraw();

	// Draw view controllers
    for (ViewController* vc : _viewControllers) {
		View* view = vc->getView();
		if (!view->_layoutValid) {
			view->measure(_surfaceRect.size.width, _surfaceRect.size.height);
			view->layout();
            ensureFocusedViewIsInSafeArea();
		}
        _surface->render(view, this);
	}

	
	incFrames();

    // Tick animations
    TIMESTAMP now = app.currentMillis();
    auto it = _animations.begin();
    _animationsModified = false;
    while (it != _animations.end()) {
        ObjPtr<Animation> anim = *it++;
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
    }
    return true;
}




void Window::setBlendMode(int blendMode) {
    if (blendMode != _blendMode) {
        if (blendMode == BLENDMODE_NONE) {
            check_gl(glDisable, GL_BLEND);
        } else {
            if (blendMode == BLENDMODE_NORMAL) {
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // normal alpha blend
            } else {
                glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
            }
            if (_blendMode == BLENDMODE_NONE) {
                check_gl(glEnable, GL_BLEND);
            }
        }
        _blendMode = blendMode;
    }
}

void Window::pushClip(RECT clip) {
    bool firstClip = _clips.size()==0;
    if (!firstClip) {
        clip.intersectWith(_clips.top());
    } else {
        check_gl(glEnable, GL_SCISSOR_TEST);
    }
    _clips.push(clip);
    glScissor(clip.left(), clip.top(), clip.size.width, clip.size.height);
}
void Window::popClip() {
    assert(_clips.size()>0);
    RECT clip = _clips.top();
    _clips.pop();
    glScissor(clip.left(), clip.top(), clip.size.width, clip.size.height);
    if (_clips.size() == 0) {
        check_gl(glDisable, GL_SCISSOR_TEST);
    }

}

void Window::prepareToDraw() {
    // GL context init
    if (!_doneGlInit) {
        _doneGlInit = 1;
        check_gl(glDepthMask, GL_TRUE);
        check_gl(glClear, GL_DEPTH_BUFFER_BIT);
        check_gl(glDepthMask, GL_FALSE);
        check_gl(glDisable, GL_DEPTH_TEST);
        check_gl(glActiveTexture, GL_TEXTURE0);
        _blendMode = BLENDMODE_NONE;
        check_gl(glDisable, GL_BLEND);
        //_enabledFlags.scissorTest = 0;
        check_gl(glDisable, GL_SCISSOR_TEST);

        // As long as we only have one quadbuffer we only need to bind the once
        _quadBuffer->bind();
        _currentVertexConfig = 0;

        COLOR windowBackgroundColor = app.getStyleColor("window.background-color");
        _backgroundColor[0] = (windowBackgroundColor & 0xFF) / 255.0f;
        _backgroundColor[1] = ((windowBackgroundColor & 0xFF00)>>8) / 255.0f;
        _backgroundColor[2] = ((windowBackgroundColor & 0xFF0000)>>16) / 255.0f;
        _backgroundColor[3] = ((windowBackgroundColor & 0xFF000000)>>24) / 255.0f;
        check_gl(glClearColor, _backgroundColor[0],_backgroundColor[1],_backgroundColor[2],_backgroundColor[3]);
    }
    if (_backgroundColor[3] > 0.f) {
        check_gl(glClear, GL_COLOR_BUFFER_BIT);
    }

    _renderCounter++;
    _currentSurface = NULL;
    _currentTexture = NULL;
    _currentProg = 0;
}

void Window::setCurrentSurface(Surface* surface) {
    if (surface != _currentSurface) {
        _currentSurface = surface;
        _currentSurface->use();
    }
}

void Window::setVertexConfig(int vertexConfig) {
    _currentVertexConfig = vertexConfig;
    check_gl(glVertexAttribPointer, VERTEXATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), 0);
    check_gl(glVertexAttribPointer, VERTEXATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)8);
//    check_gl(glVertexAttribPointer, VERTEXATTRIB_TEXCOORD, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(VERTEX), (void*)8);
    check_gl(glVertexAttribPointer, VERTEXATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VERTEX), (void*)16);

    check_gl(glEnableVertexAttribArray, VERTEXATTRIB_POSITION);
    check_gl(glEnableVertexAttribArray, VERTEXATTRIB_TEXCOORD);
    check_gl(glEnableVertexAttribArray, VERTEXATTRIB_COLOR);

}

void Window::bindTexture(Bitmap* texture) {
    if (texture&& _currentTexture != texture) {
        _currentTexture = texture;
        texture->bind();
    }
}

RECT Window::getSafeArea() {
    RECT safeArea = _surfaceRect;
    EDGEINSETS safeInsets = _safeAreaInsets;
    safeInsets.bottom += _softKeyboardRect.size.height;
    safeInsets.applyToRect(safeArea);
    return safeArea;
}

void Window::updateSafeArea() {
    RECT safeArea = getSafeArea();
    for (auto vc : _viewControllers) {
        if (vc->_window) {
            vc->updateSafeArea(safeArea);
        }
    }
    ensureFocusedViewIsInSafeArea();
}

void Window::setSoftKeyboardRect(const RECT rect) {
    _softKeyboardRect = rect;
    updateSafeArea();
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
    _viewControllers.push_back(viewController);
    attachViewController(viewController);
    requestRedraw();
}
void Window::dismissModalViewController(ViewController* viewController) {
    auto currentTop = _viewControllers.rbegin();
    assert(*currentTop == viewController);
    detachViewController(viewController);
    _viewControllers.pop_back();
    requestRedraw();
}

void Window::ensureFocusedViewIsInSafeArea() {
    if (_focusedView != nullptr) {
        
        // Get the focused view's window rect
        RECT rect = _focusedView->getOwnRect();
        rect.origin = _focusedView->mapPointToWindow(rect.origin);
        
        // Calculate the extent it is below the safe area
        float dx=0,dy=0;
        RECT safeArea = getSafeArea();
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
