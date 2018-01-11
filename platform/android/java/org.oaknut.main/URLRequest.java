package org.oaknut.main;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.concurrent.Future;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;


public class URLRequest implements Runnable {


    static LinkedBlockingQueue<Runnable> queue = new LinkedBlockingQueue<>();
    static ThreadPoolExecutor executor = new ThreadPoolExecutor(4, 4, 0, TimeUnit.MILLISECONDS, queue);

    long cobj;
    String url;
    Future<?> future;

    public URLRequest(long cobj, String url) {
        // NB: This is called on native-app-glue engine thread, not app thread.
        // NB: Not sure if the above comment is still true...
        this.cobj = cobj;
        this.url = url;
        future = executor.submit(this);
    }

    public void cancel() {
        future.cancel(true);
    }

    @Override
    public void run() {
        HttpURLConnection urlConnection = null;
        try {
            URL url = new URL(this.url);
            urlConnection = (HttpURLConnection) url.openConnection();
            // todo: upload request body
            InputStream inputStream = new BufferedInputStream(urlConnection.getInputStream());

            String contentType = urlConnection.getHeaderField("Content-Type");
            if ("image/jpeg".equalsIgnoreCase(contentType) || "image/png".equalsIgnoreCase(contentType)) {
                final Bitmap bitmap = BitmapFactory.decodeStream(inputStream);
                if (bitmap != null) {
                    App.handler.post(new Runnable() {
                        @Override
                        public void run() {
                            nativeOnGotBitmap(cobj, bitmap);
                        }
                    });
                    return;
                }
            } else {
                final ByteArrayOutputStream baos = new ByteArrayOutputStream();
                byte[] byteChunk = new byte[4096];
                int n;
                while ((n = inputStream.read(byteChunk)) > 0 ) {
                    baos.write(byteChunk, 0, n);
                }
                App.handler.post(new Runnable() {
                    @Override
                    public void run() {
                        nativeOnGotGenericData(cobj, baos.toByteArray());
                    }
                });
                return;
            }
        } catch (IOException e) {
            android.util.Log.e("Oaknut",  e.toString());
        } finally {
            if (urlConnection != null) {
                urlConnection.disconnect();
            }
        }
        // If we get here something went wrong
    }

    native void nativeOnGotBitmap(long cobj, Bitmap bitmap);
    native void nativeOnGotGenericData(long cobj, byte[] data);


}
