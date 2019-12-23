//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#import <oaknut.h>


@interface TimerTarget : NSObject
- (void)timerEvent:(NSTimer*)timer;
@end

static TimerTarget* s_timerTarget;


class OSTimer : public Timer {
public:
    OSTimer(const std::function<void()>& del, int intervalMillis, bool repeats) : Timer(del, intervalMillis, repeats) {
        _timer = [NSTimer scheduledTimerWithTimeInterval:intervalMillis/1000.0 target:s_timerTarget selector:@selector(timerEvent:) userInfo:[NSValue valueWithPointer:this] repeats:repeats];
        //del->_osobj = (void*)CFBridgingRetain(timer);
    }
        
    void stop() override {
        if (_timer) {
            [_timer invalidate];
            _timer = NULL;
        }
        Timer::stop();
    }
    
    void dispatch() {
        _del();
    }
    
protected:
    NSTimer* _timer;
};

@implementation TimerTarget

+ (void)load {
    s_timerTarget = [TimerTarget new];
}
- (void)timerEvent:(NSTimer*)timer {
    OSTimer* ostimer = (OSTimer*)((NSValue*)timer.userInfo).pointerValue;
    ostimer->dispatch();
}

@end

Timer* Timer::start(const std::function<void()>& del, int intervalMillis, bool repeats) {
    return new OSTimer(del, intervalMillis, repeats);
}

#endif
