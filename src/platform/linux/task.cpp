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
    task->complete();
    return 0;
}

class TaskLinux : public Task {
public:
    guint _id;

    TaskLinux(std::function<void()> func, int delay) : Task(func) {
        GMainContext* context = g_main_context_get_thread_default ();
        GSource* source = (delay<=0) ? g_idle_source_new() : g_timeout_source_new(delay);
        g_source_set_callback (source, mainThreadThunk, this, NULL);
        _id = g_source_attach (source, context);
        g_source_unref (source);
    }
};

Task* App::postToMainThread(std::function<void ()> func, int delay/*=0*/) {
    return new TaskLinux(func, delay);
}

#endif

