//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup base_group
 * @class Task
 * @brief A simple API for async background operations.
 *
 * A `Task` is a chain of 1 or more lambda 'subtasks' that execute in sequence. Each subtask
 * takes a `variant` as input and returns another `variant` as output, i.e. the output of one
 * subtask becomes the input of the next.
 *
 * A subtask is specified to execute in one of three thread contexts: Main, Background, and IO.
 * Main is self-explanatory, the last subtask in the Task usually runs on the Main thread, and
 * updates UI with the result of the operations in preceding subtasks.
 * Background. The runtime keeps a small pool of threads (one per core) for CPU-intensive
 * work that should not perform IO or block. Common subtasks sent to Background are image
 * decompression, text parsing, decryption, and so on.
 * IO is a larger pool of threads intended for jobs that perform IO and will therefore spend
 * most of their time suspended until IO completes. The IO thread pool grows and shrinks dynamically.
 *
 * As an example, the code to load a file from disk then process the file contents into a usable form
 * and then finally update the UI would look something like this:
 *
 *    Task* task = Task::enqueue({
 *         {Task::IO, [=](variant& input) -> variant {
 *             return loadFileIntoMemory(input);
 *         },
 *         {Task::Background, [=](variant& input)  -> variant {
 *             return processLoadedFile(input);
 *         },
 *         {Task::Main, [=](variant& input)  -> variant {
 *             updateUI();
 *         }
 *     });
 * }
 *
 * Tasks are cancellable (see `Task::cancel()`) so background subtasks should periodically
 * check `isCancelled()` and early exit if appropriate.
 *
 * Tasks can optionally hold a strong reference on an 'owner' object, this will typically be
 * whatever object needs to stay alive until the Task completes.
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
    Task(const vector<subtask>& subtasks, Object* owner=nullptr);
    void enqueueNextSubtask(const variant& input);
    void cancel();
    bool isCancelled() const {
        return _status == status::Cancelled;
    }
    ~Task();
    
    // Helpers
    static Task* enqueue(const vector<subtask>& subtasks, Object* owner=nullptr);
    static bool isMainThread();
    static void postToMainThread(std::function<void()> callback, int delay=0);

protected:

    std::atomic<status> _status;
    vector<subtask> _subtasks;
    sp<Object> _owner;
    
    friend class ThreadPool;
};



