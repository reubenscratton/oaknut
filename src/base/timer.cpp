//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


Timer::Timer(const std::function<void()>& del, int intervalMillis, bool repeats) : _del(del), _repeats(repeats) {
    _active = true; //
    retain(); // always keep a ref
}

void Timer::stop() {
    if (_active) {
        _active = false;
        release();
    }
}

Timer::~Timer() {
    assert(!_active); // oops! this timer is still active... it should be stop()ped
}
