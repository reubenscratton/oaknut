//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_LINUX

#include <oaknut.h>

TIMESTAMP App::currentMillis() {
    struct timeval te;
    gettimeofday(&te, NULL);
    TIMESTAMP milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return milliseconds;
}


class OSTimer : public Timer {
public:
    OSTimer(const TimerDelegate& del, int intervalMillis, bool repeats) : Timer(del, intervalMillis, repeats) {
        _timer = gdk_threads_add_timeout(intervalMillis, TimerFunc, this);
    }
    ~OSTimer() {
    }

    static gboolean TimerFunc(gpointer user_data) {
        OSTimer* timer = (OSTimer*)user_data;
        dispatch(timer);
        return timer->_repeats;
    }

    void stop() {
        Timer::stop();
        if (_timer) {
            g_source_remove(_timer);
            _timer = 0;
        }
    }

    static void dispatch(OSTimer* timer) {
        if (!timer->_repeats) {
            timer->stop();
        }
        timer->_del();
    }

protected:
    guint _timer;
};


Timer* Timer::start(const TimerDelegate& del, int intervalMillis, bool repeats) {
    return new OSTimer(del, intervalMillis, repeats);
}

#endif
