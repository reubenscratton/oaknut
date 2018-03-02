//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class TaskQueue : public Object {
public:

    static TaskQueue* create(const string& name);
    static void postToMainThread(std::function<void(void)> task);

    virtual int enqueueTask(std::function<void(void)> task)=0;
    virtual bool cancelTask(int taskId)=0;

protected:
    TaskQueue(const string& name);

    string _name;
};
