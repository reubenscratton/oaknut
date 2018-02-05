//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_LINUX

#include "oaknut.h"
#include <sys/time.h>

long oakCurrentMillis() {
    struct timeval te;
    gettimeofday(&te, NULL);
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return (long)milliseconds;
}

string oakGetAppHomeDir() {
    return string("."); // will this work in AppImage?
}



Data* oakLoadAsset(const char* assetPath) {
    
    string str = "assets/";
    str.append(assetPath);
    FILE* asset = fopen(str.data(), "rb");
    if (!asset) {
        oakLog("Failed to open asset: %s", assetPath);
        return NULL;
    }
    
    Data* data = new Data();
    fseek (asset, 0, SEEK_END);
    data->cb = ftell(asset);
    data->data = (uint8_t*) malloc (sizeof(char)*data->cb);
    fseek ((FILE*)asset, 0, SEEK_SET);
    size_t read = fread(data->data, 1, data->cb, (FILE*)asset);
    assert(read == data->cb);
    fclose(asset);
    return data;
    
}


static bool s_signalInit;


class OSTimer : public Timer {
public:
    OSTimer(const TimerDelegate& del, int intervalMillis, bool repeats) : Timer(del, intervalMillis, repeats) {
        if (!s_signalInit) {
            s_signalInit = true;
            struct sigaction sa;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = SA_SIGINFO;
            sa.sa_sigaction = TimerHandlerCB;
            int rv = sigaction(SIGRTMAX, &sa, NULL);
            assert(rv == 0);
        }
        _sigevent.sigev_notify = SIGEV_SIGNAL;
        _sigevent.sigev_signo = SIGRTMAX;
        _sigevent.sigev_value.sival_ptr = this;
        int rv = timer_create(CLOCK_MONOTONIC, &_sigevent, &_timer);
        assert(rv==0);

    }

    static void TimerHandlerCB(int signal, siginfo_t* siginfo, void* param) {
        OSTimer* timer = (OSTimer*)param;
        oakLog("TimerHandlerCB!");
    }

    void stop() {
        if (_timer) {
            timer_delete(_timer);
        }
        Timer::stop();
    }

    static void dispatch(OSTimer* timer) {
        if (!timer->_repeats) {
            timer->stop();
        }
        timer->_del();
    }

protected:
    struct sigevent _sigevent;
    timer_t _timer;
};


Timer* Timer::start(const TimerDelegate& del, int intervalMillis, bool repeats) {
    return new OSTimer(del, intervalMillis, repeats);
}

#endif