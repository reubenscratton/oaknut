//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

typedef std::function<void(void)> TASKFUNC;

/**
 Task represents a cancelable operation that performs a callback
 on the main thread when finished (unless cancelled).

 NB: Task is *not* an API for scheduling background app work, see Worker for that.

 Task is not intended for instantiation in app code, they are
 typically created and returned by platform APIs. For example
 Bitmap::createFromData() returns a Task representing a background
 operation that decodes a bitmap.
 
 The Task returned by Task::postToMainThread() does not do any
 background work.
 
 */

class Task : public Object {
public:
    
    // API
    Task(TASKFUNC oncomplete);
    bool isCancelled() const;
    virtual void complete();
    virtual void cancel();
    

    static Task* postToMainThread(TASKFUNC func, int delay=0);

protected:

    TASKFUNC _oncomplete;
    bool _cancelled;
};

