//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"




Window::Window() : _rootViewController(NULL), _scale(1) {
	_canvas = new Canvas();
    _surface = new Surface();
}



void Window::setRootViewController(ViewController* viewController) {
	ViewController* currentRootVC = _rootViewController;
	if (currentRootVC) {
		viewController->onWillPause();
		currentRootVC->detachFromWindow();
		viewController->onDidPause();
	}
	_rootViewController = viewController;
	if (viewController) {
		viewController->onWillResume();
			_canvas->prepareToDraw();

		viewController->attachToWindow(this);
		viewController->onDidResume();
	}
}

void Window::resizeSurface(int width, int height, float scale) {
    if (_surface->_size.width==width && _surface->_size.height==height) {
        return;
    }
	//oakLog("Window::resize %d %d %d", width, height, scale);
    _surfaceRect = RECT_Make(0,0,width,height);
	_scale = scale;
    _surface->setSize(SIZE_Make(width, height));
	ViewController* rootVC = _rootViewController;
	if (rootVC) {
		rootVC->_view->setNeedsLayout();
	}
}


void Window::dispatchTouchEvent(int event, int finger, long time, int x, int y) {
	//oakLog("Window::dispatchTouch %d %d %d %d", event, finger, x, y);
	ViewController* rootVC = _rootViewController;
    x *= _scale;
    y *= _scale;
	POINT pt = POINT_Make(x, y);
	TOUCH& touch = _touches[finger];
    
    // Record point history for velocity calculation
    if (event == TOUCH_EVENT_DOWN) {
        //glInsertEventMarkerEXT(0, "com.apple.GPUTools.event.debug-frame");
        touch.pastIndex = touch.pastCount = 0;
		touch.ptDown = pt;
		touch.isDragging = false;
		if (time - touch.timeOfDownEvent >= DBLCLICK_THRESHOLD) {
			touch.numClicks = 0;
		}
		touch.timeOfDownEvent = time;
    }
    if (event == TOUCH_EVENT_DOWN || event==TOUCH_EVENT_MOVE) {
        if (event==TOUCH_EVENT_MOVE) { // filter out spurious move events (seen on iOS 10)
            int prevIndex = touch.pastIndex-1;
            if (prevIndex<0) prevIndex+=NUM_PAST;
            if (touch.pastPts[prevIndex].equals(pt)) {
                touch.pastTime[prevIndex] = time;
                return;
            }
        }
        touch.pastTime[touch.pastIndex] = time;
        touch.pastPts[touch.pastIndex] = pt;
        touch.pastIndex = (touch.pastIndex+1) % NUM_PAST;
        touch.pastCount++;
    }
 
	if (event == TOUCH_EVENT_DOWN) {
		touch.touchedView = rootVC->_view->dispatchTouchEvent(TOUCH_EVENT_DOWN, finger, time, pt);
	}
	else if (event == TOUCH_EVENT_MOVE) {
		if (!touch.isDragging) {
			float dx = x - touch.ptDown.x;
			float dy = y - touch.ptDown.y;
			float dist = sqrtf(dx*dx + dy*dy);
			if (idp(dist) >= TOUCH_SLOP) {
				touch.isDragging = true;
				View* interceptView = rootVC->_view->dispatchTouchEvent(TOUCH_EVENT_DRAG, finger, time, pt);
				if (interceptView) {
					if (touch.touchedView) {
						touch.touchedView->dispatchTouchEvent(TOUCH_EVENT_CANCEL, finger, time, pt);
					}
					touch.touchedView = interceptView;
				}
			}
		}
		if (touch.touchedView) {
			//POINT offset = offsetToView(touch.touchedView);
			//pt.x -= offset.x;
			//pt.y -= offset.y;
			touch.touchedView->dispatchTouchEvent(TOUCH_EVENT_MOVE, finger, time, pt);
		}
	}
	else if (event == TOUCH_EVENT_UP) {
		if (touch.touchedView) {
			//POINT offset = offsetToView(touch.touchedView);
			//pt.x -= offset.x;
			//pt.y -= offset.y;
			touch.touchedView->dispatchTouchEvent(TOUCH_EVENT_UP, finger, time, pt);
		}
		if (!touch.isDragging) {
			// TAP!
			touch.numClicks++;
			if (touch.touchedView) {
				touch.touchedView->dispatchTouchEvent(TOUCH_EVENT_TAP, finger, time, pt);
			}
		} else {
			// FLING!
                
			// Work out the drag velocity by getting the distance travelled from the oldest historical point
			// and extrapolating an average distance per second.
			int numPoints = min(touch.pastCount, NUM_PAST);
			int index = touch.pastIndex - numPoints;
			if (index<0) index += NUM_PAST;
			while (numPoints-- > 1) {
				POINT ptFrom = touch.pastPts[index];
				long timeFrom = touch.pastTime[index];
				index = (index+1) % NUM_PAST;
				if (time-timeFrom >= 333) { // ignore historical points that are too old
					continue;
				}
				int dTime = (int)(time - timeFrom);
				if (dTime <= 0) continue;
				float dx = pt.x - ptFrom.x;
				float dy = pt.y - ptFrom.y;
				float thisVeloX = dx * 1000.0f/dTime;
				float thisVeloY = dy * 1000.0f/dTime;
                POINT velocity = {0,0};
				velocity.x = (velocity.x==0) ? thisVeloX : ((velocity.x + thisVeloX)/2);
				velocity.y = (velocity.y==0) ? thisVeloY : ((velocity.y + thisVeloY)/2);
                if (touch.touchedView) {
                    touch.touchedView->dispatchTouchEvent(TOUCH_EVENT_FLING, finger, time, velocity);
                }
                break;
			}
		}

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
    
#if IOS_WEBGL_BUG
	_oddFrame = !_oddFrame;
#endif


	_canvas->prepareToDraw();

	// Draw view controllers
	ViewController* rootVC = _rootViewController;
	if (rootVC) {
		View* view = rootVC->_view;
		if (!_viewLayoutValid) {
			//oakLog("Performing measure() and layout() %d", _surfaceRect.size.width);
			_inLayoutPass = true;
			view->measure(_surfaceRect.size.width, _surfaceRect.size.height);
			view->layout();
			_viewLayoutValid = true;
			_inLayoutPass = false;
		}
        
        _surface->render(view, _canvas);
	}

	
	incFrames();

    // Tick animations
    long now = oakCurrentMillis();
    auto it = _animations.begin();
    while (it != _animations.end()) {
        Animation* anim = *it;
        if (anim->_state == ANIMATION_STATE_STOPPED) {
            _animations.erase(it++);
        } else {
            anim->tick(now);
            it++;
        }
    }
    
    Object::flushAutodeletePool();

    // If there are any animations running, request a redraw immediately
    if (_animations.size()) {
        requestRedraw();
    }
    
}

void Window::setNeedsLayout() {
	if (_inLayoutPass) {
		return;
	}
	_viewLayoutValid = false;
	requestRedraw();
}

#if EMSCRIPTEN
EMSCRIPTEN_KEEPALIVE
extern "C" void dispatchMainWindowDraw() {
    mainWindow->draw();
}
#endif

void Window::requestRedraw() {
	if (_redrawNeeded) {
		return;
	}
	_redrawNeeded = true;
#if EMSCRIPTEN
    EM_ASM({ requestAnimationFrame( function() { _dispatchMainWindowDraw(); }) });
#else
    oakRequestRedraw();
#endif
}

POINT Window::offsetToView(View* view) {
	POINT pt = {0,0};
	ViewController* rootVC = _rootViewController;
	while (view && view != rootVC->_view) {
		pt.x += view->_frame.origin.x;
		pt.y += view->_frame.origin.y;
		view = view->_parent;
	}
	return pt;
}

bool Window::setFirstResponder(View* view) {
    if (_firstResponder == view) {
        return false;
    }
    _firstResponder = view;
    if (view) {
        _keyboardHandler = view->getKeyboardInputHandler();
        if (_keyboardHandler != NULL) {
            oakKeyboardShow(true);
        }
    } else {
         oakKeyboardShow(false);
    }
    return true;
}





