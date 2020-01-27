//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if PLATFORM_LINUX

#include <oaknut.h>

static int mainThreadThunk(void* data) {
    auto pfunc = (std::function<void()>*)data;
    *pfunc();
    delete pfunc;
    return 0;
}

void App::postToMainThread(std::function<void()> func, int delay/*=0*/) {
    GMainContext* context = g_main_context_get_thread_default ();
    GSource* source = (delay<=0) ? g_idle_source_new() : g_timeout_source_new(delay);
    std::function<void()>* cpy = new std::function<void()>(func);
    g_source_set_callback (source, mainThreadThunk, cpy, NULL);
    guint id = g_source_attach (source, context);
    g_source_unref (source);
}

#endif

