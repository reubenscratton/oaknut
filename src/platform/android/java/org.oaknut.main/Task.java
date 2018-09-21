package org.oaknut.main;


import java.util.concurrent.Future;


class Task implements Runnable {
    final long nativeObj;
    Future<?> future;

    Task(long nativeObj) {
        this.nativeObj = nativeObj;
    }

    @Override
    public void run() {
        nativeRun(nativeObj);
    }

    native void nativeRun(long nativeObj);

    public void cancel() {
        if (future != null) {
            future.cancel(true);
            future = null;
        }
    }

}

