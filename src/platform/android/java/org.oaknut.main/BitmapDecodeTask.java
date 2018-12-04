package org.oaknut.main;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;


import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class BitmapDecodeTask implements Runnable {

    static ExecutorService executorService = Executors.newFixedThreadPool(4);

    long _cppTask;
    byte[] _bytes;
    Future<?> _future;
    boolean _cancelled;

    public BitmapDecodeTask(long cppTask, byte[] bytes) {
        _cppTask = cppTask;
        _bytes = bytes;
        _future = executorService.submit(this);
    }

    public void cancel() {
        _cancelled = true;
        if (_future != null) {
            _future.cancel(true);
        }
    }


    @Override
    public void run() {
        if (_cancelled) {
            return;
        }
        Bitmap bitmap = BitmapFactory.decodeByteArray(_bytes, 0, _bytes.length);
        if (_cancelled) {
            return;
        }
        TaskQueue.handler.post(new Runnable() {
            @Override
            public void run() {
                if (!_cancelled) {
                    nativeHandleDecodedBitmap(_cppTask, bitmap);
                }
            }
        });
    }

    private native void nativeHandleDecodedBitmap(long cppTask, Bitmap bitmap);
}
