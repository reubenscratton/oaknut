package org.oaknut.main;


public class Timer implements Runnable {

    long nativeObj;
    long delay;
    boolean repeats;

    public static Timer schedule(long nativeObj, long delay, boolean repeats) {
        Timer timer = new Timer();
        timer.nativeObj = nativeObj;
        timer.delay = delay;
        timer.repeats = repeats;
        App.handler.postDelayed(timer, delay);
        return timer;
    }

    public void unschedule() {
        App.handler.removeCallbacks(this);
    }

    @Override
    public void run() {
        if (repeats) {
            App.handler.postDelayed(this, delay);   // todo: correct for drift
        }
        nativeDispatch(nativeObj);
    }

    static native void nativeDispatch(long nativeObj);

}
