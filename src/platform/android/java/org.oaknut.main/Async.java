package org.oaknut.main;


import java.util.concurrent.Executor;
import java.util.concurrent.Executors;

public class Async {

    Executor executor;
    static Async asyncMainThread = new Async();

    public Async() {
        executor = Executors.newSingleThreadExecutor();
    }

    public void enqueue(final long nativeObj) {
        executor.execute(new Runnable() {
            public void run() {
                nativeRun(nativeObj);
            }
        });
    }

    public static void runOnMainThread(final long callback) {
        asyncMainThread.enqueue(callback);
    }

    private native void nativeRun(long nativeObj);
}
