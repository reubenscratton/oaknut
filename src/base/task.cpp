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

TaskQueue::TaskQueue(const string& name) : _name(name) {
}


