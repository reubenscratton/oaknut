//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

semaphore::semaphore(uint32_t value/*=0*/) {
#ifdef __APPLE__
    _sem = dispatch_semaphore_create(value);
#else
    sem_init(&_sem, 0, value);
#endif
}

semaphore::~semaphore() {
#ifdef __APPLE__
    _sem = nil;
#else
    sem_destroy(&_sem);
#endif
}

// Returns true if semaphore aquired, false if timed out or errored
bool semaphore::wait(int timeoutMillis /*=-1*/) {
#ifdef __APPLE__
    return dispatch_semaphore_wait(_sem, (timeoutMillis<0)?DISPATCH_TIME_FOREVER:(dispatch_time(DISPATCH_TIME_NOW, timeoutMillis*1000000LL)))==0;
#else
    int r;
    if (timeoutMillis<0) {
        do {
                r = sem_wait(&sem);
        } while (r == -1 && errno == EINTR);
    } else {
        struct timespec ts;
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)    {
            return false;
        }
        ts.tv_sec += (timeoutMillis/1000);
        ts.tv_nsec += (timeoutMillis%1000) * 1000000;
        while ((r = sem_timedwait(&sem, &ts)) == -1 && errno == EINTR)
            continue;
    }
    return (r == 0);
#endif
}

void semaphore::signal() {
#ifdef __APPLE__
    dispatch_semaphore_signal(_sem);
#else
    sem_post(&_sem);
#endif
}
