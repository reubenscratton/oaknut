package org.oaknut.main;


import android.os.Handler;
import android.os.Looper;

import java.util.concurrent.Executor;
import java.util.concurrent.Executors;

public class Async {


    Executor executor;
    static Async asyncMainThread = new Async();
    static Handler handler;
    static {
        handler = new Handler(Looper.getMainLooper());
    }

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

    public static void runOnMainThread(int delay, final long callback) {
        if (delay > 0) {
            handler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    asyncMainThread.enqueue(callback);

                }
            }, delay);
            return;
        }
        asyncMainThread.enqueue(callback);
    }

    private native void nativeRun(long nativeObj);
}
