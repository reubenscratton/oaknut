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
    int priority;

    enum exec_context {
        MainThread,
        Background,
        IO
    };
    
    enum status {
        Created,
        Queued,
        Executing,
        Cancelled,
        Complete
    };

    struct subtask {
        exec_context _threadType;
        std::function<variant(variant&)> _func;
        variant _input, _output;
        sp<Task> task;
        bool operator<(const subtask& rhs) const {
            return task->priority<rhs.task->priority;
        }
    private:
        void exec();
        friend class ThreadPool;
        friend class Task;
    };

    // API
    Task(const vector<subtask>& subtasks);
    void enqueueNextSubtask(const variant& input);
    void cancel();
    bool isCancelled() const {
        return _status == status::Cancelled;
    }
    ~Task();
    
    // Helpers
    static Task* enqueue(const vector<subtask>& subtasks);
    static bool isMainThread();
    static void postToMainThread(std::function<void()> callback, int delay=0);
    static void flushCurrentThread();
    static void addObjectToCurrentThreadDeletePool(Object*);

    // File helpers
    static variant fileLoad(const string& path);
    static variant fileLoad(int fd);
    static error fileSave(const string& path, const bytearray& data);

protected:

    std::atomic<status> _status;
    vector<subtask> _subtasks;
    
    friend class ThreadPool;
};



