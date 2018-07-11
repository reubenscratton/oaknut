//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>
//#include <thread>


Window::Window() : _rootViewController(NULL), _scale(1) {
}



void Window::setRootViewController(ViewController* viewController) {
    if (viewController == _rootViewController) {
        return;
    }
    ViewController *currentRootVC = _rootViewController;
    if (currentRootVC) {
        viewController->onWillPause();
        currentRootVC->detachFromWindow();
        viewController->onDidPause();
    }
    _rootVcAttached = false;
    _rootViewController = viewController;
    attachRootVC();
}
void Window::attachRootVC() {
	if (_rootViewController && _surface && !_rootVcAttached) {
        _rootViewController->onWillResume();
		prepareToDraw();
        _rootViewController->attachToWindow(this);
        _rootViewController->onDidResume();
        _rootVcAttached = true;
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
    _surface->setSize(SIZE_Make(width, height));
	if (_rootViewController) {
        if (!_rootVcAttached) {
            attachRootVC();
        }
        _rootViewController->getView()->setNeedsLayout();
	}
}

Window::MotionTracker::MotionTracker(int source) {
    this->source = source;
    isDragging = false;
    touchedView = NULL;
    timeOfDownEvent = 0;
    pastIndex = pastCount = 0;
}

void Window::MotionTracker::dispatchInputEvent(int event, TIMESTAMP time, POINT pt, Window* window) {
    if (touchedView && !touchedView->_window) {
        touchedView = NULL; // avoid sending events to detached views
    }
    if (event == INPUT_EVENT_DOWN) {
        isDragging = false;
        if (multiclickTimer) {
            multiclickTimer->stop();
            multiclickTimer = NULL;
            numClicks++;
        }
        pastIndex = pastCount = 0;
        ptDown = pt;
        timeOfDownEvent = time;
        touchedView = window->_rootViewController->getView()->dispatchInputEvent(INPUT_EVENT_DOWN, source, time, pt);
        if (touchedView) {
            _didSendLongpressEvent = false;
            _longpressTimer = Timer::start([=] {
                if (touchedView && touchedView->_window) {
                    touchedView->dispatchInputEvent(INPUT_EVENT_LONG_PRESS, source, time, pt);
                    _didSendLongpressEvent = true;
                }
                _longpressTimer = NULL;
            }, LONG_PRESS_THRESHOLD, false);
        }
    }
    if (event == INPUT_EVENT_DOWN || event == INPUT_EVENT_MOVE) {
        if (event == INPUT_EVENT_MOVE) { // filter out spurious move events (seen on iOS 10)
            int prevIndex = pastIndex - 1;
            if (prevIndex < 0) prevIndex += NUM_PAST;
            if (pastPts[prevIndex].equals(pt)) {
                pastTime[prevIndex] = time;
                return;
            }
        }
        pastTime[pastIndex] = time;
        pastPts[pastIndex] = pt;
        pastIndex = (pastIndex + 1) % NUM_PAST;
        pastCount++;
    }

    if (event == INPUT_EVENT_MOVE) {
        if (timeOfDownEvent && !isDragging) {
            float dx = pt.x - ptDown.x;
            float dy = pt.y - ptDown.y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (app.idp(dist) >= TOUCH_SLOP) {
                isDragging = true;
                View *interceptView = window->_rootViewController->getView()->dispatchInputEvent(INPUT_EVENT_DRAG, source, time, pt);
                if (interceptView) {
                    if (touchedView) {
                        touchedView->dispatchInputEvent(INPUT_EVENT_CANCEL, source, time, pt);
                    }
                    touchedView = interceptView;
                }
                if (_longpressTimer) {
                    _longpressTimer->stop();
                    _longpressTimer = NULL;
                }
            }
        }
        if (touchedView) {
            touchedView->dispatchInputEvent(INPUT_EVENT_MOVE, source, time, pt);
        }
    } else if (event == INPUT_EVENT_UP) {
        if (_longpressTimer) {
            _longpressTimer->stop();
            _longpressTimer = NULL;
        }
        if (touchedView) {
            touchedView->dispatchInputEvent(INPUT_EVENT_UP, source, time, pt);
        }
        if (!_didSendLongpressEvent) {
            if (!isDragging) {
                // TAP!
                if (touchedView) {
                    touchedView->dispatchInputEvent(INPUT_EVENT_TAP, source, time, pt);
                    app.log("tap %d", numClicks);
                }
                multiclickTimer = Timer::start([=] {
                    if (touchedView && touchedView->_window) {
                        touchedView->dispatchInputEvent(INPUT_EVENT_TAP_CONFIRMED, source, time, pt);
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
                int numPoints = min(pastCount, NUM_PAST);
                int index = pastIndex - numPoints;
                if (index < 0) index += NUM_PAST;
                while (numPoints-- > 1) {
                    POINT ptFrom = pastPts[index];
                    TIMESTAMP timeFrom = pastTime[index];
                    index = (index + 1) % NUM_PAST;
                    if (time - timeFrom >= 333) { // ignore historical points that are too old
                        continue;
                    }
                    int dTime = (int) (time - timeFrom);
                    if (dTime <= 0) continue;
                    float dx = pt.x - ptFrom.x;
                    float dy = pt.y - ptFrom.y;
                    float thisVeloX = dx * 1000.0f / dTime;
                    float thisVeloY = dy * 1000.0f / dTime;
                    POINT velocity = {0, 0};
                    velocity.x = (velocity.x == 0) ? thisVeloX : ((velocity.x + thisVeloX) / 2);
                    velocity.y = (velocity.y == 0) ? thisVeloY : ((velocity.y + thisVeloY) / 2);
                    if (touchedView) {
                        touchedView->dispatchInputEvent(INPUT_EVENT_FLING, source, time, velocity);
                    }
                    break;
                }
            }
        }
        //touchedView = NULL;
        timeOfDownEvent = 0;

    }

}

void Window::dispatchInputEvent(int event, int source, TIMESTAMP time, int x, int y) {
	//app.log("Window::dispatchTouch %d %d %d %d", event, source, x, y);
    POINT pt = POINT_Make(x, y);

    if (SOURCE_TYPE(source)==INPUT_SOURCE_TYPE_MOUSE || SOURCE_TYPE(source)==INPUT_SOURCE_TYPE_FINGER) {

        // Start or lookup motion tracker
        MotionTracker *tracker = NULL;
        for (auto it=_motionTrackers.begin() ; it!=_motionTrackers.end() ; it++) {
            if ((*it)->source == source) {
                tracker = *it;
                break;
            }
        }
        if (!tracker) {
            tracker = new MotionTracker(source);
            //glInsertEventMarkerEXT(0, "com.apple.GPUTools.event.debug-frame");
            _motionTrackers.push_back(tracker);
        }

        // Let the tracker process the new input event
        tracker->dispatchInputEvent(event, time, pt, this);

    }
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
	ViewController* rootVC = _rootViewController;
	if (rootVC) {
		View* view = rootVC->getView();
		if (!_viewLayoutValid) {
			//app.log("Performing measure() and layout() %d", _surfaceRect.size.width);
			_inLayoutPass = true;
			view->measure(_surfaceRect.size.width, _surfaceRect.size.height);
			view->layout();
			_viewLayoutValid = true;
			_inLayoutPass = false;
		}
        
//        std::this_thread::sleep_for (std::chrono::milliseconds(100));
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

void Window::setNeedsLayout() {
	if (_inLayoutPass) {
		return;
	}
	_viewLayoutValid = false;
	requestRedraw();
}


void Window::requestRedraw() {
	if (_redrawNeeded) {
		return;
	}
	_redrawNeeded = true;
    app.requestRedraw();
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

bool Window::setFocusedView(View* view) {
    if (_focusedView == view) {
        return false;
    }
    if (_focusedView) {
        _focusedView->setState({STATE_FOCUSED, 0});
    }
    _focusedView = view;
    if (view) {
        view->setState({STATE_FOCUSED, STATE_FOCUSED});
        _keyboardHandler = view->getKeyboardInputHandler();
        if (_keyboardHandler != NULL) {
            app.keyboardShow(true);
        }
    } else {
         app.keyboardShow(false);
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
void Window::glEnableScissorTest(bool enabled) {
    if (enabled && !_enabledFlags.scissorTest) {
        _enabledFlags.scissorTest = 1;
        check_gl(glEnable, GL_SCISSOR_TEST);
    }
    else if (!enabled && _enabledFlags.scissorTest) {
        _enabledFlags.scissorTest = 0;
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
        check_gl(glClearColor, 1,0,0,1);
        check_gl(glClear, GL_COLOR_BUFFER_BIT);
        check_gl(glDisable, GL_DEPTH_TEST);
        check_gl(glActiveTexture, GL_TEXTURE0);
        _blendMode = BLENDMODE_NONE;
        check_gl(glDisable, GL_BLEND);
        _enabledFlags.scissorTest = 0;
        check_gl(glDisable, GL_SCISSOR_TEST);

        // As long as we only have one quadbuffer we only need to bind the once
        _quadBuffer->bind();
        _currentVertexConfig = 0;
    }

    _renderCounter++;
    _currentSurface = NULL;
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
    check_gl(glVertexAttribPointer, VERTEXATTRIB_COLOUR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VERTEX), (void*)16);

    check_gl(glEnableVertexAttribArray, VERTEXATTRIB_POSITION);
    check_gl(glEnableVertexAttribArray, VERTEXATTRIB_TEXCOORD);
    check_gl(glEnableVertexAttribArray, VERTEXATTRIB_COLOUR);

}

void Window::bindTexture(Bitmap* texture) {
    if (texture&& _currentTexture != texture) {
        _currentTexture = texture;
        texture->bind();
    }

}




