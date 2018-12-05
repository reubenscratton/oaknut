//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if PLATFORM_LINUX

#include <oaknut.h>


static int mainThreadThunk(void* data) {
    Task* task = (Task*)data;
    task->exec();
    task->release();
    return 0;
}

void App::postToMainThread(std::function<void ()> func, int delay/*=0*/) {
    Task* task = new PosixTaskQueue::PosixTask(func);
    task->retain();
    g_idle_add(mainThreadThunk, task);
}

#endif

