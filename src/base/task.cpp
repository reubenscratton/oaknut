//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

Task::Task(std::function<void(void)> oncomplete) : _oncomplete(oncomplete) {
    retain();
}

void Task::cancel() {
    _cancelled = true;
}

void Task::complete() {
    if (!_cancelled) {
        _oncomplete();
    }
    release();
}

bool Task::isCancelled() const {
    return _cancelled;
}


