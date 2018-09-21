package org.oaknut.main;


import android.os.Handler;
import android.os.Looper;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;


public class TaskQueue {


    ExecutorService executorService;
    static Handler handler;
    static {
        handler = new Handler(Looper.getMainLooper());
    }

    public TaskQueue() {
        executorService = Executors.newSingleThreadExecutor();
    }


    public Task enqueue(final long nativeObj) {
        Task task = new Task(nativeObj);
        task.future = executorService.submit(task);
        return task;
    }

    public static void runOnMainThread(int delay, final long callback) {
        if (delay > 0) {
            handler.postDelayed(new Task(callback), delay);
            return;
        }
        handler.post(new Task(callback));
    }

}
