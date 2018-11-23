//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_ANDROID || PLATFORM_LINUX

#include <oaknut.h>

class PosixTaskQueue : public Object {
public:
    pthread_t _pthread;
    pthread_cond_t _cond;
    pthread_mutex_t _mutex;
    vector<pair<int,std::function<void(void)>>> _tasks;
    int _counter;
    
    PosixTaskQueue(const string& name);
    ~PosixTaskQueue();
    
    int enqueueTask(std::function<void(void)> func);
    bool cancelTask(int taskId);
    void terminate();
    
    
    void threadFunc();
    
};

#endif
