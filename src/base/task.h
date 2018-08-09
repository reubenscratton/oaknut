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
    bool cancel();

    static Task* create(TASKFUNC func);
    static void ensureSharedGLContext(); // use this if you need to use GL from a background thread (i.e. image processing)

protected:
    Task(TASKFUNC func);

    ObjPtr<class TaskQueue> _queue;
    TASKFUNC _func;

};

class TaskQueue : public Object {
public:

    static TaskQueue* create(const string& name);
    static void postToMainThread(TASKFUNC task);

    virtual void enqueueTask(Task* task)=0;
    virtual bool cancelTask(Task* task)=0;

protected:
    TaskQueue(const string& name);

    string _name;
    vector<ObjPtr<Task>> _tasks;

};
