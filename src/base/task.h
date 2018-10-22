//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

typedef std::function<void(void)> TASKFUNC;

class Task : public Object {
public:
    void exec();
    virtual bool cancel()=0;

    static void ensureSharedGLContext(); // use this if you need to use GL from a background thread (i.e. image processing)
    static void postToMainThread(TASKFUNC func, int delay=0);

protected:
    Task(TASKFUNC func);

    sp<class TaskQueue> _queue;
    TASKFUNC _func;
};

class TaskQueue : public Object {
public:

    static TaskQueue* create(const string& name);

    virtual Task* enqueueTask(TASKFUNC task)=0;

protected:
    TaskQueue(const string& name);

    string _name;
    vector<sp<Task>> _tasks;
};
