//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>





TaskQueue* TaskQueue::create(const string &name) {
    return NULL;
}




class Foo {
public:
    Foo(std::function<void(void)> func) : _func(func) {
    }
    
    std::function<void(void)> _func;
};

static void mainThreadThunk(Foo* foo) {
    foo->_func();
    delete foo;
}



void Task::postToMainThread(TASKFUNC func, int delay) {
    void* foo = new Foo(func);
    if (emscripten_is_main_runtime_thread()) {
        EM_ASM({
            window.setTimeout(function() { Runtime.dynCall('vi', $0, [$1]); }, $2);
        }, mainThreadThunk, foo, delay);
    } else {
        emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, mainThreadThunk, foo);
    }
}
#endif

