//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup base_group
 * @class Task
 * @brief Base class for a cancelable background operation
 *
 */

class Task : public Object {
public:
    
    enum RunContext {
        MainThread,
        Background,
        IO
    };
    
    Task(RunContext context, std::function<variant(variant&)> func);

    void dispatch();
    
    typedef pair<RunContext, std::function<variant(variant&)>> spec;
    
    static Task* enqueue(const vector<spec>& s);
    static void enqueue(Task* task);

    static bool isMainThread();

    enum status {
        Created,
        Queued,
        Executing,
        Cancelled,
        Complete
    };

    bool isCancelled() const {
        return _status == status::Cancelled;
    }

    virtual void cancel();
    

    // File helpers
    static variant fileLoad(const string& path);
    static error fileSave(const string& path, const bytearray& data);

protected:

    RunContext _runContext;
    variant _input, _output;
    std::function<variant(variant&)> _func;
    std::atomic<status> _status;
    sp<Task> _nextTask;
};



