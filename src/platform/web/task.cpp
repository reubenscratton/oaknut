//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>


static void mainThreadThunk(Task* task) {
    task->complete();
}



Task* App::postToMainThread(std::function<void(void)> func, int delay) {
    Task* task = new Task(func);
    if (emscripten_is_main_runtime_thread()) {
        EM_ASM({
            window.setTimeout(function() { dynCall('vi', $0, [$1]); }, $2);
        }, mainThreadThunk, task, delay);
    } else {
        assert(0); // below func is missing, its part of pthread support. What do we do??
        // emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, mainThreadThunk, task);
    }
    return task;
}

#endif

