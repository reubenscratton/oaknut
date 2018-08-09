//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

Task::Task(TASKFUNC func) : _func(func) {
}

void Task::exec() {
    _func();
}

bool Task::cancel() {
    bool cancelled = true;
    if (_queue) {
        cancelled = _queue->cancelTask(this);
        _queue = NULL;
    }
    return cancelled;
}

TaskQueue::TaskQueue(const string& name) : _name(name) {
}


